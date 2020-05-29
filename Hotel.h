#pragma once
#include <occi.h>
#include <iostream>
#include <iomanip>
using namespace oracle::occi;
using namespace std;
class Hotel
{
public:
	Hotel();
	virtual ~Hotel();
	/*공용 메뉴*/
	void showAllRooms(); //테이블 테스트용 방 전부 출력
	int SignIn();	//로그인
	void SignUp();	//회원가입

	/*매니저 전용 함수*/
	void rooms_occupied();	//점유된 방 확인
	void housekeeping();	//하우스키핑

	/*일반 사용자*/
	void rooms_available();	//이용가능 방
	void cost_at_checkout();//청구서 출력
	void make_reservation();//예약 신청
	void my_reservations();	//예약 확인

protected:
	Environment* env;
	Connection* con;
	string user;
	string passwd;
	string dbCon;
};
