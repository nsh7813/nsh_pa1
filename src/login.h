/* ***************************************************************************************************
 * Copyright (c) 2021 Seung Hyeon Roh All Right Reserved
 *
 * - File Name : login.h
 * - File Type : c++ header file
 * - File Version(Last Update Date) : 1.0
 * - Date : Nov. 7, 2021.
 * - Description : login system header file
 * **************************************************************************************************/


#include "user.h"

using namespace std;

bool idCheck(user usr, string Id) {		// check id 
	if (usr.getID().length() != Id.length()) return false;
	if (usr.getID() != Id) return false;
	return true;
}

bool pwCheck(user usr, string pwd) {		// check pw
	if (usr.getPW().length() != pwd.length()) return false;
	if (usr.getPW() != pwd) return false;
	return true;
}

int isUser(vector<user> usrs, string Id, string pwd) {	// check id/pw is valid
	for (size_t i = 0; i < usrs.size(); i++) {
		if (idCheck(usrs.at(i), Id) && pwCheck(usrs.at(i), pwd)) return (int) i;
	}
	return -1;
}

extern void error_handling(const char* message);
