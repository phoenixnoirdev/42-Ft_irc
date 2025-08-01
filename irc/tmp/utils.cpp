
#include "IrcLib.hpp"

#define PORT_MAX 65535

/*PARSING*/
/*convert a string <port> passed by argument to a int*/

bool PortConvert(const std::string& arg, uint16_t& outPort)
{
	int port;

	std::stringstream ss(arg);
	/*return bool to stringstream*/
	/*verify if its a valid int*/
	if (!(ss >> port) || !ss.eof())
    {
		std::cerr << "Error: port conversion fail" << std::endl;
		return (false);
	}

	if (port < 1 || port > PORT_MAX)// 
	{
		std::cerr << "Port out of range" << std::endl;
		return (false);
	}
	outPort = static_cast<uint16_t>(port);
		return (true);
}

/*bool IpConvert(const std::string& ipStr, struct in_addr& outAddr)
{
	//  hint.s_addr = inet_addr("132.241.5.10");  inet_addr only work with literal ip's not  hostname like gethostbyname()
    struct hostent* he = gethostbyname(ipStr.c_str());
    if (!he || he->h_addrtype != AF_INET)
    {
        std::cerr << "Error: invalid IP or hostname: " << ipStr << std::endl;
        return false;
    }

    std::memcpy(&outAddr, he->h_addr, sizeof(struct in_addr));
    return true;
}*/

bool IpConvert(const std::string& ipStr, struct in_addr& outAddr)
{
    struct addrinfo hints;
    struct addrinfo* res = NULL;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // Somente IPv4
    hints.ai_socktype = SOCK_STREAM; // Tipo de socket, opcional

    int status = getaddrinfo(ipStr.c_str(), NULL, &hints, &res);
    if (status != 0 || !res)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return false;
    }

    // Extrai o endereço IP da struct sockaddr_in
    struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
    outAddr = ipv4->sin_addr;

    freeaddrinfo(res); // Libera a memória alocada
    return true;
}

/* getaddrinfo() return a pointer to addrinfo struct
struct addrinfo {
    int              ai_flags;      // Opções para a busca (ex: AI_PASSIVE, AI_CANONNAME)
    int              ai_family;     // Família de endereço (AF_INET, AF_INET6, AF_UNSPEC)
    int              ai_socktype;   // Tipo de socket (SOCK_STREAM, SOCK_DGRAM)
    int              ai_protocol;   // Protocolo (IPPROTO_TCP, IPPROTO_UDP, etc)
    socklen_t        ai_addrlen;    // Tamanho da estrutura de endereço
    struct sockaddr *ai_addr;       // Ponteiro para a estrutura de endereço (sockaddr_in, sockaddr_in6, etc)
    char            *ai_canonname;  // Nome canônico do host (se solicitado)
    struct addrinfo *ai_next;       // Próximo elemento na lista encadeada
};
*/

/*****to understand how sockaddr_in struct works ******

struct sockaddr_in
{
	short				sin_family;		// Ex: AF_INET
    unsigned			short sin_port; // Porta (htons()!)
    struct in_addr		sin_addr;  		// <-- este é o campo que representa o IP
    char 				sin_zero[8];	//padding
};

*****e a estrutura sin_addr que 'e chamada na struct sockaddr_in ******

struct in_addr
{
    in_addr_t			s_addr;			// IP address (em binário)
};
*/



