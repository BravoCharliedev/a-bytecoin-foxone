//#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

#define PORT 9898


class sockectClient{
public:


	~sockectClient();
	bool conect();
	bool send_data(std::string data);
	bool set_port(int port);
	bool set_ip(std::string ip);
	sockectClient(std::string ip, int port);

private:

	int Sport;
	std::string Sip;
	int sock = 0;
};

