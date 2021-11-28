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
#include <mutex>
#include <condition_variable>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <cstring>

using std::thread;

#define BUF_SIZE 100

typedef struct {
	int sock;
	int usr_code;
} session_table;

mutex udp_mutex;
condition_variable udp_cond;

void sendUDP(int product_code);
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

	thread thread1(sendUDP, ords.getCode());
	thread1.detach();
	
	handle_client();
	

	return 0;
}

void sendUDP(int product_code) {
	int sock, snd_len;
	struct sockaddr_in udp_addr;
	socklen_t udp_addr_size;
	char buf[300];
	string data;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	Dept dept;
	if (sock < -1) {
		cout << "Socket create error\n";
		exit(1);
	}

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	//udp_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	udp_addr.sin_port = htons(UDP_PORT);
	if (bind(sock, (struct sockaddr*) &udp_addr, sizeof(udp_addr)) < 0) {
		cout << "Bind error\n";
		exit(1);
	}	

	udp_addr_size = sizeof(udp_addr);	
	while(!endf) {
		unique_lock<mutex> lock(udp_mutex);
		udp_cond.wait(lock);
		dept = copyDept5((Dept) ords);
		memset(buf, 0, sizeof(buf));
		data.clear();
		sprintf(buf, "%12d", product_code);
		data += string(buf);
		sprintf(buf, "%12d", dept.getInitPrice());
		data += string(buf);
		for (int i = 0; i < 5; i++) {
			sprintf(buf, "%12d%12d", dept.getLevel(i, 'A').price, dept.getLevel(i, 'A').volume);
			data += string(buf);
			sprintf(buf, "%12d%12d", dept.getLevel(i, 'B').price, dept.getLevel(i, 'B').volume);
			data += string(buf);
		}
		strncpy(buf, data.c_str(), data.length());
		snd_len = sendto(sock, buf, strlen(buf), 0, (struct sockaddr*) &udp_addr, udp_addr_size);
		cout << "send udp: " << snd_len << endl;
		cout << buf << endl;
	}
/*
	for (int i = 0; i < 100; i++) {
		sleep(5);
		//cout << dept.getInitPrice() << endl;
		snd_len = sendto(sock, (void*) &dpt, sizeof(dpt), 0, (struct sockaddr*) &udp_addr, sizeof(udp_addr));
		cout << "send: " << snd_len << endl;
	}
*/
	cout << "close thread" << endl;
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
						fd_max = clnt_sock;
						s_table.resize(fd_max + 1);
						s_table.at(clnt_sock) = new_table;
					}
					s_table.at(clnt_sock) = new_table;
				} else if (i == STDIN_FILENO) { 		// close server;
					memset(buf, 0, sizeof(buf));
					if ((buf_len = read(i, buf, sizeof(buf))) > 0) {
						if (strncmp(buf, "exit", 4) == 0) {
							endf = true;
							udp_cond.notify_one();
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
						//users.at(s_table.at(i).usr_code).setConnection(-1);
						//s_table.erase(s_table.begin() + i);
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
	char sbuf[100];
	order ord, tmp;
	user info;
	map<int, int> cpyProducts;
	vector<order> cpyOrders;
	
	recv_opcode = atoi(buf);
	if (table.usr_code == -1 && recv_opcode != LOGIN_USER && recv_opcode != ADD_USER) {
		return -1;
	}

	switch(recv_opcode) {
	case LOGIN_USER:		// id/pw , return success or not
		memset(id, 0, sizeof(id));	
		memset(pw, 0, sizeof(pw));	
		sscanf(buf, "%d %s %s", &recv_opcode, id, pw);
		uid = isUser(users, string(id), string(pw));
		if (uid >= 0) {
			users.at(uid).setConnection(table.sock);
			ret = uid;
			udp_cond.notify_one();
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
			while(!ordQue.empty()) {	// full traded orders
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
			cout << "before: " << users[uid].getProducts()[p_code] << " -> after: ";
			if (AB == 'A') users[uid].delProduct(p_code, ret);
			else if (AB == 'B') users[uid].addProduct(p_code, ret);
			cout << users[uid].getProducts()[p_code] << endl;
			if (autofixed.volume != 0) {
				if (ret != vol) {
					users[uid].addOrder(autofixed);
				} else {
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
				}
			}
		} else {
			uid = table.usr_code;
			ord.oid = ords.getLastOID();
			users[uid].addOrder(ord);
		} 
		// send dept
		udp_cond.notify_one();
		cout << "traded: " << ret << endl;
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
		if (table.usr_code == -1) {
			users.push_back(user(string(id), string(pw)));
		} else { 
			ret = -1; 
		}
		break;
	case GET_INFO:
		if (table.usr_code == -1) {
			return (-1);
		}
		info = users[table.usr_code];
		cpyProducts = users[table.usr_code].getProducts();
		cpyOrders = info.getOrders();
		memset(sbuf, 0, sizeof(sbuf));
		sprintf(sbuf, "%12d%12d%12d", info.getBalance(), (int) cpyProducts.size(), (int) cpyOrders.size());

		send(table.sock, sbuf, strlen(sbuf), 0);
		for (auto it : cpyProducts) {
			memset(sbuf, 0, sizeof(sbuf));
			sprintf(sbuf, "%12d%12d", (int) it.first, (int) it.second);
			cout << it.first << " " << it.second << endl;
			send(table.sock, sbuf, strlen(sbuf), 0);
		}
		for (size_t i = 0; i < cpyOrders.size(); i++) {
			memset(sbuf, 0, sizeof(sbuf));
			send(table.sock, (char*) &cpyOrders.at(i), sizeof(order), 0);
		}
		//send(table.sock, (char*)&users[table.usr_code], atoi(sbuf), 0);
		break;
	default:
		break;
	}

	return ret;
}

void batch() {
	ords = ORDER(100, 2222);
	users.clear();
	string test = string("admin");
	user new_user = user(test, test);
	users.push_back(new_user);
	users[0].addProduct(2222, 10000);
	return;
}

void error_handling(const char *message)
{
	std::cout << message;
	exit(1);
}
/*
void printDept() {
	Dept dept = copyDept5((Dept) ords);
	printf("\tavol\tprice\n");
	for (int i = 4; i >= 0; i--) {
		printf("%d\t%d\t%d\n", i,  ords.getLevel(i, 'A').volume, ords.getLevel(i, 'A').price);
	}
	for (int i = 0; i < 5; i++) {
		printf("\t\t\t%d\t%d\t%d\n", ords.getLevel(i, 'B').price, ords.getLevel(i, 'B').volume, i);
	}
	printf("\t\t\tprice\tbvol\n");
}
*/
