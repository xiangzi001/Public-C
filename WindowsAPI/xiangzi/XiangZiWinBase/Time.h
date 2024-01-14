
class XTime {
public:
	int year;
	int month;
	int day;
	int h;
	int m;
	int s;
	XTime() {
		year = 0;
		month = 0;
		day = 0;
		h = 0;
		m = 0;
		s = 0;
	}
	XTime(tm time) {
		year = time.tm_year + 1900;
		month = time.tm_mon + 1;
		day = time.tm_mday;
		h = time.tm_hour;
		m = time.tm_min;
		s = time.tm_sec;
	}
	XTime(int* n) {
		year = n[0];
		month = n[1];
		day = n[2];
		h = n[3];
		m = n[4];
		s = n[5];
	}
	void next_year() {
		year += 1;
	}
	void next_month() {
		if (month == 12) {
			next_year();
			month = 1;
		}
		else
			month += 1;
	}
	void next_day() {
		if (day >= 28) {
			int a = 0;
			if (month == 2)
				a = 3;
			else if (month < 8)
				a = 2 - (month % 2);
			else
				a = 1 + month % 2;

			switch (a)
			{
			case 1: {
				if (day == 31)
					break;
				else
					return;
			}
			case 2: {
				if (day == 30)
					break;
				else
					return;
			}
			case 3:
				break;
			}

			next_month();
			day = 1;
		}
		else
			day += 1;
	}
	void next_h() {
		if (h == 23) {
			next_day();
			h = 0;
		}
		else
			h += 1;
	}
	void next_m() {
		if (m == 59) {
			next_h();
			m = 0;
		}
		else
			m += 1;
	}
	void next_s() {
		if (s == 59) {
			next_m();
			s = 0;
		}
		else
			s += 1;
	}
	void get_time() {
		struct tm tmp = { 0 };
		time_t t = time(0);
		localtime_s(&tmp, &t);
		*this = tmp;
	}
};

string strtm(XTime tm) {
	string str;
	str.append(_str(tm.year));
	str.append(".");
	str.append(_str(tm.month));
	str.append(".");
	str.append(_str(tm.day));
	str.append(" ");
	str.append(_str(tm.h));
	str.append(".");
	str.append(_str(tm.m));
	str.append(".");
	str.append(_str(tm.s));
	return str;
}

class XClocks {
private:
	int num;
	XTime* clock;
	bool** check;
public:
	void init(int n) {
		num = n;
		clock = new XTime[n];
		check = new bool*[n];
	}
	void set(int n, XTime t) {
		clock[n] = t;
	}
	bool ring(int n) {
		XTime t;
		t.get_time();
		if		(clock[n].s != t.s)
			return false;
		else if (clock[n].m != t.m)
			return false;
		else if (clock[n].h != t.h)
			return false;
		else if (clock[n].day != t.day)
			return false;
		else if (clock[n].month != t.month)
			return false;
		else if	(clock[n].year	!= t.year)
			return false;
		else 
			return true;
	}

	void next_year(int n) {
		clock[n].next_year();
	}
	void next_month(int n) {
		clock[n].next_month();
	}
	void next_day(int n) {
		clock[n].next_day();
	}
	void next_h(int n) {
		clock[n].next_h();
	}
	void next_m(int n) {
		clock[n].next_m();
	}
	void next_s(int n) {
		clock[n].next_s();
	}
};

class XTimers {
private:
	int num = 0;
	double* time;
	system_clock::time_point* start;
	bool* ring_;
public:
	void init(int n) {
		num = n;
		time = new double[n];
		start = new system_clock::time_point[n];
		ring_ = new bool[n];
	}
	void stop(int n) {
		time[n] = 0;
		start[n] = system_clock::now();
		ring_[n] = false;
	}
	void set(int n, double t) {
		time[n] = t;
		start[n] = system_clock::now();
		ring_[n] = true;
	}
	void set(int n) {
		start[n] = system_clock::now();
		ring_[n] = false;
	}
	double ring(int n) {
		if (time[n] == 0)
			return false;
		else {
			double spend_time = (double((duration_cast<microseconds>(system_clock::now() - start[n])).count()) * microseconds::period::num / microseconds::period::den);
			if (ring_[n]) {
				if (spend_time < time[n]) {
					return false;
				}
				else {
					return true;
				}
			}
			else
				return spend_time;
		}
	}

};