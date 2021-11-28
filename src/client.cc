#include "client.h"

using namespace std;

int order_balance;
int phase = 0;
user self;

void getInfo();
void error_handling(const char *message);

int main()
{
        int sock, ret, recv_len;
	int fd_max, fd_num, opcode, usr_code = -1;
	int p_code, price, vol;
        struct sockaddr_in serv_addr;
	char buf[MAX_LEN + 1];
	char id[20], pw[20];
	char ASK_OR_BID;
	bool endf = false;
	fd_set readset, copyset;

	sock=socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        serv_addr.sin_port=htons(SERVER_PORT);

	if(sock < 0) error_handling("socket error!\n");	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) error_handling("connect error!\n");
	
	FD_ZERO(&readset);
	FD_SET(STDIN_FILENO, &readset);
	FD_SET(sock, &readset);
	fd_max = sock;
	usr_code = -1;
	memset(id, 0, sizeof(id));
	memset(pw, 0, sizeof(pw));

	while(!endf) {
		copyset = readset;
		if ((fd_num = select(fd_max + 1, &copyset, NULL, NULL, NULL)) < 0 ) {
			error_handling("select failed.\n");
		}
		if (fd_num == 0) continue;
		if (FD_ISSET(STDIN_FILENO, &copyset)) {
			memset(buf, 0, sizeof(buf));
			cin >> opcode;
			switch (opcode) {
			case LOGIN_USER:
			case ADD_USER:
				if (phase != 0) {
					cout << "Error: Already logged in!" << endl;
					break;
				}
				cin >> id >> pw;
				sprintf(buf, "%d %s %s", opcode, id, pw);
				send(sock, buf, strlen(buf), 0);
				recv_len = recv(sock, buf, sizeof(buf), 0);
				ret = atoi(buf);
				if (recv_len == 0) {
					cout << "Error: Server disconnected." << endl;
					exit(1);
				}
				if (ret < 0) {
					cout << "Error: invalid command!" << endl;	
				} else if (ret == LOGIN_USER) {
					cout << "Client login success." << endl;
					sscanf(buf, "%d %d", &ret ,&usr_code);
					self = user(string(id), string(pw));
					self.setConnection(sock);
					phase = 1;
				}
				break;
			case LOGOUT_USER:
				endf = true;	
				break;
			case CLIENT_ORDER:
				if (phase == 0) {
					cout << "Error: invalid command!" << endl;
					break;
				}
				getInfo();
				send(sock, "7", 1, 0);
				recv_len = recv(sock, buf, sizeof(buf), 0);
				cin >> p_code >> price >> vol >> ASK_OR_BID;
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "%d %d %d %d %c", opcode, p_code, price, vol, ASK_OR_BID);
				send(sock, buf, strlen(buf), 0);
				recv_len = recv(sock, buf, sizeof(buf), 0);
				ret = atoi(buf);
				if (recv_len == 0) {
					cout << "Error: Server disconnected." << endl;
					exit(1);
				}
				if (ret < 0) {
					cout << "Error: invalid command!" << endl;	
				} else {
					cout << "order success" << endl;
				}			
				break;
			case CANCEL_ORDER:
			case FIX_ORDER:
				break;
			default:
				endf = true;
				break;
			}
		} else if (FD_ISSET(sock, &copyset)){
			memset(buf, 0, sizeof(buf));
			ret = read(sock, buf, sizeof(buf));
			if (ret > 0) {
				opcode = atoi(buf);
				switch (opcode) {
				case LOGIN_SUCCESS:
					break;
				case CREATE_NEW:
					break;
				case ERROR_MSG:
					cout << "Error: " << buf << endl;
					break;
				case GET_INFO:
				default:
					break;
				}
			} 
		}
	}
	close(sock);
	return 0; 	
}


void error_handling(const char *message)
{
	cout << message;
	exit(1);
}

void getInfo() {

	return;
}
