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
	/*���� �޴�*/
	void showAllRooms(); //���̺� �׽�Ʈ�� �� ���� ���
	int SignIn();	//�α���
	void SignUp();	//ȸ������

	/*�Ŵ��� ���� �Լ�*/
	void rooms_occupied();	//������ �� Ȯ��
	void housekeeping();	//�Ͽ콺Ű��

	/*�Ϲ� �����*/
	void rooms_available();	//�̿밡�� ��
	void cost_at_checkout();//û���� ���
	void make_reservation();//���� ��û
	void my_reservations();	//���� Ȯ��

protected:
	Environment* env;
	Connection* con;
	string user;
	string passwd;
	string dbCon;
};
