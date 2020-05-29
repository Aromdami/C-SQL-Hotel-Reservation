#include "Table.h"
#include <iostream>
#include <occi.h>
#include <iomanip>
using namespace std;
using namespace oracle::occi;

int main() {
	Table* table = new Table();
	int select = 9;
	
	
	while (select != 0)
	{
		cout << "==============================" << endl
			<< "1. Show Data" << endl
			<< "2. Add Data" << endl
			<< "3. Search by Locations" << endl
			<< "0. quit" << endl
			<< "==============================" << endl << "Select option : "; cin >> select;

		if (select == 1)
			table->Show();
		else if (select == 2)
			table->Add();
		else if (select == 3)
			table->JoinLoc();
		else if (select == 0)
		{
			cout << "Exiting..." << endl;
			break;
		}
	}
	

	delete table;
	
	return 0;
}
