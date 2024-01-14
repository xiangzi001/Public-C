
string constr(string a, string b) {
	string n;
	n.append(a);
	n.append(b);
	return n;
}

string constr(string a, string b, string c) {
	string n;
	n.append(a);
	n.append(b);
	n.append(c);
	return n;
}
/*
char* _str(int i) {
	char ch[63];
	_itoa_s(i, ch, 10);
	return ch;
}
*/
string _str(int i) {
	return to_string(i);
}

char* _char(int n) {
	int len = 1;
	int num = n;
	while (1) {
		num = num/10;
		if (num < 1)
			break;
		else
			len++;
	}
	char* ch = new char[len];

	for (int i = len-1; i >= 0; i--) {
		ch[i] = n%10 + 48;
		n = n / 10;
	}
	return ch;
}

char* _char(string str) {
//	char* data = (char*)malloc((str.length() + 1) * sizeof(char));
	int len = str.length();
	char* data = new char[len];
	str.copy(data, len, 0);
	return data;
}

char* _char(const char* ch) {
	return const_cast<char*>(ch);
}

char* _char(wchar_t* wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
	char* psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}

wchar_t* _wchar(char* sText)
{
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sText, -1, NULL, 0);//把第五个参数设成NULL的到宽字符串的长度包括结尾符

	wchar_t* pwText = NULL;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete[]pwText;
		pwText = NULL;
	}
	unsigned nLen = MultiByteToWideChar(CP_ACP, 0, sText, -1, pwText, dwNum + 10);
	if (nLen >= 0)
	{
		pwText[nLen] = 0;
	}
	return pwText;
}

char* _char(LPCWSTR wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);//把第五个参数设成NULL的到宽字符串的长度包括结尾符
	char* psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}

int _int(string ch) {
	return atoi(_char(ch));
}


