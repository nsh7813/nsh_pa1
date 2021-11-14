/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : dept.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 14, 2021.
 * - Description : Dept header file
 * **************************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Dept {
	private:
	int initial_price;
	vector <int> ask;
	vector <int> bid;

	public:
	Dept();
	Dept(int initP);
	~Dept();
	void UpdateDeptLevel();

};
