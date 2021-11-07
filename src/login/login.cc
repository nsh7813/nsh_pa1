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
	id.clear();
	pw.clear();
}

User_info::~User_info() {
	id.clear();
	pw.clear();
}

bool User_info::isUser(string Id, string pwd) {
	if (idCheck(Id) && pwCheck(pwd)) return true;
	return false;
}

void User_info::addNewUser(string Id, string pwd) {
	if (idCheck(Id)) {
		cout << "Can not use this ID!\n";
		return;
	}
	if (pwdCheck(pwd)) {
		cout << "Can not use this password!\n";
	}
	id.push_back(Id);
	pwd.push_back(pwd);
	user_num++;
}

bool User_info::idCheck(string Id) {
	for (size_t ii = 0; ii < getUserNum(); ii++) {
		if (id.at(i).length() != Id.length()) continue;
		if (id.at(i) != Id) continue;
		return true;
	}
	return false;
}

bool User_info::pwCheck(string pwd) {
	for (size_t ii = 0; ii < getUserNum(); ii++) {
		if (pw.at(i).length() != pwd.length()) continue;
		if (pw.at(i) != pwd) continue;
		return true;
	}
	return false;
}
