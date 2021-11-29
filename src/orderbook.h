/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : orderbook.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : orderbook system header file
 * **************************************************************************************************/

#ifndef _ORDERBOOK_H_
#define _ORDERBOOK_H_

#include "user.h"
#include "dept.h"
#include <iostream>
#include <vector>
#include <string>
#include <queue>


class ORDERBOOK : public Dept{
	private:
	int last_oid;
	int last_price;
	map<int, order> ordbook;
	vector<vector<int>> Abook;
	vector<vector<int>> Bbook;

	public:
	ORDERBOOK();				// default constructor
	ORDERBOOK(int _initP, int _p_code);	// constructor
	~ORDERBOOK();				// destructor
	int getLastOID();			// return last_oid
	map<int, order> getOrdbook();		// return ordbook
	order getOrderbyID(int oid);		// return order by orderID
	int getLastPrice();			// return Last price (close price)
	void addLastOID();			// add 1 last_oid
	int calcOrder(order raw_data);		// calculate client's order
	int delOrder(int oid);			// delete order
	void setLastPrice(int price);		// set last price
//	void insertBook(int lv, LEVEL new_lv, char AB);	// insert Book
};

/*
int ORDERBOOK::insertBook(int lv, LEVEL new_lv, char AB) {
	addLastOID();
	addNewLevel(lv, new_lv, AB);
	if (AB == 'A') {
		Abook.insert(Abook.begin() + lv, vector<int> (1, getLastOID()));
	} else {
		Bbook.insert(Bbook.begin() + lv, vector<int> (1, getLastOID()));
	}
}
*/
#endif
