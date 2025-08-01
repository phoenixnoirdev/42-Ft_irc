
#include "inc.hpp"

//    🐓 functions dans le repertoire  tmp utils.cpp
	/*convert Port in **function mainUtils.cpp** with parsing*/


int server(int argc, char **argv)
{
    char		host[NI_MAXHOST];//1024	🐓 
	char		svc[NI_MAXSERV];//32 	🐓 📍 🗼
	uint16_t	port;

	/*
	if(!(PortConvert(argv[1], port)))
		return (1);
	*/

	/*IP  mainUtils.cpp */	
	sockaddr_in		hint;

	/*memset(&hint, 0, sizeof(hint));*/
	if (!IpConvert(argv[2], hint.sin_addr))
	    return (1);

	hint.sin_family = AF_INET; //ipv4
	hint.sin_port = htons(port);
















   /*Creat a socket*/
	/*listnig est le descripteur de socket retourné par socket()*/
	
	
	
	int			listening; // fd

	listening = socket(AF_INET, SOCK_STREAM, 0); 
	if (listening == -1)
	{
		std::cerr << "Can't create a socket!" << std::endl;
		return (-2);
	}

    int opt = 1;
	if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt failed" << std::endl;
		return (-1);
	}




	/*setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// Aqui você define como não-bloqueante corretamente: apenas quando tiver varios clientes
	int flags = fcntl(listening, F_GETFL, 0);
	if (flags == -1 || fcntl(listening, F_SETFL, flags | O_NONBLOCK) == -1)
	{
	    std::cerr << "Erro ao configurar O_NONBLOCK\n";
    	return (-1);
	}*/














	/*int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);*/
	/*bind*/
	if (bind (listening, (struct sockaddr*)&hint, sizeof(hint)) == -1)
	{
		std::cerr << "can't bind IP/port" << std::endl;
		return (-2);
	}

	/*Mark the socket for listening in*/
	if ( listen( listening, SOMAXCONN) == -1)
	{
		std::cerr << "Can't Listen " << std::endl;
		return (-2);
	}
	
	 std::cout << "Server OK - listening on " << argv[2] << ":" << port << std::endl;
	 std::cout << "mensagem 2 Server OK - listening on Port:" << port << " Ip: " << inet_ntoa(hint.sin_addr) << std::endl;
//	std::cout << "mensagem 2 Server OK - listening on Port:" << hint.sin_port << " Ip: " << inet_ntoa(hint.sin_addr) << std::endl;
	 // foi para testar o livro hint.sin_port = 0 (da porta aleatoria) 

	
	/*Accept a call*/
	/*Accept a client connection*/
	int			clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize;
	clientSize = sizeof(client);

	clientSocket = accept(listening,  (sockaddr*)&client, &clientSize);
	if (clientSocket == -1)
	{
		std::cout << "Problem with client connecting" << std::endl;
		return (-2);
	}

	/*Close the listening socket	*/
	close (listening);
	memset (host, 0, NI_MAXHOST);
	memset (svc, 0, NI_MAXSERV);

	int result = getnameinfo((sockaddr*)&client,
										clientSize,
										host,
										NI_MAXHOST,
										svc,
										NI_MAXSERV,
										0);
	if (result == 0)
	{
		std::cout << host << " connected on " << svc << std::endl;
	}
	else
	{
		//inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		//std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
		std::cout << inet_ntoa(client.sin_addr)
          << " connected on port "
          << ntohs(client.sin_port) << std::endl;


	}


	// While receiving - display message, echo message
	char buf[BUF_SIZE];
	while (true)
	{
		//clean the buffer
		memset (buf, 0, BUF_SIZE);
		//wait for message
		int bytesRecv = recv(clientSocket, buf, BUF_SIZE, 0 );
		if (bytesRecv == 0)
		{
			std::cerr << "Client Disconected " << std::endl;
			break ;
		}
		if (bytesRecv < 0)
		{
			std::cout << "There was connection issue " << std::endl;
			break ;
		}
		std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl; 
		
		/*resend message*/
		/*send(clientSocket, buf, bytesRecv , 0);*/
		send(clientSocket, "Hello, world!\n", 14, 0) ;

		if (send(clientSocket, buf, bytesRecv , 0) == -1)
		{
			std::cerr << "Send failed" << std::endl;
			break;
		}
	
	}

	close(clientSocket);

    return (0);

}