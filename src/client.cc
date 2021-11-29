/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : client.cc
 * - File Type : c++ source file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : cleint source file
 * **************************************************************************************************/

#include "client.h"
#include <thread>

using namespace std;
using std::thread;

bool endf = false;
int phase = 0;
user self;
Dept dept;

void getInfo();
void error_handling(const char *message);
void recv_dept(int p_code);


int main()
{
        int sock, ret, recv_len;
	int fd_max, fd_num, opcode, usr_code = -1;
	int p_code, price, vol;
        struct sockaddr_in serv_addr;
	char buf[MAX_LEN + 1];
	char id[20], pw[20];
	char ASK_OR_BID;
	order ord;
	fd_set readset, copyset;
	string usage;

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

	dept = Dept(100, 2222);
	thread udp_thread(recv_dept, 2222);
	udp_thread.detach();

	self = user();
	usage = "Usage! (int = command)\n1: login, enter ID PW\n2: exit\n3: order, enter product_code price volume A(ask) or B(bid)\n";
	usage += "6: add new user, enter ID PW\n7: view user information\n0: print Usage\n";
	cout << usage;	
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
			case 0:
				cout << usage;
				break;
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
					self.setConnection(sock);
					getInfo();
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
				cin >> p_code >> price >> vol >> ASK_OR_BID;
				ord.p_code = p_code; ord.price = price; ord.volume = vol;
				ord.oid = 0; ord.uid = usr_code; ord.ab = ASK_OR_BID;
				if (!self.canOrder(ord)) {
					cout << "Error: invalid order!" << endl;
					break;
				}
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
			case GET_INFO:
				if (phase == 0) {
					cout << "Error: invalid command!" << endl;
					break;
				}
				getInfo();
				self.printUserInfo();
				break;
			default:
				endf = true;
				break;
			}
		} else if (FD_ISSET(sock, &copyset)){
			memset(buf, 0, sizeof(buf));
			ret = read(sock, buf, sizeof(buf));
		}
	}
	close(sock);
	return 0; 	
}

void recv_dept(int p_code) {
	int sock;
	int recv_len;
	socklen_t udp_addr_size;
	struct sockaddr_in udp_addr;
	struct ip_mreq join_udp;
	char buf[300];
	char *lptr;
	LEVEL lv;

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	udp_addr.sin_port = htons(UDP_PORT);
	udp_addr_size = sizeof(udp_addr);

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		error_handling("udp socket init failed!\n");
	}
	
	bind(sock, (struct sockaddr*)&udp_addr, sizeof(udp_addr));
	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_udp, sizeof(join_udp));
	while(!endf) {
		memset(buf, 0, sizeof(buf));
		recv_len = recvfrom(sock, buf, 12 * 22, 0, (struct sockaddr*)&udp_addr, &udp_addr_size);
		cout << "udp recv: " << recv_len << endl;
		if (recv_len > 0) {
			lptr = buf;
			//if more than one product
			//dept.setCode(atoi(lptr));	lptr += 12;
			//dept.setInitPrice(atoi(lptr));	lptr += 12;
			lptr += 24;
			for (int i = 0; i < 5; i++) {
				lv.price = atoi(lptr);
				lptr += 12;
				lv.volume = atoi(lptr);
				lptr += 12;
				dept.UpdateDeptLevel(i, lv, 'A');
				lv.price = atoi(lptr);
				lptr += 12;
				lv.volume = atoi(lptr);
				lptr += 12;
				dept.UpdateDeptLevel(i, lv, 'B');
			}
			print5level(dept);
		}
	}
	close(sock);
}

void error_handling(const char *message)
{
	cout << message;
	exit(1);
}

void getInfo() {
	char *info;
	char buf[100];
	int size, recv_len;
	int sock = self.getConnection();
	int bal, itemsize, ordsize, p_code, vol, oid, uid, price;
	order tmp;
	memset(buf, 0, sizeof(buf));
	send(sock, "7", 1, 0);
	if ((recv_len = recv(sock, buf, 12*3, 0)) > 0) {
		sscanf(buf, "%d%d%d", &bal, &itemsize, &ordsize);
	}
	self.setBalance(bal);
	for (int i = 0; i < itemsize; i++) {
		memset(buf, 0, sizeof(buf));
		recv(sock, buf, 12*2, 0);
		sscanf(buf, "%d%d", &p_code, &vol);
		self.addProduct(p_code, vol);
	}
	for (int i = 0; i < ordsize; i++) {
		memset(buf, 0, sizeof(buf));
		recv_len = recv(sock, buf, sizeof(order), 0);
		memcpy(&tmp, buf, recv_len);
		self.addOrder(tmp);
	}
}
