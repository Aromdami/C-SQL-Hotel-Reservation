#pragma once
#include <occi.h>
#include <iostream>
#include <iomanip>
using namespace oracle::occi;
using namespace std;
class Employees
{
public:
	Employees();
	~Employees();
	void List();
private:
	Environment* env;
	Connection* con;
	string user;
	string passwd;
	string dbCon;
};