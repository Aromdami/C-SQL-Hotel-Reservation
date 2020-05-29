#pragma once
#include <occi.h>
#include <iostream>
#include <iomanip>
using namespace oracle::occi;
using namespace std;
class Table
{
public:
	Table ();
	virtual ~Table();
	void Show();
	void Add();
	void JoinLoc();
private:
	Environment* env;
	Connection* con;
	string user;
	string passwd;
	string dbCon;
};