/*
 * socketsClient.cpp
 *
 *  Created on: 08-05-2018
 *      Author: devcore-01
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include "socketsClient.h"
#include <string.h>
#include <vector>
#include <unistd.h>

sockectClient::sockectClient(std::string ip, int port): Sip(ip), Sport(port) {}

bool sockectClient::set_ip(std::string ip){
	Sip= ip;
	return true;
}
bool sockectClient::set_port(int port ){
	Sport= 12998;
	return true;
}
bool sockectClient::conect(){
    struct sockaddr_in address;
    int valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        //printf("\n Socket creation error \n");
        return false;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12998);


    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "209.50.50.41" , &serv_addr.sin_addr)<=0)
    {
        //printf("\nInvalid address/ Address not supported \n");
        return false;
    }
    //printf("\n try to conect \n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return false;
    }
    //printf("\n conect sucessfull  \n");
	return true;
}
bool sockectClient::send_data(std::string data){
	//int on = 1;
	//setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, 2);
	std::vector<char> Buffer(data.c_str(), data.c_str() + data.size() + 1);
    char *hello = &Buffer[0];
    if (send(sock , hello , Buffer.size() , 0 )<0){
    	return false;
    }
	return true;

}

sockectClient::~sockectClient(){}
