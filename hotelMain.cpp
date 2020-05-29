#include "Hotel.h"
#include <iostream>
#include <occi.h>
#include <iomanip>
using namespace std;
using namespace oracle::occi;

void managerMod(Hotel* hotel);	//매니저 전용 메뉴용 함수
void customerMod(Hotel* hotel);	//고객 전용 메뉴용 함수

int main() {
	Hotel* hotel = new Hotel();
	int select = 9;
	int mod;
	
	while (select != 0)
	{
		cout << "==============================" << endl
			<< "1. Sign In" << endl
			<< "2. Sign Up" << endl
			<< "3. Show All Rooms in Hotel" << endl
			<< "0. Exit" << endl
			<< "==============================" << endl << "Select option : "; cin >> select;
		if (select == 1)
		{
			mod = hotel->SignIn(); //mod = 1 : manager 2 : customer 0 : error
			if (mod == 1)
				managerMod(hotel);
			else if (mod == 2)
				customerMod(hotel);
			else
				cout << "Error Occured. Please Try again Later..." << endl << endl;
		}
		else if (select == 2)
			hotel->SignUp();
		else if (select == 3)
			hotel->showAllRooms();
		else
			cout << "wrong input!" << endl;
	}
	delete hotel;

	return 0;
}

void managerMod(Hotel* hotel)
{
	int select = 0;
	while (select != 3)
	{
		cout << "=======================" << endl
			<< "[Manager Mod]" << endl
			<< "1.Check Occupied Rooms" << endl
			<< "2.List House Keeping" << endl
			<< "3.Sign Out" << endl
			<< "=======================" << endl << "Select Option : "; cin >> select;
		
		if (select == 1)
			hotel->rooms_occupied();
		else if (select == 2)
			hotel->housekeeping();
		else if (select == 3)
			return;
		else
			cout << "wrong input" << endl;
	}
}

void customerMod(Hotel* hotel)
{
	int select = 0;
	while (select != 5)
	{
		cout << "=======================" << endl
			<< "[Customer Mod]" << endl
			<< "1.Check Available Rooms" << endl
			<< "2.Check Cost at Checkout" << endl
			<< "3.Make Reservation" << endl
			<< "4.My Reservations" << endl
			<< "=======================" << endl << "Select Option : "; cin >> select;

		if (select == 1)
			hotel->rooms_available();
		else if (select == 2)
			hotel->cost_at_checkout();xm
		else if (select == 3)
			hotel->make_reservation();
		else if (select == 4)
			hotel->my_reservations();
		else
			cout << "wrong input" << endl;
	}
}