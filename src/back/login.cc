/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : login.cc
 * - File Type : c++ source file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 7, 2021.
 * - Description : login system source file
 * **************************************************************************************************/

#include "login.h"


User_info::User_info() {
	user_num = 0;
	usr_list.clear();
}

User_info::~User_info() {
	usr_list.clear();
}

bool User_info::isUser(string Id, string pwd) {
	if (idCheck(Id) && pwCheck(pwd)) return true;
	return false;
}

void User_info::addNewUser(string Id, string pwd) {
	user new_usr;
	if (idCheck(Id)) {
		cout << "Can not use this ID!\n";
		return;
	}
	if (pwCheck(pwd)) {
		cout << "Can not use this password!\n";
	}
	new_usr = user(Id, pwd);
	usr_list.push_back(new_usr);
	user_num++;
}

bool User_info::idCheck(string Id) {
	for (size_t i = 0; i < getUserNum(); i++) {
		if (usr_list.at(i).getID().length() != Id.length()) continue;
		if (usr_list.at(i).getID() != Id) continue;
		return true;
	}
	return false;
}

bool User_info::pwCheck(string pwd) {
	for (size_t i = 0; i < getUserNum(); i++) {
		if (usr_list.at(i).getPW().length() != pwd.length()) continue;
		if (usr_list.at(i).getPW() != pwd) continue;
		return true;
	}
	return false;
}
