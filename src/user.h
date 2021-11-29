/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : user.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : user system header file
 * **************************************************************************************************/

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

//#define SOCKET_IP "230.10.10.105"
#define SERVER_PORT 32033		// tcp
#define UDP_PORT 50550			// udp
#define UDP_TTL 64
#define ASK	'A'
#define BID	'B'


typedef struct {
	int oid;
	int uid;
	int p_code;
	int price;
	int volume;
	char ab;
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
	user() {connection = -1;}		// default constructor
	user(std::string ID, std::string PW) : id(ID), pw(PW), connection(-1), balance(1000000) {products.clear();} // constructor
	~user() {products.clear(); orders.clear();}		// destructor
	std::string getID() {return id;}			// return ID
	std::string getPW() {return pw;}			// return PW
	int getConnection() {return connection;}		// return user's socket
	int getBalance() {return balance;}			// return balance
	std::map<int,int> getProducts() {return products;}	// return products
	std::vector<order> getOrders() {return orders;}		// return orders
	void setID(std::string ID) {id = ID;}			// set ID
	void setPW(std::string PW) {pw = PW;}			// set PW
	void setConnection(int sock) {connection = sock;}	// set socket
	void setBalance(int bal) {balance = bal;}		// set balance
	void addProduct(int p_code, int volume) {		// add product
		if (products.count(p_code))
			products[p_code] += volume;
		else 
			products.insert({p_code, volume});
	}
	void delProduct(int p_code, int volume) {		// delete product
		products[p_code] -= volume;
		if (products[p_code] == 0) products.erase(p_code);
	}
	void modifyProduct(int p_code, int volume) {		// modify produdct volume
		if (products.count(p_code)) products[p_code] = volume;
		else products.insert({p_code, volume});
	}
	void addOrder(int oid, int p_code, int price, int vol) { // add order
		order o;
		o.oid = oid;
		o.uid = 0;
		o.p_code = p_code;
		o.price = price;
		o.volume = vol;
		orders.push_back(o);
	}
	void addOrder(order ord) {				// add order
		orders.push_back(ord);
	}
	void delOrder(int oid) {				// delete order (with oid)
		for (size_t i = 0; i < orders.size(); i++) {
			if (orders[i].oid = oid) {
				orders.erase(orders.begin() + i);
				break;
			}
		}
	}
	void orderClear() {					// clear order
		orders.clear();
	}
	void refresh(user rhs) {				// refresh user information
		id = rhs.getID();
		pw = rhs.getPW();
		balance = rhs.getBalance();
		products = rhs.getProducts();
		orders = rhs.getOrders();
	}
	int getAvail() {					// get Available balance to order
		int bal = getBalance();
		for (size_t i = 0; i < orders.size(); i++) {
			if (orders[i].ab == ASK) continue;
			bal -= orders[i].price * orders[i].volume;
		}
		return bal;
	}
	bool canOrder(order ord) {				// check order possible
		if (ord.ab == ASK) {
			if (products[ord.p_code] < ord.volume) return false;
		} else if (ord.ab == BID) {
			if (getAvail() < ord.volume * ord.price) return false;
		} else return false;
		return true;
	}
	void printUserInfo() {					// print user information
		std::cout << "-Balance: " << getBalance() << "\t" << "-Availabe balance: " << getAvail() << std::endl;
		std::cout << "-Products\n" << " product\tvolume\n";
		for (auto it : getProducts()) {
			std::cout << " " << it.first << "\t" << it.second << std::endl;
		}
		std::cout << "-Current orders\n" << "Ask/Bid\toid\tproduct\tprice\tvolume" << std::endl;
		for (size_t ii = 0; ii < orders.size(); ii++) {
			if (orders.at(ii).ab == ASK) std::cout << "Ask\t";
			else std::cout << "Bid\t";
			std::cout << orders.at(ii).oid << "\t";
			std::cout << orders.at(ii).p_code << "\t";
			std::cout << orders.at(ii).price << "\t";
			std::cout << orders.at(ii).volume << std::endl;	
		}
	}
};


#endif
