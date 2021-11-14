#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>

#define SERVER_PORT 32032
#define MAX_LEN 50

void error_handling(const char *message);

using namespace std;

int main()
{
        int sock, ret;
        struct sockaddr_in serv_addr;
	char buf[MAX_LEN + 1];
	char id[MAX_LEN + 1], pw[MAX_LEN + 1];

	sock=socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        serv_addr.sin_port=htons(SERVER_PORT);

	if(sock < 0) error_handling("socket error!\n");	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) error_handling("connect error!\n");

	memset(id, 0, sizeof(id));
	while(1) {
		cout << "enter ID: ";
		cin >> id;
		if (strlen(id) > 20) {
			memset(id, 0, sizeof(id));
			cout << "ID length is lower than 20" << endl;
			continue;
		}
		break;
	}
	memset(pw, 0, sizeof(pw));
	while(1) {
		cout << "enter Password: ";
		cin >> pw;
		if (strlen(pw) > 20) {
			memset(pw, 0, sizeof(pw));
			cout << "Password length is lower than 20" << endl;
			continue;
		}
		break;
	}
	memcpy(buf, id, 20);
	memcpy(buf + 20, pw, 20);
	buf[40] = '\0';
	ret = send(sock, buf, 40, 0);
	cout << "Send: " << ret << endl;
	close(sock);
	return 0; 	
}

void error_handling(const char *message)
{
	cout << message;
        exit(1);
}
