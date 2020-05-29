#include "Table.h"
#include <stdexcept>
using namespace std;
using namespace oracle::occi;
/*
* connect to the test database as the HR sample user and use the EZCONNECT
* method of specifying the connect string. Be sure to adjust for your
* environment! The format of the string is host:port/service_name
*/
Table::Table()
{
	user = "hr";
	passwd = "oracle";
	dbCon = "//localhost:1521/xe";
	env = Environment::createEnvironment(Environment::DEFAULT);
	

	try {
		cout << "Preparing Instant Client environment ... ";
		env = Environment::createEnvironment(Environment::DEFAULT);
		cout << "OK" << endl;
	}
	catch (...) {
		cout << "Failed" << endl;
		return;
	}
	try {
		cout << "Connecting to server ... ";
		con = env->createConnection(user, passwd, dbCon);
		cout << "OK" << endl;
	}
	catch (...) {
		cout << "Failed" << endl;
		exit(EXIT_FAILURE);
	}
}
Table::~Table()
{
	env->terminateConnection(con);
	Environment::terminateEnvironment(env);
}

/*
* simple test method to select data from the employees table and display the results
*/
void Table::Show()
{
	Statement* stmt;
	ResultSet* rs;
	int sel;
	while (1) {

		cout << "Select Table to show" << endl;
		cout << "1. EMP (Employees) 2. Dept (Departments)" << endl;
		cin >> sel;

		if (sel == 1)
		{
			try {
				int  count = 0;
				cout << "Loading Current EMP Table..." << endl;
				stmt = con->createStatement();
				rs = stmt->executeQuery("SELECT EMPNO, ENAME, DEPTNO FROM EMP");
				while (rs->next()) {
					count++;
					int test_id;
					string test_name;
					int test_no;
					test_id = rs->getInt(1);
					test_name = rs->getString(2);
					test_no = rs->getInt(3);

					cout << "#" << setw(3) << count << "  id : " << setw(5) << test_id << "\tname : " << setw(10) << test_name << "\tdeptno : " << setw(2) << test_no << endl;
				}
				if (count == 0)
					cout << "Current Table is Empty" << endl;
				stmt->closeResultSet(rs);
				con->terminateStatement(stmt);
				cout << " ... OK" << endl;
				break;
			}
			catch (...) {
				cout << " ... Failed" << endl;
				break;
			}
		}
		else if (sel == 2)
		{
			try {
				int  count = 0;
				cout << "Loading Current DEPT Table..." << endl;
				stmt = con->createStatement();
				rs = stmt->executeQuery("SELECT * FROM DEPT");
				while (rs->next()) {
					count++;
					int test_id;
					string test_name;
					string test_loc;
					test_id = rs->getInt(1);
					test_name = rs->getString(2);
					test_loc = rs->getString(3);

					cout << "#" << setw(3) << count << "  Department Number : " << setw(2) << test_id << "\tDepartment Name : " << setw(15) << test_name << "\tLocation : " << setw(2) << test_loc << endl;
				}
				if (count == 0)
					cout << "Current Table is Empty" << endl;
				stmt->closeResultSet(rs);
				con->terminateStatement(stmt);
				cout << " ... OK" << endl;
				break;
			}
			catch (...) {
				cout << " ... Failed" << endl;
				break;
			}
		}
		else
			cout << "wrong Input!" << endl;
	}
}
void Table::Add()
{
	Statement* stmt;
	int sel;
	while (1) {

		cout << "Select Table to add data" << endl;
		cout << "1. EMP (Employees) 2. Dept (Departments)" << endl;
		cin >> sel;

		
		if (sel == 1)
		{
			try {
				cout << "Enter Informations of an Employee" << endl;
				string empno, ename, deptno;
				cout << "Employee Number   : "; cin >> empno;
				cout << "Employee Name     : "; cin >> ename;
				cout << "Department Number : "; cin >> deptno;

				for (int i = 0; i < ename.size(); i++)
					ename[i] = toupper(ename[i]);
			


				string qry = "INSERT INTO EMP VALUES (" + empno + ", '" + ename + "', " + deptno + ")";
				
				stmt = con->createStatement();
				stmt->executeUpdate(qry);
				
				con->terminateStatement(stmt);
				cout << " ... Succeded" << endl;
				break;
			}
			catch (...) {
				cout << " ... Failed" << endl;
				break;
			}
		}
		else if (sel == 2)
		{
			try {
				cout << "Enter Informations of an Department" << endl;
				string deptno, dname, loc;
				cout << "Department Number : "; cin >> deptno;
				cout << "Department Name   : "; cin >> dname;
				cout << "Location          : "; cin >> loc;
			
				for (int i = 0; i < dname.size(); i++)
					dname[i] = toupper(dname[i]);
				for (int i = 0; i < loc.size(); i++)
					loc[i] = toupper(loc[i]);

				string qry = "INSERT INTO DEPT VALUES (" + deptno + ", '" + dname + "', '" + loc + "')";
				stmt = con->createStatement();
				stmt->executeUpdate(qry);

				con->terminateStatement(stmt);
				cout << " ... Succeded" << endl;
				break;
			}
			catch (...) {
				cout << " ... Failed" << endl;
				break;
			}
		}
		else
			cout << "wrong Input!" << endl;
	}
}

void Table::JoinLoc()
{
	Statement* stmt;
	ResultSet* rs;
	string search;
	string result;
	try {
		int  count = 0;
		cout << "Loading Available Locations..." << endl;
		stmt = con->createStatement();
		rs = stmt->executeQuery("SELECT LOC FROM DEPT");
		while (rs->next()) {
			count++;
			string test_loc;
			test_loc = rs->getString(1);
			cout << "#" << setw(3) << count << " "<< setw(20) << test_loc << endl;
		}
		if (count == 0)
			cout << "Current Table is Empty" << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
		return;
	}

	while (1)
	{
		cout << "Enter Department's Location Searching for : "; cin >> search;
		for (int i = 0; i < search.size(); i++)
			search[i] = toupper(search[i]);
		result ="SELECT e.EMPNO, e.ENAME, d.DNAME FROM EMP e JOIN DEPT d ON e.deptno = d.deptno WHERE d.LOC ='" + search + "'";
		try {
			int  count = 0;
			cout << "Searching Employees at " << search << endl;
			stmt = con->createStatement();
			rs = stmt->executeQuery(result);
			while (rs->next()) {
				count++;
				int eno;
				string ename;
				string dname;

				eno = rs->getInt(1);
				ename = rs->getString(2);
				dname = rs->getString(3);

				cout << "#" << setw(3) << count << "  ID : " << setw(5) << eno << "\tName : " << setw(10) << ename << "\tDepartment : " << setw(15) << dname << endl;
			}
			if (count == 0)
				cout << "No matching result found" << endl;
			stmt->closeResultSet(rs);
			con->terminateStatement(stmt);
			cout << " ... Loaded" << endl;
			break;
		}
		catch (...) {
			cout << " ... Failed" << endl;
			break;;
		}
	}
}