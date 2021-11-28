#ifndef _ORDER_H_
#define _ORDER_H_

#include "user.h"
#include "dept.h"
#include <iostream>
#include <vector>
#include <string>
#include <queue>


class ORDER : public Dept{
	private:
	int last_oid;
	map<int, order> ordbook;
	vector<vector<int>> Abook;
	vector<vector<int>> Bbook;

	public:
	ORDER();
	ORDER(int _initP, int _p_code);
	~ORDER();
	int getLastOID();
	void addLastOID();
	map<int, order> getOrderbook();
	order getOrderbyID(int oid);
	int calcOrder(order raw_data, char AB);
	int delOrder(int oid);
};

#endif
