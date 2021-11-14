/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : server.cc
 * - File Type : c++ source file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 14, 2021.
 * - Description : server source file
 * **************************************************************************************************/

#include "login.h"
#include "dept.h"
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cstring>

#define SERVER_PORT 32032
#define BUF_SIZE 50

void sendUDP();
void handle_client();
void error_handling(const char *message);

int main() {
	User_info users = User_info();		
	// get Users information & order batch init
/*
	thread thread1(sendUDP);
	thread1.detach();
*/	
	handle_client();	
			
	

	return 0;
}

void sendUDP() {

}

void handle_client() {			// port : 32032
        int serv_sock, clnt_sock;
        int endf, buf_len;
	char buf[50];
	string id, pw;
        struct sockaddr_in serv_addr;
        struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
        serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1) error_handling("socket error!\n");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(SERVER_PORT);
        clnt_addr_size = sizeof(clnt_addr); 

        if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) error_handling("bind error");
        if(listen(serv_sock, 5) < 0) error_handling("listen error");

        while(1) {
                clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
                if (clnt_sock < 0) {
                        continue;
                }
                break;
        }

	while(1) {
		endf = 0;
		memset(buf, 0, sizeof(buf));
		buf_len = recv(clnt_sock, buf, BUF_SIZE, 0);
		if (buf_len > 0) {
			id = buf;
			pw = buf + 20;
		} else continue;
		cout << id << endl << pw << endl;
		break;
	} 
	

	close(clnt_sock);
        close(serv_sock);
}

void error_handling(const char *message)
{
	cout << message;
        exit(1);
        return;
}

