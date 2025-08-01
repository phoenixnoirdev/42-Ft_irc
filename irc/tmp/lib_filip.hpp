
/*  🐓  */

#ifndef LIB_FILIP_HPP
# define LIB_FILIP_HPP

# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
//# include <string.h>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <iostream>
# include <string>
# include <sstream>
# include <limits>
# include <fcntl.h>

# define BUF_SIZE 4096
# define BACKLOG 10 

bool PortConvert(const std::string& arg, uint16_t& outPort);
bool IpConvert(const std::string& ipStr, struct in_addr& outAddr);




#endif