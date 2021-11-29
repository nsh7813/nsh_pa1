/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : orderbook.cc
 * - File Type : c++ source file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : orderbook system source file
 * **************************************************************************************************/

#include "orderbook.h"

queue<int> ordQue;
order autofixed;
int sum_balance;

ORDERBOOK::ORDERBOOK() : Dept() {
	last_oid = 0;
	last_price = 0;
	ordbook.clear();
	Abook.clear();
	Bbook.clear();
}

ORDERBOOK::~ORDERBOOK() {
	ordbook.clear();
	Abook.clear();
	Bbook.clear();
}

ORDERBOOK::ORDERBOOK(int _initP, int _p_code) : Dept(_initP, _p_code) {
	last_oid = 0;
	last_price = _initP;
	ordbook.clear();
	Abook.resize(10);
	Bbook.resize(10);
}

map<int, order> ORDERBOOK::getOrdbook() {
	return ordbook;
}

order ORDERBOOK::getOrderbyID(int oid) {
	order tmp = {};
	if (ordbook.count(oid)) {
		return ordbook[oid];
	}
	return tmp;
}

int ORDERBOOK::getLastOID() {
	return last_oid;
}

void ORDERBOOK::addLastOID() {
	last_oid++;
	while(ordbook.count(last_oid)) {
		last_oid++;
		if(last_oid == 10000000) last_oid = 1;
	}
}

int ORDERBOOK::calcOrder(order raw_data) {
	size_t lv;
	int traded = 0;
	LEVEL top, tlv;
	order tmp;
	autofixed = {};
	sum_balance = 0;
	char AB = raw_data.ab;

	if (!isPriceValid(raw_data.price)) return (-1);
	if (AB == 'A') {		// Sell
		top = getLevel(0, 'B');
		if (raw_data.price > top.price) {	// to orderbook
			for (lv = 0; lv < getLevelAll(AB).size(); lv++) {
				if (getLevelAll(AB).at(lv).price == 0) {
					tlv.volume = raw_data.volume;
					tlv.price = raw_data.price;
					updateBook(lv, tlv, AB);
				}
				else if (raw_data.price == getLevelAll(AB).at(lv).price) {
					tlv = getLevel(lv, AB);
					tlv.volume += raw_data.volume;
					updateBook(lv, tlv, AB);
				} else if(raw_data.price < getLevelAll(AB).at(lv).price) {
					tlv.price = raw_data.price;
					tlv.volume = raw_data.volume;
					insertBook(lv, tlv, AB);
				} else continue;
				raw_data.oid = getLastOID();
				ordbook.insert({getLastOID(), raw_data});
				break;
			}
			if (lv == getLevelAll(AB).size()) {
				tlv.price = raw_data.price;
				tlv.volume = raw_data.volume;
				addLastOID();
				Abook.push_back(vector<int> (1, getLastOID()));
				raw_data.oid = getLastOID();
				ordbook.insert({getLastOID(), raw_data});
				UpdateDeptLevel(lv, tlv, AB);	
			}
		} else {				// trade
			while(raw_data.volume > 0) {
				top = getLevel(0, 'B');
				if (raw_data.price > top.price) {
					tlv.price = raw_data.price;
					tlv.volume = raw_data.volume;
					insertBook(0, tlv, AB);
					raw_data.oid = getLastOID();
					ordbook.insert({getLastOID(), raw_data});
					autofixed = raw_data;
					break;
				}
				if (raw_data.volume >= top.volume) {
					raw_data.volume -= top.volume;
					traded += top.volume;
					delLevel(0, 'B');
					for (size_t i = 0; i < Bbook[0].size(); i++) {
						// trade success
						ordQue.push(Bbook[0][i]);
					}
					sum_balance += top.volume * top.price;
					Bbook.erase(Bbook.begin());
				} else {
					tmp = getOrderbyID(Bbook[0][0]);
					if (raw_data.volume >= tmp.volume) {
						raw_data.volume -= tmp.volume;
						traded += tmp.volume;
						top.volume -= tmp.volume;
						ordQue.push(Bbook[0][0]);
						sum_balance += tmp.price * tmp.volume;
						Bbook[0].erase(Bbook[0].begin());
					} else {
						autofixed = tmp;	
						autofixed.volume -= raw_data.volume; 
						top.volume -= raw_data.volume;
						sum_balance += tmp.price * raw_data.volume;
						traded += raw_data.volume;
						raw_data.volume = 0;
					}
					UpdateDeptLevel(0, top, 'B');
				}
			}
		}	
	} else if (AB == 'B'){		// Buy
		top = getLevel(0, 'A');
		if (top.price == 0) top.price = 0x7FFFFFF;
		if (raw_data.price < top.price) {	// to orderbook
			for (lv = 0; lv < getLevelAll(AB).size(); lv++) {
				if (getLevelAll(AB).at(lv).price == 0) {
					tlv.volume = raw_data.volume;
					tlv.price = raw_data.price;
					updateBook(lv, tlv, AB);
				}
				else if (raw_data.price == getLevelAll(AB).at(lv).price) {
					tlv = getLevel(lv, AB);
					tlv.volume += raw_data.volume;
					updateBook(lv, tlv, AB);
				} else if(raw_data.price > getLevelAll(AB).at(lv).price) {
					tlv.price = raw_data.price;
					tlv.volume = raw_data.volume;
					insertBook(lv, tlv, AB);
				} else continue;
				raw_data.oid = getLastOID();
				ordbook.insert({getLastOID(), raw_data});
				break;
			}
			if (lv == getLevelAll(AB).size()) {
				tlv.price = raw_data.price;
				tlv.volume = raw_data.volume;
				addLastOID();
				Bbook.push_back(vector<int> (1, getLastOID()));
				raw_data.oid = getLastOID();
				ordbook.insert({getLastOID(), raw_data});
				UpdateDeptLevel(lv, tlv, AB);	
			}
		} else {				// trade
			while(raw_data.volume > 0) {
				top = getLevel(0, 'A');
				if (raw_data.price < top.price) {
					tlv.price = raw_data.price;
					tlv.volume = raw_data.volume;
					insertBook(0, tlv, AB);
					raw_data.oid = getLastOID();
					ordbook.insert({getLastOID(), raw_data});
					autofixed = raw_data;
					break;
				}
				if (raw_data.volume >= top.volume) {
					raw_data.volume -= top.volume;
					traded += top.volume;
					delLevel(0, 'A');
					for (size_t i = 0; i < Abook[0].size(); i++) {
						// trade success
						ordQue.push(Abook[0][i]);
					}
					sum_balance -= top.volume * top.price;
					Abook.erase(Abook.begin());
				} else {
					tmp = getOrderbyID(Abook[0][0]);
					if (raw_data.volume >= tmp.volume) {
						raw_data.volume -= tmp.volume;
						traded += tmp.volume;
						top.volume -= tmp.volume;
						ordQue.push(Abook[0][0]);
						Abook[0].erase(Abook[0].begin());
						sum_balance -= tmp.price * tmp.volume;
					} else {
						autofixed = tmp;	
						autofixed.volume -= raw_data.volume;
						top.volume -= raw_data.volume;
						sum_balance -= tmp.price * raw_data.volume;
						traded += raw_data.volume;
						raw_data.volume = 0;
					}
					UpdateDeptLevel(0, top, 'A');	
				}
			}
		}	

	} else return (-1);
	return traded;
}

int ORDERBOOK::delOrder(int oid) {
	ordbook.erase(oid);
	return 0;	
}

int ORDERBOOK::getLastPrice() {
	return last_price;
}

void ORDERBOOK::setLastPrice(int price) {
	last_price = price;
}


void ORDERBOOK::insertBook(int lv, LEVEL new_lv, char AB) {
	addLastOID();
	addNewLevel(lv, new_lv, AB);
	if (AB == ASK) {
		Abook.insert(Abook.begin() + lv, vector<int> (1, getLastOID()));
	} else {
		Bbook.insert(Bbook.begin() + lv, vector<int> (1, getLastOID()));
	}
}

void ORDERBOOK::updateBook(int lv, LEVEL new_lv, char AB) {
	addLastOID();
	if (AB == ASK) {
		Abook[lv].push_back(getLastOID());
	} else {
		Bbook[lv].push_back(getLastOID());
	}
	UpdateDeptLevel(lv, new_lv, AB);	
}
