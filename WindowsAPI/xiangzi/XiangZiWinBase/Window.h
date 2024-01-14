
#define WIN_CREATE 1
#define WIN_UPDATE 2
/*
void WaitFor(bool* b) {
	while (*b) {
		Sleep(1);
	}
}
*/
#ifndef SAFE_RELEASE(x)
#define SAFE_RELEASE(x) {	if (x) x->Release(); x = NULL;	}
#endif // !SAFE_RELEASE(x)

#ifndef WaitFor(b)
void WaitFor(bool* b){	while (*b){Sleep(1);}}
#endif // !WaitFor(b)

#ifndef WaitUntil(b)
void WaitUntil(bool* b) { while (!(*b)) { Sleep(1); } }
#endif // !WaitUntil(b)


#define PlaySound(name)	{	PlaySoundA(name, NULL, SND_ASYNC);	}
#define MessageBox(ch)	{	MessageBoxA(NULL, ch, "demo", MB_ICONEXCLAMATION | MB_OK);	}
struct Windows_h_Data{
	int windows_num;

	bool while_use = false;
	void* while_func;
	double* while_fps;
	bool* while_run;

	bool _Use;
	void* _Obj;
	int _FuncsNum;
	void* _Funcs;

	UINT_PTR TimerID = NULL;

}Windows_h;

void WINAPI _while() {
	void* func = Windows_h.while_func;
	bool* run = Windows_h.while_run;
	double maxFPS = *Windows_h.while_fps;
	Windows_h.while_use = false;

	microseconds duration;
	system_clock::time_point start;
	system_clock::time_point end;
	double spend_time;
	double max_spend_time = 1.0 / maxFPS;

	while ((*run)) {
		start = system_clock::now();

		(*(void(*)())func)();

		end = system_clock::now();
		duration = duration_cast<microseconds>(end - start);
		spend_time = (double(duration.count()) * microseconds::period::num / microseconds::period::den);
		if (max_spend_time > spend_time) {
			Sleep((max_spend_time - spend_time) * 1000);
		}
	}

}

void _SendMessage(void* obj, int msg);

class XWindow {
private:
	XMouse m_test = { false,false,false,false,false,false };
	XMouse m_now = { false,false,false,false,false,false };
	XMouse m_old = { false,false,false,false,false,false };
	XKey k_test;
	XKey k_now;
	XKey k_old;

	bool _draw;
	HDC hdc_1;
	HDC hdc_2;
public:
	bool use = false;

	char* name;
	HWND hwnd;
	HINSTANCE hInstance;
	HANDLE hThread;
	RECT ClientRect={ 0,0,600,400 };
	RECT WindowRect;

	RECT DrawRect={ 0,0,600,400 };
	HDC hdc;

	CImage* draw_1 = new CImage;
	CImage* draw_2 = new CImage;
	POINT mouse;
	char mouse_kind = 0;

	void changeDefinition(RECT rect) {
		DrawRect = rect;
		NewImage(draw_1, hdc_1, DrawRect.right, DrawRect.bottom);
		NewImage(draw_2, hdc_2, DrawRect.right, DrawRect.bottom);
	}
	void _Create() {
		if (use) {
			SendMessageA(hwnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
		}
		hwnd = CreateWindowExA(WS_EX_CLIENTEDGE, "WindowClass", name, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			(ClientRect).right, (ClientRect).bottom,
			NULL, NULL, hInstance, NULL);
		if (hwnd == NULL) {
			MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		HIMC g_hIMC = ImmAssociateContext(hwnd, NULL);
		
		changeDefinition(ClientRect);
		Windows_h.windows_num++;
		use = true;
	}

	void set(const char* Name, RECT Rect, HINSTANCE HInstance) {
		name = _char(Name);
		ClientRect = Rect;
		hInstance = HInstance;

	}
	void create() {
		_SendMessage(&(*this), WIN_CREATE);
	}
	void close() {
		Windows_h.windows_num--;
		use = false;
		//DeleteDC(hdc);
		(*draw_1).Destroy();
		(*draw_2).Destroy();
		//DeleteDC(hdc_1);
		//DeleteDC(hdc_2);
	}
	void update() {
		m_old = m_now;
		m_now = m_test;
		//m_test = { false,false,false,false,false,false };

		bool* k = k_old.key;
		k_old = k_now;
		k_now = k_test;
		k_test.key = k;
		k_test.reset();

		if (_draw) 
			hdc = hdc_1;
		else 
			hdc = hdc_2;
		_draw = !_draw;

		InvalidateRect(hwnd, NULL, false);
		//_SendMessage(&(*this),WIN_UPDATE);
	}
	void client_paint(HDC Window_hDc) {
		if (_draw)
			(*draw_1).Draw(Window_hDc, ClientRect);
		else
			(*draw_2).Draw(Window_hDc, ClientRect);
	}
	void join(void* func) {
		WaitUntil(&use);

		thread t_main(func);
		t_main.detach();
	}
	void join(void* func, double max_fps) {
		WaitUntil(&use);
		WaitFor(&(Windows_h.while_use));
		Windows_h.while_use = true;

		Windows_h.while_func = func;
		Windows_h.while_fps = &max_fps;
		Windows_h.while_run = &use;

		thread t_main(_while);
		t_main.detach();
	}
	void message_box(const char* ch) {
		MessageBox(NULL, ch, "demo", MB_ICONEXCLAMATION | MB_OK);
	}
	void mouse_getpos(LPARAM lParam) {
		mouse.x = GET_X_LPARAM(lParam) * DrawRect.right / ClientRect.right;
		mouse.y = GET_Y_LPARAM(lParam) * DrawRect.bottom / ClientRect.bottom;
	}
	bool mouse_click(UINT msg) {
		return m_test.mouseclick(msg);
	}
	//void mouse_next() {
		//m_now = m_test;
		//m_old = m_now;
		//m_test = { 0,false,false,false,false,false,false };
	//}
	void key_press(WPARAM wParam) {
		k_test.keypress(wParam);
	}
	//void key_next() {
	//	XKey temp = k_old;
	//	k_old = k_now;
	//	k_now = k_test;
	//	temp.reset();
	//	k_test = temp;
	//}
	bool if_mouse_press(UINT msg) {
		return m_now(msg);
	}
	bool if_mouse_pressonce(UINT msg) {
		return (m_now(msg) && (!m_old(msg)));
	}
	bool if_key_press(WPARAM wParam) {
		return k_now(wParam);
	}
	bool if_key_pressonce(WPARAM wParam) {
		return (k_now(wParam) && (!k_old(wParam)));
	}
};

void MainMessage(void* func) {
	WaitFor(&(Windows_h._Use));
	Windows_h._Use = true;
	Windows_h._FuncsNum = 0;
	Windows_h._Funcs = func;
}

void _SendMessage(void* obj, int msg) {
	WaitFor(&(Windows_h._Use));
	Windows_h._Use = true;
	Windows_h._Obj = obj;
	Windows_h._FuncsNum = msg;
}

bool DefWindowsThreadMessage() {
	if (Windows_h._Use) {
		switch (Windows_h._FuncsNum)
		{
		case 0: {
			(*((void(*)())(Windows_h._Funcs)))();
		}
		case WIN_CREATE: {
			(*(XWindow*)(Windows_h._Obj))._Create();
			break;
		}
		case WIN_UPDATE: {
			UpdateWindow((*(XWindow*)(Windows_h._Obj)).hwnd);
			break;
		}
		case CAMERA_START: {
			(*(XCamera*)(Windows_h._Obj))._Start();
			break;
		}
		case CAMERA_STOP: {
			(*(XCamera*)(Windows_h._Obj)).destroy();
		}
		default:
			break;
		}
		Windows_h._Use = false;
	}
	if (Windows_h.windows_num == 0) {
		PostQuitMessage(0);
		return false;
	}

	return true;
}

//void CALLBACK TimerProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime) {
//	DefWindowsThreadMessage();
//}

void WindowsInit(WNDPROC func, HINSTANCE hInstance, LPCSTR icon, UINT fps) {
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = func;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = "WindowClass";
	wc.hIcon = LoadIcon(hInstance, icon);
	wc.hIconSm = LoadIcon(hInstance, icon);
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
	Windows_h.windows_num = 0;
	Windows_h._Use = false;
	if(fps)
		Windows_h.TimerID = SetTimer(NULL, NULL, fps, NULL);
}

void WindowsStop() {
	Windows_h.windows_num = 0;
	KillTimer(NULL, Windows_h.TimerID);
}

/*
bool _Run;
XWindow* _Window;
RECT _Rect;
HINSTANCE _HInstance;
char* _Name;
void WinMSGMain() {
	XWindow* win = _Window;
	RECT rect = _Rect;
	HINSTANCE hInstance = _HInstance;
	char* name = _Name;
	_Run = false;

	(*win).create_window(name, rect, hInstance);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void XWindow::create(const char* name, RECT rect, HINSTANCE hInstance) {
	while(_Run){}
	_Run = true;
	_Window = &(*this);
	_Rect = rect;
	_HInstance = hInstance;
	_Name = _char(name);

	thread WinMSG(WinMSGMain);
	WinMSG.detach();
}

void GetMSG() {
	MSG msg;
	while (1) {
		if (_Create) {
			XWindowData win = _Window;

			if ((*win.use)) {
				SendMessageA(*win.hwnd, WM_DESTROY, NULL, NULL);
			}
			(*win.use) = true;

			(*win.hwnd) = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", win.name, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				(*win.rect).right, (*win.rect).bottom,
				NULL, NULL, NULL, NULL);
			if ((*win.hwnd) == NULL) {
				MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
				return;
			}
			_Create = false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
*/
