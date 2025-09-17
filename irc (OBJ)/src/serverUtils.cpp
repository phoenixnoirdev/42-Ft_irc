
//#include "Commands.hpp"
# include "../inc/inc.hpp"


// serverUtils.cpp
#include "../inc/server.hpp"   // garante a declaração do Server
#include <sstream>

void Server::kickUser(const std::string& nickToKick, int kickerFd, const std::string& reason)
{
    // 0) valida kicker
    std::map<int, User>::iterator itK = _User.find(kickerFd);
    if (itK == _User.end())
        return; // kicker já desconectou

    User &kicker = itK->second;

    // 1) marca ban por nick (global simples)
    getBanList().addBan(nickToKick);

    // 2) encontra vítima (se online)
    int victimFd = -1;
    std::map<int, User>::iterator itV = _User.end();
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it) {
        if (it->second.getNick() == nickToKick) { victimFd = it->first; itV = it; break; }
    }

    // 3) feedback ao kicker (sempre)
    {
        std::string line = ":" + kicker.getNick() + "!~" + kicker.getName() +
                           "@localhost KICK #default " + nickToKick + " :" +
                           (reason.empty() ? "" : reason) + "\r\n";
        ::send(kickerFd, line.c_str(), line.size(), 0);
    }

    // 4) vítima não está online — só informa e sai
    if (victimFd == -1) {
        std::string note = ":server NOTICE " + kicker.getNick() +
                           " :User " + nickToKick + " not online; ban set\r\n";
        ::send(kickerFd, note.c_str(), note.size(), 0);
        return;
    }

    // 5) avisa a vítima ANTES de fechar
    {
        const User &victim = itV->second;
        std::string bye = ":server NOTICE " + victim.getNick() +
                          " :You were KICKed (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
        ::send(victimFd, bye.c_str(), bye.size(), 0);
    }

    // 6) remove a vítima de todos os canais (se tiver API; senão, ignore)
    for (std::map<int, Channel>::iterator c = _Chan.begin(); c != _Chan.end(); ++c) {
        // se o Channel tiver algo como HasUser/RemoveUser use aqui:
        // if (c->second.HasUser(victimFd)) c->second.RemoveUser(victimFd);
    }

    // 7) fecha socket e limpa mapas/FDs — sem tocar mais em 'victim' depois do erase
    itV->second.closeSocket();                      // fecha fd
    FD_CLR(victimFd, &_Readfds);                    // limpa do set
    _User.erase(victimFd);                          // apaga do mapa (não use 'victim' depois daqui)
}


void Server::handleKickCommand(int clientSocket, const std::string& line)
{
    // 0) valida kicker
    std::map<int, User>::iterator itK = _User.find(clientSocket);
    if (itK == _User.end())
        return; // kicker não existe mais
    User &kicker = itK->second;

    // 1) parse: KICK #canal nick [:motivo]
    std::string chanName, victimNick, reason;

    std::string tmp = line.substr(5); // após "KICK "
    std::string::size_type sp1 = tmp.find(' ');
    if (sp1 == std::string::npos) {
        std::string err = ":server 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
    chanName = tmp.substr(0, sp1);

    std::string rest = tmp.substr(sp1 + 1);
    std::string::size_type sp2 = rest.find(' ');
    if (sp2 == std::string::npos) {
        victimNick = rest;
    } else {
        victimNick = rest.substr(0, sp2);
        std::string::size_type colon = line.find(" :");
        if (colon != std::string::npos)
            reason = line.substr(colon + 2);
    }

    if (victimNick.empty()) {
        std::string err = ":server 461 " + kicker.getNick() + " KICK :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }

    // 2) registra ban global por NICK
    getBanList().addBan(victimNick);

    // 3) encontra a vítima se estiver online
    int victimFd = -1;
    std::map<int, User>::iterator itV = _User.end();
    for (std::map<int, User>::iterator it = _User.begin(); it != _User.end(); ++it) {
        if (it->second.getNick() == victimNick) { victimFd = it->first; itV = it; break; }
    }

    // 4) feedback ao kicker (podes trocar #default pelo nome real do teu canal se tiveres)
    {
        std::string reply = ":" + kicker.getNick() + "!~" + kicker.getName() +
                            "@localhost KICK " + chanName + " " + victimNick +
                            " :" + (reason.empty() ? "" : reason) + "\r\n";
        ::send(clientSocket, reply.c_str(), reply.size(), 0);
    }

    // 5) se a vítima não está online, só informa
    if (victimFd == -1) {
        std::string note = ":server NOTICE " + kicker.getNick() +
                           " :User " + victimNick + " not online; ban set\r\n";
        ::send(clientSocket, note.c_str(), note.size(), 0);
        return;
    }

    // 6) avisa a vítima antes de fechar
    {
        const User &victim = itV->second;
        std::string bye = ":server NOTICE " + victim.getNick() +
                          " :You were KICKed (" + (reason.empty() ? "no reason" : reason) + ")\r\n";
        ::send(victimFd, bye.c_str(), bye.size(), 0);
    }

    // 7) (opcional) remover de canais aqui, se tiver API para isso

    // 8) fecha socket e limpa estruturas (NÃO usa 'victim' depois do erase)
    itV->second.closeSocket();
    FD_CLR(victimFd, &_Readfds);
    _User.erase(victimFd);
}

void Server::handleUnbanCommand(int clientSocket, const std::string& line)
{
    std::map<int, User>::iterator itK = _User.find(clientSocket);
    if (itK == _User.end())
        return;
    User &who = itK->second;

    // UNBAN <nick>
    if (line.size() <= 6) {
        std::string err = ":server 461 " + who.getNick() + " UNBAN :Not enough parameters\r\n";
        ::send(clientSocket, err.c_str(), err.size(), 0);
        return;
    }
    std::string nick = line.substr(6);
    // tira espaços no fim/início (simples)
    while (!nick.empty() && (nick[0]==' '||nick[0]=='\t')) nick.erase(0,1);
    while (!nick.empty() && (nick[nick.size()-1]==' '||nick[nick.size()-1]=='\t')) nick.erase(nick.size()-1);

    bool ok = getBanList().removeBan(nick);
    std::string ans = ":server NOTICE " + who.getNick() + " :UNBAN " + nick + (ok ? " ok" : " (not banned)") + "\r\n";
    ::send(clientSocket, ans.c_str(), ans.size(), 0);
}
