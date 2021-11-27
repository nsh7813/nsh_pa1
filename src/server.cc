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
#include "orderbook.h"
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <cstring>

using std::thread;

#define SERVER_PORT 32032		// tcp
#define UDP_PORT 50550	
#define BUF_SIZE 100

typedef struct {
	int sock;
	int usr_code;
} session_table;

void sendUDP(Dept dpt);
void handle_client();
int handle_oper(char *buf, session_table table);
void batch();
void error_handling(const char *message);

bool endf = false;
vector<user> users;
extern queue<int> ordQue;
extern order autofixed;
extern int sum_balance;
ORDER ords;


int main() {
	// get Users information & order batch init	
	batch();

	thread thread1(sendUDP, copyDept5(ords));
	thread1.detach();
	
	handle_client();
	

	return 0;
}

void sendUDP(Dept dpt) {
	int sock, snd_len;
	struct sockaddr_in udp_addr;
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < -1) {
		cout << "Socket create error\n";
		exit(1);
	}

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	udp_addr.sin_port = htons(UDP_PORT);
	
	if (bind(sock, (struct sockaddr*) &udp_addr, sizeof(udp_addr)) < 0) {
		cout << "Bind error\n";
		exit(1);
	}	

	for (int i = 0; i < 100; i++) {
		sleep(5);
		//cout << dept.getInitPrice() << endl;
		snd_len = sendto(sock, (void*) &dpt, sizeof(dpt), 0, (struct sockaddr*) &udp_addr, sizeof(udp_addr));
		cout << "send: " << snd_len << endl;
	}
	close(sock);
}

void handle_client() {			// port : 32032
        int serv_sock, clnt_sock;
        int i, buf_len, fd_max, fd_num, op_ret, opcode;
	char buf[BUF_SIZE];
        struct sockaddr_in serv_addr;
        struct sockaddr_in clnt_addr;
	struct timeval timeout;
	socklen_t clnt_addr_size;
        serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	fd_set readset, copyset;
	session_table new_table;
	vector <session_table> s_table;

        memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	if(serv_sock == -1) error_handling("socket error!\n");
        if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) error_handling("bind error\n");
        if(listen(serv_sock, 5) < 0) error_handling("listen error\n");

	FD_ZERO(&readset);
	FD_SET(serv_sock, &readset);
	FD_SET(STDIN_FILENO, &readset);
	fd_max = serv_sock;
	new_table.sock = -1;
	new_table.usr_code = -1;
	s_table.resize(fd_max + 1);
	for (size_t ii = 0; ii < fd_max + 1; ii++) s_table.push_back(new_table);

        while(!endf) {
		copyset = readset;
		if ((fd_num = select(fd_max + 1, &copyset, NULL, NULL, NULL)) < 0) {
			error_handling("select error\n");
		}
		
		if (fd_num == 0) continue;
		for (i = 0; i < fd_max + 1; i++) {
			if (FD_ISSET(i, &copyset)) {
				if (i == serv_sock) {				// accept
					clnt_addr_size = sizeof(clnt_addr);
					if ((clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size)) < 0) {
						cout << "accept fail!" << endl;
						continue;
					}
					FD_SET(clnt_sock, &readset);
					new_table.sock = clnt_sock;
					new_table.usr_code = -1;
					if (fd_max < clnt_sock) {
						s_table.resize(fd_max + 1);
						s_table.at(clnt_sock) = new_table;
						fd_max = clnt_sock;
					}
					s_table.at(i) = new_table;
				} else if (i == STDIN_FILENO) { 		// close server;
					memset(buf, 0, sizeof(buf));
					if ((buf_len = read(i, buf, sizeof(buf))) > 0) {
						if (strncmp(buf, "exit", 4) == 0) {
							endf = true;
							break;
						}
					}
				} else {
					memset(buf, 0, sizeof(buf));
					if ((buf_len = read(i, buf, sizeof(buf))) > 0) {
						op_ret = handle_oper(buf, s_table.at(i));
						opcode = atoi(buf);
						if (op_ret < 0) {	// send error message
							send(i, "-1", 2, 0);
						}
						else {
							memset(buf, 0, sizeof(buf));
							switch(opcode) {
							case LOGIN_USER:
								s_table.at(i).usr_code = op_ret;
								sprintf(buf, "%d %d", opcode, op_ret);
								break;
							case ADD_USER:
							case CLIENT_ORDER:
								sprintf(buf, "%d", opcode);
								break;
							default:
								break;
							}
							write(i, buf, strlen(buf)); 
						}
					}
					if (buf_len == 0) {
						// close connection 
						close(i);
						FD_CLR(i, &readset);
						users.at(s_table.at(i).usr_code).setConnection(-1);
						s_table.erase(s_table.begin() + i);
					}
				}
			}
		}
        }

	for (i = 0; i < fd_max + 1; i++) {
		if (FD_ISSET(i, &readset)) {
			close(i);
		}
	}
	return;
}

int handle_oper(char *buf, session_table table) {
	int recv_opcode, send_opcode, ret = 0;
	int uid, p_code, price, vol, bal;
	char id[20], pw[20], AB;
	order ord, tmp;
	recv_opcode = atoi(buf);
	if (table.usr_code == -1 && recv_opcode != LOGIN_USER && recv_opcode != ADD_USER) {
		return -1;
	}
	//sscanf(buf, "%d %s %s", &recv_opcode, &
	switch(recv_opcode) {
	case LOGIN_USER:		// id/pw , return success or not
		memset(id, 0, sizeof(id));	
		memset(pw, 0, sizeof(pw));	
		sscanf(buf, "%d %s %s", &recv_opcode, id, pw);
		uid = isUser(users, string(id), string(pw));
		if (uid > 0) {
			users.at(uid).setConnection(table.sock);
			ret = uid;
		} else { 
			ret = -1; 
		}
		break;
	case LOGOUT_USER:		// no option
		break;
	case CLIENT_ORDER:		// product code / price / volume , return success or not	
		sscanf(buf, "%d %d %d %d %c", &recv_opcode, &p_code, &price, &vol, &AB);
		if (AB == 'B' && users[table.usr_code].getBalance() < price * vol) return (-1);
		ord.oid = 0;
		ord.uid = table.usr_code;
		ord.p_code = p_code;
		ord.price = price;
		ord.volume = vol;
		ret = ords.calcOrder(ord, AB);
		if (ret < 0) break;
		else if (ret > 0) {			// have traded data
			while(!ordQue.empty()) {
				tmp = ords.getOrderbyID(ordQue.front());
				uid = tmp.uid;
				bal = users[uid].getBalance();
				if (AB == 'A') {
					users[uid].delProduct(p_code, tmp.volume);
					users[uid].setBalance(bal + tmp.volume * tmp.price);
				} else if (AB == 'B') {
					users[uid].addProduct(p_code, tmp.volume);
					users[uid].setBalance(bal - tmp.volume * tmp.price);
				}
				ords.delOrder(ordQue.front());
				ordQue.pop();	
			}
			uid = table.usr_code;
			bal = users[uid].getBalance();
			users[uid].setBalance(bal + sum_balance);	
			if (autofixed.uid != table.usr_code) {	
				if (AB == 'A') users[uid].delProduct(p_code, vol);
				else if (AB == 'B') users[uid].addProduct(p_code, vol);

				uid = autofixed.uid;
				bal = users[uid].getBalance();
				ord = ords.getOrderbyID(autofixed.oid);
				vol = ord.volume - autofixed.volume; 
				if (AB == 'B') {
					users[uid].delProduct(p_code, vol);
					users[uid].setBalance(bal + vol * autofixed.price);
				} else if (AB == 'A') {
					users[uid].addProduct(p_code, vol);
					users[uid].setBalance(bal - vol * autofixed.price);
				}
				users[uid].delOrder(autofixed.oid);
				users[uid].addOrder(autofixed);
			} else {
				users[uid].addOrder(autofixed);
				if (AB == 'A') users[uid].delProduct(p_code, vol - autofixed.volume);
				else if (AB == 'B') users[uid].addProduct(p_code, vol - autofixed.volume);
			}
		} else {
			uid = table.usr_code;
			ord.oid = ords.getLastOID();
			users[uid].addOrder(ord);
		} 
		// send dept
		break;
	case CANCEL_ORDER:		// oid , return success or not 
		break;
	case FIX_ORDER:			// oid / product code / price / volume , return success or not
		break;
	case ADD_USER:			// id/pw , return success or not
		memset(id, 0, sizeof(id));	
		memset(pw, 0, sizeof(pw));	
		sscanf(buf, "%d %s %s", &recv_opcode, id, pw);
		for (size_t i = 0; i < users.size(); i++) {
			if (idCheck(users.at(i), string(id)) || pwCheck(users.at(i), string(pw))) {
				return -1;
			}
		}
		if (uid == 0) {
			users.push_back(user(string(id), string(pw)));
		} else { 
			ret = -1; 
		}
		break;
	default:
		break;
	}

	return ret;
}

void batch() {
	ords = ORDER(100, 2222);
	users.clear();
	string test = "admin";
	user new_user = user(test, test);
	users.push_back(new_user);
	users[0].addProduct(2222, 10000);

/*
	for (int i = 0; i < 20; i++) {
		ords.UpdateDeptLevel(i, 100 + i, 1000, 'A');
		ords.UpdateDeptLevel(i, 99 - i, 1000, 'B');
	}
*/
	return;
}

void error_handling(const char *message)
{
	std::cout << message;
	exit(1);
}

