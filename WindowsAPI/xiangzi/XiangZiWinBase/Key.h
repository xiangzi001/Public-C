//mouse
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN					0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
//key
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define VK_A							0x41
#define VK_B							0x42
#define VK_C							0x43
#define VK_D							0x44
#define VK_E							0x45
#define VK_F							0x46
#define VK_G							0x47
#define VK_H							0x48
#define VK_I							0x49
#define VK_J							0x4A
#define VK_K							0x4B
#define VK_L							0x4C
#define VK_M							0x4D
#define VK_N							0x4E
#define VK_O							0x4F
#define VK_P							0x50
#define VK_Q							0x51
#define VK_R							0x52
#define VK_S							0x53
#define VK_T							0x54
#define VK_U							0x55
#define VK_V							0x56
#define VK_W							0x57
#define VK_X							0x58
#define VK_Y							0x59
#define VK_Z							0x5A

//other 
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define Down(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)


bool press_once(bool a, bool b) {
	return (a and a != b);
}

class XKey
{
private:
public:
	bool* key = new bool[254];
	void reset() {
		for (int i = 0; i < 255; i++)
			key[i] = false;
	}
	void keypress(WPARAM wParam) {
		key[wParam] = true;
	}
	bool operator ()(WPARAM wParam) {
		return key[wParam];
	}
	~XKey() {
		delete[] key;
	}
};
/*
class XKey
{
private:
public:

	bool a; 
	bool b; 
	bool c; 
	bool d; 
	bool e;
	bool f; 
	bool g; 
	bool h; 
	bool i; 
	bool j;
	bool k; 
	bool l; 
	bool m; 
	bool n; 
	bool o;
	bool p; 
	bool q; 
	bool r; 
	bool s; 
	bool t;
	bool u; 
	bool v; 
	bool w; 
	bool x; 
	bool y;
	bool z;
	bool f1; 
	bool f2; 
	bool f3; 
	bool f4; 
	bool f5; 
	bool f6;
	bool f7; 
	bool f8; 
	bool f9; 
	bool f10; 
	bool f11; 
	bool f12;
	bool tab; 
	bool caps; 
	bool shift; 
	bool ctrl; 
	bool enter;

	void reset() {
		a = false;
		b = false;
		c = false;
		d = false;
		e = false;
		f = false;
		g = false;
		h = false;
		i = false;
		j = false;
		k = false;
		l = false;
		m = false;
		n = false;
		o = false;
		p = false;
		q = false;
		r = false;
		s = false;
		t = false;
		u = false;
		v = false;
		w = false;
		x = false;
		y = false;
		z = false;
		f1 = false;
		f2 = false;
		f3 = false;
		f4 = false;
		f5 = false;
		f6 = false;
		f7 = false;
		f8 = false;
		f9 = false;
		f10 = false;
		f11 = false;
		f12 = false;
		tab = false;
		caps = false;
		shift = false;
		ctrl = false;
		enter = false;
	}

	void keypress(WPARAM wParam) {
		switch (wParam) {
		case VK_A:  a = true; break;
		case VK_D:  d = true; break;
		case VK_S:  s = true; break;
		case VK_W:  w = true; break;
		case VK_F1: f1 = true; break;
		case VK_F2: f2 = true; break;
		case VK_F3: f3 = true; break;
		}
	}
};
*/

class XMouse
{
private:
public:
	bool m_l;
	bool m_m;
	bool m_r;
	bool m_x;

	bool w_up;
	bool w_down;

	void reset() {
		*this = { false,false,false,false,false,false };
	}

	bool mouseclick(UINT msg) {
		switch (msg)
		{
		case WM_LBUTTONDOWN:{
			m_l = true;
			return true;
		}
		case WM_LBUTTONUP:{
			m_l = false;
			return true;
		}
		case WM_MBUTTONDOWN:{
			m_m = true;
			return true;
		}
		case WM_MBUTTONUP:{
			m_m = false;
			return true;
		}
		case WM_XBUTTONDOWN:{
			m_x = true;
			return true;
		}
		case WM_XBUTTONUP:{
			m_x = false;
			return true;
		}
		case WM_RBUTTONDOWN:{
			m_r = true;
			return true;
		}
		case WM_RBUTTONUP: {
			m_r = false;
			return true;
		}
		default:
			return false;
		}
	}
	bool operator() (UINT msg) {
		switch (msg)
		{
		case WM_LBUTTONDOWN: {
			return m_l;
		}
		case WM_LBUTTONUP: {
			return (!m_l);
		}
		case WM_MBUTTONDOWN: {
			return m_m;
		}
		case WM_MBUTTONUP: {
			return (!m_m);
		}
		case WM_XBUTTONDOWN: {
			return m_x;
		}
		case WM_XBUTTONUP: {
			return (!m_x);
		}
		case WM_RBUTTONDOWN: {
			return m_r;
		}
		case WM_RBUTTONUP: {
			return (!m_r);
		}
		default:
			return false;
		}
	}
	void mousewheel(WPARAM wParam) {
		w_up = (wParam == VK_W);
		w_down = (wParam == VK_F2);
	}
};