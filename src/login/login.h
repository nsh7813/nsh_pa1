/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : login.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 7, 2021.
 * - Description : login system header file
 * **************************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class User_info {
	private:
	vector <string> id;
	vector <string> pw;
	int user_num;

	public:
	User_info();				// default constructor
	~User_info();			
	int getUserNum() {return user_num;}	// returen how many user
	bool isUser(string Id, string pwd);	// check id/pw is valid
	bool idCheck(string Id);		// check id 
	bool pwCheck(string pwd);		// check pw
	void addNewUser(string Id, string pwd);	// sign in new user

};
