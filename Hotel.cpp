#define _CRT_SECURE_NO_WARNINGS
#include "Hotel.h"
#include <stdexcept>
using namespace std;
using namespace oracle::occi;

#define NUMBER_OF_ROOMS 25
static string thisyear, thismonth, thisday;
static string curUser;
static int reservno = 0;


/*프로그램상 오늘 날짜 확인용 함수*/
const string currentDateTime() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y", &tstruct); //YYYY
	thisyear = buf;
	strftime(buf, sizeof(buf), "%m", &tstruct); //MM
	thismonth = buf;
	strftime(buf, sizeof(buf), "%Y/%m/%d", &tstruct); //DD
	thisday = buf;
	strftime(buf, sizeof(buf), "%Y/%m/%d", &tstruct); //YYYY/MM/DD
	
	return buf;
}


Hotel::Hotel()
{
	user = "hr";
	passwd = "oracle";
	dbCon = "//localhost:1521/xe";
	/*연결 환경 구축, 실패시 catch로 failed 문구 출력*/
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
	/*연결 구축, 실패시 catch 로 failed 문구 출력*/
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
Hotel::~Hotel()
{
	env->terminateConnection(con);
	Environment::terminateEnvironment(env);
}

int Hotel::SignIn()	//mananger - 1, customer - 2, failed - 0
{
	Statement* stmt;
	ResultSet* rs;
	int sel;
	while (1) {
		cout << "Sign in as" << endl;
		cout << "1. a Manager" << endl << "2. a customer" << endl << ">>>";
		cin >> sel;
		if (sel == 1)
		{
			try {
				int check = 0;
				int pass;
				cout << "Password : "; cin >> pass;
				cout << "Loading Current EMP Hotel..." << endl;
				stmt = con->createStatement();
				rs = stmt->executeQuery("SELECT ACCID, ACCPASS FROM ACCOUNTS WHERE ACCTYPE = 'MGR'");
				while (rs->next()) {
					string accid;
					int accpass;
					accid = rs->getString(1);
					accpass = rs->getInt(2);
					if (accid == "manager" && accpass == pass)
					{
						check++;
						break;
					}
				}
				stmt->closeResultSet(rs);
				con->terminateStatement(stmt);
				if (check == 0)
				{
					cout << "Wrong password" << endl;
					break;
				}
				else
				{
					cout << "signed in as a manager" << endl;
					curUser = "manager";
					return 1;
				}
			}
			catch (...) {
				cout << " ... Failed to connect server" << endl;
				return 0;
			}
		}
		else if (sel == 2)
		{
			try {
				int  check = 0;
				int pass;
				string id;
				cout << "User ID  : "; cin >> id;
				cout << "Password : "; cin >> pass;
				cout << "Signing in..." << endl;
				stmt = con->createStatement();
				rs = stmt->executeQuery("SELECT ACCID, ACCPASS, CNAME FROM ACCOUNTS WHERE ACCTYPE = 'STD'");
				while (rs->next()) {
					check++;
					string accid;
					int accpass;
					string username;
					accid = rs->getString(1);
					accpass = rs->getInt(2);
					username = rs->getString(3);

					if (accid == id && accpass == pass)
					{
						check++;
						cout << "signed in!" << endl;
						cout << "Welcome, " << username << endl;
						curUser = accid;
						break;
					}
				}
				stmt->closeResultSet(rs);
				con->terminateStatement(stmt);
				if (check == 0)
				{
					cout << "Wrong password" << endl;
					break;
				}
				else
					return 2;
			}
			catch (...) {
				cout << " ... Failed to connect server" << endl;
				break;
			}
		}
		else
			cout << "wrong Input!" << endl;
	}
	return 0;
}
void Hotel::SignUp()
{
	Statement* stmt;
	int sel;
	while (1) {
		string uid= "", uname = "", pass = "";
		cout << "New UserName (2 ~ 5 Length) : "; while (uname.length() > 10 || uname.length() < 2)cin >> uname;
		cout << "New User ID (3 ~ 10 Length) : "; while (uid.length() > 10 || uid.length() < 3) cin >> uid;
		cout << "New Password     (4 Digits) : "; while (pass.length() != 4) cin >> pass;

		try {
			string qry = "INSERT INTO ACCOUNTS VALUES ('" + uid + "', " + pass + ", '" + uname + "', 'STD')";

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
}

void Hotel::showAllRooms()
{
	Statement* stmt;
	ResultSet* rs;
	try {
		int  count = 0;
		cout << "Loading All rooms' information from Hotel..." << endl;
		stmt = con->createStatement();
		rs = stmt->executeQuery("SELECT * FROM ROOMS");
		cout << "RoomNumber\tRoomType\tCost per Day\tLast Checkin\tLast Checkout\tRoomService Done" << endl;
		while (rs->next()) {
			count++;
			cout << "     " << setw(4) << setfill('0') << rs->getInt(1) << "\t"
				<< setw(8) << setfill(' ') << rs->getString(2) << "\t    " << setw(5) << rs->getInt(3) << "\t"
				<< (rs->isNull(4) ? "      -     " : "  " + rs->getDate(4).toText("YYYY/MM/DD")) << "\t"
				<< (rs->isNull(5) ? "      -     " : "  " + rs->getDate(5).toText("YYYY/MM/DD")) << "\t"
				<< "       " << rs->getString(6) << endl;
		}
		if (count == 0)
			cout << "Current Table is Empty" << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
	}
}

void Hotel::rooms_occupied()
{
	/*시스템상 날짜와 각 방의 최근 checkout/checkin을 비교하여 */
	Statement* stmt;
	ResultSet* rs;
	try {
		int  count = 0;
		cout << "Loading Currently occupied Rooms... [" << currentDateTime() << "]" << endl;
		stmt = con->createStatement();
		//작성된 질의가 길어져 문장을 구분함
		string s1 = "SELECT RNUMBER, RTYPE, LCHECKIN FROM ROOMS";			//호실, 종류, 최근 체크인 날짜를 선택
		string s2 = " WHERE (LCHECKIN is not NULL AND LCHECKOUT is NULL)";	//체크인이 되어있으나 아직 체크아웃이 없는 상태
		string s3 = " OR (LCHECKIN < SYSDATE AND LCHECKOUT < LCHECKIN)";	//체크아웃이 오늘이 아닌 과거의 것으로 된 상태
		rs = stmt->executeQuery(s1 + s2 + s3);
		
		cout << "RoomNumber\tRoomType\tCheck In Date" << endl;
		while (rs->next()) {
			count++;
			cout << "     " << setw(4) << setfill('0') << rs->getInt(1) << "\t"
				<< setw(8) << setfill(' ') << rs->getString(2) << "\t  "
				<< (rs->isNull(3) ? "      -     " : rs->getDate(3).toText("YYYY/MM/DD")) << endl;
		}
		if (count == 0)
			cout << "Current Table is Empty" << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
		cout << "currently " << count << " rooms are occupied." << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
	}
}

void Hotel::housekeeping()
{
	static string kp[3] = { "0000-00-00","0000-00-00", "0000-00-00" };
	//각 하우스키핑 실시 날짜를 저장함. 순서대로 복도, 화장실, 비품확인
	int ans;
	
	cout << "==List of HouseKeeping==" << endl;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
			cout << "1. Clean Up Hallways ";
		else if (i == 1)
			cout << "2. Clean Up Restrooms ";
		else if (i == 2)
			cout << "3. Supply Check ";

		/*최근 처리 날짜와 현재날짜를 비교하여 체크*/
		if (kp[i] == currentDateTime())
			cout << "Done" << endl;
		else
		{
			while (1)
			{
				cout << "(1. Done / 2. Not Done) :";
				cin >> ans;
				if (ans == 1)
				{
					kp[i] = currentDateTime();
					break;
				}
				else if (ans == 2)
					break;
				else
					cout << "wrong input" << endl;
			}
		}
	}
	/*각 객실별 하우스키핑 확인을 위한 방 나열*/
	Statement* stmt;	//각 호실 호출용 query
	Statement* stmt2;	//청소 후 업데이트용 query
	ResultSet* rs;
	try {
		int  count = 0;
		cout << "Loading Rooms in Need of HouseKeep... "<< endl;
		stmt = con->createStatement();
			
		rs = stmt->executeQuery("SELECT RNUMBER, RTYPE FROM ROOMS WHERE CLNUP = 'X'");
		while (rs->next()) {
			count++;
			while (1) {
				cout << "Room    NO." << setw(4) << setfill('0') << rs->getInt(1) << "\t"
					<< setw(8) << setfill(' ') << "(" << rs->getString(2) << ") [1. Done  / 2. Not Done] : ";
				cin >> ans;
				if (ans == 1)
				{
					stmt2 = con->createStatement();
					stmt2->executeUpdate("UPDATE ROOMS SET CLNUP = 'O' WHERE RNUMBER =" + rs->getString(1));
					//해당 방을 청소했다고 답할 시, 청소 상태를 O로 변경시켜줌
					con->terminateStatement(stmt2);
					break;
				}
				else if (ans == 2)
					break;
				else
					cout << "wrong answer!" << endl;
			}
		}
		if (count == 0)
			cout << "All Rooms are Cleaned Up." << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		
	}
	catch (...) {
		cout << " ...connection Failed" << endl;
	}
}
void Hotel::rooms_available()
{
	Statement* stmt;
	ResultSet* rs;
	try {
		int  count = 0;
		cout << "Loading Currently Available Rooms... [" << currentDateTime() << "]" << endl;
		stmt = con->createStatement();
		//작성된 질의가 길어져 문장을 구분함
		string s1 = "SELECT RNUMBER, RTYPE, RCOST FROM ROOMS ";				//호실, 종류, 하루당 숙방비용을 선택
		string s2 = "WHERE (LCHECKIN IS NULL OR ";							//체크인 기록이 없는 상태
		string s3 = "(LCHECKOUT < SYSDATE AND LCHECKIN < LCHECKOUT)) ";		//마지막 체크아웃이 하루 전이며, 그 체크아웃이 마지막 숙박이었을 경우
		string s4 = "AND CLNUP = 'O'";										//해당 방의 하우스키핑이 완료된 상태
		rs = stmt->executeQuery(s1 + s2 + s3 + s4);

		cout << "RoomNumber\tRoomType\tCost Per Day" << endl;
		while (rs->next()) {
			count++;
			cout << "     " << setw(4) << setfill('0') << rs->getInt(1) << "\t"
				<< setw(8) << setfill(' ') << rs->getString(2) << "\t    " << setw(5) << rs->getInt(3) << endl;
		}
		if (count == 0)
			cout << "All Rooms are Unavailable." << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
		cout << "currently " << count << " rooms are available." << endl;
	}
	catch (...) {
		cout << " ... connection Failed" << endl;
	}
}

void Hotel::make_reservation()
{
	Statement* stmt;
	Statement* stmt2;
	Statement* stmt3;
	ResultSet* rs;

	int select, ans, count = 0;
	string rnum, rtype, chkin, chkout;
	string avail[NUMBER_OF_ROOMS];		//Rnum 선택용 
	int cost;
	cout << "[Rooms Reservation Service]" << endl;

	//방 종류 선택
	while (1)
	{
		cout << "Which Type of Rooms are you interested in?" << endl
			<< "1. Standard (cost : 30000 per day)" << endl
			<< "2. Deluxe   (cost : 45000 per day)" << endl
			<< "3. Suite    (cost : 60000 per day)" << endl
			<< "4. Cancel reservation" << endl << ">>> "; cin >> select;
		if (select == 1) { rtype = "standard"; break; }
		else if (select == 2) { rtype = "deluxe"; break; }
		else if (select == 3) { rtype = "suite"; break; }
		else if (select == 4) return;
		else cout << "Wrong input!" << endl;
	}
	
	//선택한 종류의 방 중 예약 가능한 리스트를 출력//
	try {
		cout << "Loading Currently Available Rooms... [" << currentDateTime() << "]" << endl;
		stmt = con->createStatement();
			
		//위의 available list에서 특정 rtype만을 불러와 선택에 맞는 방을 호출하도록 질의 수정//
		string s1 = "SELECT RNUMBER, RTYPE, RCOST FROM ROOMS ";				
		string s2 = "WHERE (LCHECKIN IS NULL OR ";							
		string s3 = "(LCHECKOUT < SYSDATE AND LCHECKIN < LCHECKOUT)) ";
		string s4 = "AND CLNUP = 'O' AND RTYPE = '" + rtype +"'";
		rs = stmt->executeQuery(s1 + s2 + s3 + s4);

		cout << "RoomNumber\tRoomType\tCost Per Day" << endl;
		while (rs->next()) {
		
			avail[count++] = rs->getString(1);	//string 형 배열 avail에 선택되는 방번호를 기록
			cost = rs->getInt(3);			
			cout << "     " << setw(4) << setfill('0') << rs->getInt(1) << "\t"
				<< setw(8) << setfill(' ') << rs->getString(2) << "\t    " << setw(5) << rs->getInt(3) << endl;
		}
		if (count == 0)
			cout << "All Rooms are Unavailable." << endl;
		else
			cout << "currently " << count << " rooms are available." << endl << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
	}
	catch (...) {
		cout << " ... connection Failed" << endl;
		return;
	}
	//호실 선택//
	while (1)
	{
		cout << "PLease Select Your Room Number" << endl;
		for (int i = 1; i < count + 1; i++)
			cout << i << ". " << avail[i-1] << endl;
		cout << ">>> "; cin >> select;
		if (select > 0 && select < count + 1) { rnum = avail[select - 1];	break; }
		else	cout << "wrong input" << endl;
	}
	//날짜 선택//
	
	string year, month, day;
STARTRESERV:
	while (1) {
		cout << "Please choose a year for reservation (YYYY) : "; cin >> year;
		if (year.length() == 4 && year >= thisyear)	break;
		else cout << "Wrong Input!" << endl;
	}
	while (1) {
		cout << "Please choose a month for reservation (MM) : "; cin >> month;
		if (month.length() == 2)	break;
		else cout << "Wrong Input!" << endl;
	}
	while (1) {
		cout << "Please choose a day for reservation (DD) : "; cin >> day;
		if (day.length() == 2)	break;
		else cout << "Wrong Input!" << endl;
	}
	if ((year + "/" + month + "/" + day) < currentDateTime())
	{
		cout << "Error in date configuration!\nPlease check again. (Today is " << currentDateTime << ")" << endl;
		goto STARTRESERV;
	}
	
	Date calc(env, atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str())); //날짜 계산용 SQL형 날짜 자료형
	chkin = calc.toText("YYYYMMDD");
		
	cout << "How many days do you want to stay? : "; cin >> select;
	if (select >= 0)
	{
		cout << endl << "Your Reservation will be from " << calc.toText("YYYY/MM/DD") << " to " << calc.addDays(select).toText("YYYY/MM/DD") << endl;
		calc = calc.addDays(select);
		cout << "Expected accomodation fee will be : " << cost * select << endl;
		while (1)
		{
			cout << "Are you Sure ? (1. Yes 2. No) : "; 
			cin >> ans;
			if (ans == 1)
			{
				chkout = calc.toText("YYYYMMDD");
				break;
			}
			else if (ans == 2)
				goto STARTRESERV;
			else
				cout << "Wrong Input!" << endl;
		}
				
	}
	else
		cout << "Wrong Input!" << endl;
		

	//Reservation Table 작성//
	try {
		cout << "Updating Reservation infos";
		reservno = (reservno + 1) % 999999;
		string s1 = "INSERT INTO RESERVATION VALUES (" + to_string(reservno)+", ";	//예약번호 등록
		string s2 = rnum + ", '" + rtype + "', NULL, ";								//객실 번호 및 종류 등록. rcost는 update 질의로 대신함
		string s3 = "TO_DATE(" + chkin + ", 'YYYYMMDD'), TO_DATE(" + chkin + ", 'YYYYMMDD'), ";	//체크인 아웃 날짜
		string s4 = "'" + curUser + "')";

		/*예약 정보 등록*/
		stmt2 = con->createStatement();
		stmt2->executeUpdate(s1 + s2 + s3 + s4);
		con->terminateStatement(stmt2);

		/*호실 가격 업데이트*/
		stmt3 = con->createStatement();
		stmt3->executeUpdate("UPDATE RESERVATION res SET RCOST = (SELECT RCOST FROM ROOMS rm WHERE rm.rnumber = res.roomnum) WHERE RCOST is NULL");
		con->terminateStatement(stmt3);
		cout << " ... Succeded" << endl;
	}
	catch (...)
	{
		cout << " ... Failed" << endl;
	}
}

void Hotel::my_reservations()
{
	Statement* stmt;
	ResultSet* rs;
	try {
		int  count = 0;
		cout << "Loading all reservations..." << endl;
		stmt = con->createStatement();
		string s1 = "SELECT RESNUMBER, ROOMNUM, RTYPE, CHECKIN, CHECKOUT FROM RESERVATION ";
		string s2 = "WHERE CUSTOMERID = '" + curUser + "'";
		rs = stmt->executeQuery(s1 + s2);

		cout << "ReservNumber\tRoomNumber\tRoomType\tCheck in Date\tCheck out Date" << endl;
		while (rs->next()) {
			count++;
			cout << "   " << setw(6) << setfill('0') << rs->getInt(1) << "\t"
				<< "     " << setw(4) << rs->getInt(2) << "\t " << setw(8) << rs->getString(3) << "\t"
				<< (rs->isNull(4) ? "      -     " : "  " + rs->getDate(4).toText("YYYY/MM/DD")) << "\t"
				<< (rs->isNull(5) ? "      -     " : "  " + rs->getDate(5).toText("YYYY/MM/DD")) << endl;
		}
		if (count == 0)
			cout << "There are no reservations found." << endl;
		stmt->closeResultSet(rs);
		con->terminateStatement(stmt);
		cout << " ... Loaded" << endl;
	}
	catch (...) {
		cout << " ... connection Failed" << endl;
	}
}

void Hotel::cost_at_checkout()
{
	Statement* stmt1;
	//1)룸 별 체크인최신화
	try {
		string qry = "UPDATE ROOMS SET lcheckin = (SELECT MIN(checkin) FROM RESERVATION )";
		stmt1 = con->createStatement();
		stmt1->executeUpdate(qry);
		con->terminateStatement(stmt1);
		cout << " ... Succeded" << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
		return;
	}

	//2)체크아웃을 할 객실을 선택
	Statement* stmt2;
	ResultSet* rs;
	
	try {
		int  count = 0;
		cout << "Loading all reservations..." << endl;
		stmt2 = con->createStatement();
		string s1 = "SELECT RESNUMBER, ROOMNUM, RTYPE, CHECKIN, CHECKOUT FROM RESERVATION ";
		string s2 = "WHERE CUSTOMERID = '" + curUser + "' AND TO_CHAR(checkin, 'YYYYMMDD')< TO_CHAR(SYSDATE, 'YYYYMMDD')";
		rs = stmt2->executeQuery(s1 + s2);

		cout << "ReservNumber\tRoomNumber\tRoomType\tCheck in Date\tCheck out Date" << endl;
		while (rs->next()) {
			count++;
			cout << "   " << setw(6) << setfill('0') << rs->getInt(1) << "\t"
				<< "     " << setw(4) << rs->getInt(2) << "\t " << setw(8) << rs->getString(3) << "\t"
				<< (rs->isNull(4) ? "      -     " : "  " + rs->getDate(4).toText("YYYY/MM/DD")) << "\t"
				<< (rs->isNull(5) ? "      -     " : "  " + rs->getDate(5).toText("YYYY/MM/DD")) << endl;
		}
		if (count == 0)
		{
			cout << "There are no reservations can be checked out." << endl;
			return;
		}
		stmt2->closeResultSet(rs);
		con->terminateStatement(stmt2);
		cout << " ... Loaded" << endl;
	}
	catch (...) {
		cout << " ... connection Failed" << endl;
	}
	string cktrnum;
	while (1) {
		cout << "Please Enter Exact Number of Room to CheckOut : "; cin >> cktrnum;

		if (cktrnum.length() != 4)
			cout << "Wrong Input!" << endl;
		else
			break;
	}
	//3) 해당 객실 체크아웃날짜 갱신
	Statement* stmt3;
	try {
		cout << "Checking out...";
		string qry = "UPDATE ROOMS SET lcheckout = SYSDATE WHERE rnumber = " + cktrnum;
		stmt3 = con->createStatement();
		stmt3->executeUpdate(qry);
		con->terminateStatement(stmt3);
		cout << " ... Succeded" << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
		return;
	}
	int ans;
	//4) 청구서 출력을 위한 정보 출력
	Statement* stmt4;
	ResultSet* rs2;
	int total;
	string target;
	try {
		int  count = 0, originalcost, interval;

		cout << "Loading bill..." << endl;
		stmt4 = con->createStatement();
		string sel = "SELECT act.cname, res.roomnum, res.rtype, res.checkin,res.checkout, rms.lcheckout, rms.rcost, res.resnumber ";
		string join = "FROM RESERVATION res JOIN ROOMS rms ON res.roomnum = rms.rnumber	JOIN ACCOUNTS act ON act.accid = res.customerid ";
		string cond = "WHERE TO_CHAR(res.checkin, 'YYYYMMDD') = TO_CHAR(rms.lcheckin, 'YYYYMMDD') AND res.roomnum = " + cktrnum;
		rs2 = stmt4->executeQuery(sel + join + cond);

		cout << endl << "[Total Receit]" << endl;
		cout << "Customer Name\tRoomNumber\tRoomType\tCheck in Date\tReserved CheckOut\tCheck Out Date" << endl;
		while (rs2->next()) {
			count++;
			cout << setw(10) << setfill(' ') << rs2->getString(1) << "\t"
				<< "     " << setw(4) << setfill('0') << rs2->getInt(2) << "\t "
				<< setw(8) << setfill(' ') << rs2->getString(3) << "\t"
				<< (rs2->isNull(4) ? "      -     " : "  " + rs2->getDate(4).toText("YYYY/MM/DD")) << "\t"
				<< (rs2->isNull(5) ? "      -     " : "  " + rs2->getDate(5).toText("YYYY/MM/DD")) << "\t\t"
				<< (rs2->isNull(6) ? "      -     " : "  " + rs2->getDate(6).toText("YYYY/MM/DD")) << endl << endl;

			
			originalcost = rs2->getDate(5).daysBetween(rs2->getDate(4)).getDay() * rs2->getInt(7);
			interval = rs2->getDate(6).daysBetween(rs2->getDate(5)).getDay();
			target = rs2->getString(8);
			cout << "#original cost : " << originalcost << endl;
			if (interval < 0)
				cout << "Discount for " << -interval << " days" << endl;
			else if (interval > 0)
				cout << "Extra fee for " << interval << " days" << endl;
			cout << "#total cost : " << originalcost + interval * rs2->getInt(7) << endl;
			
		}
		if (count == 0)
		{
			cout << "an Error has occured. No accomodation Found." << endl;
			return;
		}
		stmt4->closeResultSet(rs2);
		con->terminateStatement(stmt4);
		cout << endl;
	}
	catch (...) {
		cout << " ... disconnected from Server." << endl;
	}

	//5) 체크아웃 실시 문의 후 체크아웃 진행
	while (1) {
		cout << "Are you sure to check out? (1.Yes / 2. No) : "; cin >> ans;
		if (ans == 1) break;
		else if (ans == 2) return;
		else cout << "Wrong Input!" << endl;
	}
	Statement* stmt5;
	try {
		cout << "Please wait";
		string qry = "DELETE FROM RESERVATION WHERE resnumber = " + target;
		stmt5 = con->createStatement();
		stmt5->executeUpdate(qry);
		con->terminateStatement(stmt5);
		cout << " ... Succeded" << endl;
	}
	catch (...) {
		cout << " ... Failed" << endl;
		return;
	}
}
