/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : dept.cc
 * - File Type : c++ source file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 20, 2021.
 * - Description : Dept source file
 * **************************************************************************************************/

#include "dept.h"

Dept::Dept() {
	product_code = 0;
	initial_price = 1;
	ask.clear();
	bid.clear();
	ask.resize(5);
	bid.resize(5);
}

Dept::Dept(int initP, int p_code) {
	initial_price = initP;
	product_code = p_code;
	ask.clear();
	bid.clear();
	ask.resize(10);
	bid.resize(10);
}

Dept::~Dept() {
	ask.clear();
	bid.clear();	
}

void Dept::UpdateDeptLevel(int level, int price, int volume, char AB) {
	LEVEL tmp;
	tmp.price = level;
	tmp.volume = volume;
	UpdateDeptLevel(level, tmp, AB);
}
void Dept::UpdateDeptLevel(int level, LEVEL lv, char AB) {
	if (AB == 'A') {			// ask
		if (ask.size() <= level) {
			ask.push_back(lv);
		} else {
			ask.at(level) = lv;
		} 
	} else if (AB == 'B') {			// bid
		if (bid.size() <= level) {
			bid.push_back(lv);
		} else {
			bid.at(level) = lv;
		}
	}
}

void Dept::addNewLevel(int level, LEVEL lv, char AB) {
	if (AB == 'A') 
		ask.insert(ask.begin() + level, lv);	
	else if (AB == 'B') 
		bid.insert(bid.begin() + level, lv);	
}

void Dept::delLevel(int level, char AB) {
	if (AB == 'A') 
		ask.erase(ask.begin() + level);	
	else if (AB == 'B')
		bid.erase(bid.begin() + level); 
}

bool Dept::isPriceValid(int price) {
	if (price < getInitPrice() * 0.7 || price > getInitPrice() * 1.3) return false;
	return true;
}

int Dept::getInitPrice() {
	return initial_price;
}

int Dept::getCode() {
	return product_code;
}

LEVEL Dept::getLevel(int lv, char AB) {
	LEVEL tmp = {};
	if (AB == 'A' && ask.size() >= lv) return ask.at(lv);
	else if(AB == 'B' && bid.size() >= lv) return bid.at(lv);
	else return tmp;
}

vector<LEVEL> Dept::getLevelAll(char AB) {
	if (AB == 'A') return ask;
	else return bid;
}

Dept copyDept5(Dept dept) {
	size_t lv;
	Dept temp = Dept();
	temp.setCode(dept.getCode());
	temp.setInitPrice(dept.getInitPrice());
	for (size_t i = 0; i < 5; i++) {
		temp.UpdateDeptLevel(i, dept.getLevel(i, 'A'), 'A');
		temp.UpdateDeptLevel(i, dept.getLevel(i, 'B'), 'B');
	}
	return temp;
}

void print5level(Dept original) {
	Dept dept = copyDept5(original);
	printf("avol\tprice\n");
	for (int i = 4; i >= 0; i--) {
		printf("%d\t%d\n", dept.getLevel(i, 'A').volume, dept.getLevel(i, 'A').price);
	}
	for (int i = 0; i < 5; i++) {
		printf("\t\t%d\t%d\n", dept.getLevel(i, 'B').price, dept.getLevel(i, 'B').volume);
	}
	printf("\t\tprice\tbvol\n");
}
