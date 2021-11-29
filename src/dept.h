/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : dept.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.4
 * - Date : Nov. 29, 2021.
 * - Description : Dept header file
 * **************************************************************************************************/
#ifndef __DEPT_H_
#define __DEPT_H_

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

using namespace std;

typedef struct level{
	int price;
	int volume;
} LEVEL;

class Dept {
	private:
	int product_code;
	int initial_price;
	vector <level> ask;
	vector <level> bid;

	public:
	Dept();						// default constructor
	Dept(int initP, int p_code);			// constructor
	~Dept();					// destructor
	int getInitPrice();				// return initial price
	int getCode();					// return product code
	LEVEL getLevel(int lv, char AB);		// return ASK[lv] or BID[lv]
	vector <LEVEL> getLevelAll(char AB);		// return ASK or BID
	void setInitPrice(int initP) { initial_price = initP;}	// set initial price
	void setCode(int p_code) {product_code = p_code;} // set product code
	void UpdateDeptLevel(int level, int price, int volume, char AB); // update dept by level
	void UpdateDeptLevel(int level, LEVEL lv, char AB); // update dept by level
	void addNewLevel(int level, LEVEL lv, char AB); // insert ASK[lv] or BID[lv]
	void delLevel(int level, char AB);		// delete ASK[lv] or BID[lv]
	bool isPriceValid(int price);			// price limit 30% up & down
};

Dept copyDept5(Dept dept);				// copy dept 5 level
void print5level(Dept original);			// print 5 level dept
#endif
