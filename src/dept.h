/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : dept.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 14, 2021.
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
	void setInitPrice(int initP) { initial_price = initP;}
	void setCode(int p_code) {product_code = p_code;}
	int getInitPrice();				// return initial price
	int getCode();					// return product code
	void UpdateDeptLevel(int level, int price, int volume, char AB); // update dept by level
	void UpdateDeptLevel(int level, LEVEL lv, char AB); // update dept by level
	void addNewLevel(int level, LEVEL lv, char AB);
	void delLevel(int level, char AB);
	bool isPriceValid(int price);			// price limit 30% up & down
	LEVEL getLevel(int lv, char AB);
	vector <LEVEL> getLevelAll(char AB);
};

Dept copyDept5(Dept dept);		// copy dept 5 level
#endif
