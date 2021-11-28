#ifndef __USER_H__
#define __USER_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

/* client opcode */
#define LOGIN_USER      1
#define LOGOUT_USER     2
#define CLIENT_ORDER	3
#define CANCEL_ORDER    4
#define FIX_ORDER       5
#define ADD_USER        6
#define GET_INFO        7

/* server opcode */
#define LOGIN_SUCCESS   1
#define CREATE_NEW      2
#define ERROR_MSG       3

#define SOCKET_IP "127.0.0.1"
#define SERVER_PORT 32033		// tcp
#define UDP_PORT 50550			// udp

typedef struct {
	int oid;
	int uid;
	int p_code;
	int price;
	int volume;
} order;

class user {
	private:
	std::string id;
	std::string pw;
	int connection;
	int balance;
	std::map <int, int> products;
	std::vector<order> orders;

	public:
	user() {connection = -1;}
	user(std::string ID, std::string PW) : id(ID), pw(PW), connection(-1), balance(1000000) {products.clear();} 
	~user() {products.clear(); orders.clear();}
	std::string getID() {return id;}
	std::string getPW() {return pw;}
	int getConnection() {return connection;}
	int getBalance() {return balance;}
	std::map<int,int> getProducts() {return products;}
	std::vector<order> getOrders() {return orders;}
	void setID(std::string ID) {id = ID;}
	void setPW(std::string PW) {pw = PW;}
	void setConnection(int sock) {connection = sock;}
	void setBalance(int bal) {balance = bal;}
	void addProduct(int p_code, int volume) {
		if (products.count(p_code))
			products[p_code] += volume;
		else 
			products.insert({p_code, volume});
	}
	void delProduct(int p_code, int volume) {
		products[p_code] -= volume;
		if (products[p_code] == 0) products.erase(p_code);
	}
	void modifyProduct(int p_code, int volume) {
		products[p_code] = volume;
	}
	void addOrder(int oid, int p_code, int price, int vol) {
		order o;
		o.oid = oid;
		o.uid = 0;
		o.p_code = p_code;
		o.price = price;
		o.volume = vol;
		orders.push_back(o);
	}
	void addOrder(order ord) {
		orders.push_back(ord);
	}
	void delOrder(int oid) {
		for (size_t i = 0; i < orders.size(); i++) {
			if (orders[i].oid = oid) {
				orders.erase(orders.begin() + i);
				break;
			}
		}
	}
	void refresh(user rhs) {
		id = rhs.getID();
		pw = rhs.getPW();
		balance = rhs.getBalance();
		products = rhs.getProducts();
		orders = rhs.getOrders();
	}
};


#endif
