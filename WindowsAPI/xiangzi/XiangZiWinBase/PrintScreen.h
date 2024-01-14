
HBITMAP ScreenCapture(WORD BitCount, RECT rect)
{
//	rect = { rect.left * 2,rect.top * 2,rect.bottom * 2,rect.right * 2 };
	LPRECT lpRect(&rect);
	HBITMAP hBitmap;
	// ��ʾ����ĻDC
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);
	// ��ʾ����Ļ�Ŀ�͸�
//	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
//	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	int ScreenWidth (GetSystemMetrics(SM_CXSCREEN));
	int ScreenHeight(GetSystemMetrics(SM_CYSCREEN));
	// �ɵ�BITMAP�������������ȡ��λ�ý���
	HBITMAP hOldBM;
	// ����λͼ����
	PVOID lpvpxldata = nullptr;
	// ������ȡ�ĳ������
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;
	// λͼ���ݴ�С
	DWORD dwBitmapArraySize=0;
	// ������С
	DWORD nBitsOffset=0;
	DWORD lImageSize=0;
	DWORD lFileSize=0;
	// λͼ��Ϣͷ
	BITMAPINFO bmInfo{};
	// λͼ�ļ�ͷ
	BITMAPFILEHEADER bmFileHeader{};
	// д�ļ���
	HANDLE hbmfile = nullptr;
	DWORD dwWritten=0;

	// ���LPRECT ΪNULL ��ȡ������Ļ
	if (lpRect == NULL)
	{
		ixStart = iyStart = 0;
		iX = ScreenWidth;
		iY = ScreenHeight;
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}
	// ����BTIMAP
	hBitmap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	// ��BITMAPѡ�����ڴ�DC��
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);
	// BitBlt��ĻDC���ڴ�DC�����������ȡ�Ļ�ȡ���ò���
	BitBlt(hmemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	// ���ɵ�BITMAP����ѡ����ڴ�DC������ֵΪ���滻�Ķ��󣬼�����ȡ��λͼ
	hBitmap = (HBITMAP)SelectObject(hmemDC, hOldBM);
	DeleteDC(hScreenDC);
	DeleteDC(hmemDC);
	return hBitmap;
}

HBITMAP ScreenCapture(HWND hwnd)
{
	RECT WinRect;
	GetWindowRect(hwnd, &WinRect);
	return ScreenCapture(32, WinRect);
}

bool SaveScreen(string name, HWND hWnd) {
	HDC hDc = NULL;
	hWnd = (hWnd == NULL) ? GetDesktopWindow() : hWnd; //�жϴ��ھ���Ƿ�ΪNULL,���ΪNULL��Ĭ�ϻ�ȡ����DC
	hDc = GetDC(hWnd); //��ȡDC
	if (hDc == NULL) return false;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //��ȡDC���صĴ�С
	int width = GetDeviceCaps(hDc, HORZRES);  //��ȡDC���
	int hight = GetDeviceCaps(hDc, VERTRES);  //��ȡDC�߶�
	UINT dpi = GetDpiForWindow(hWnd); //��ȡdpi
	float fold = dpi/96; //����dpi����Ŵ���
	/*
	switch (dpi) {
	case 96:
		fold = 1;
		break;
	case 120:
		fold = 1.25;
		break;
	case 144:
		fold = 1.5;
		break;
	case 192:
		fold = 2;
		break;
	case 216:
		fold = 2.25;
		break;
	default:
		fold = 1;
		break;
	}
	*/
	width *= fold; //��ԭ���
	hight *= fold; //��ԭ�߶�
	CImage image;
	image.Create(width, hight, bitOfPix); //Ϊͼ���ഴ���봰��DC��ͬ��С��DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //������DCͼ���Ƶ�image
	image.Save(_char(name)); //����Ϊpng��ʽͼƬ�ļ�
	image.ReleaseDC(); //�ͷ�DC
	ReleaseDC(hWnd, hDc); //�ͷ�DC��Դ
}

void SaveWindow(string name, HWND hwnd) {
	HDC hDC = ::GetDC(hwnd);//��ȡ��ĻDC
	RECT rect;
	::GetClientRect(hwnd, &rect);//��ȡ��Ļ��С
	HDC hDCMem = ::CreateCompatibleDC(hDC);//��������DC
	HBITMAP hBitMap = ::CreateCompatibleBitmap(hDC, rect.right, rect.bottom);//��������λͼ
	HBITMAP hOldMap = (HBITMAP)::SelectObject(hDCMem, hBitMap);//��λͼѡ��DC�������淵��ֵ
	::BitBlt(hDCMem, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);//����ĻDC��ͼ���Ƶ��ڴ�DC��
	CImage image;
	image.Attach(hBitMap);
	image.Save(_char(name));//����ļ���׺Ϊ.bmp���򱣴�ΪΪbmp��ʽ
	image.Detach();
	::SelectObject(hDCMem, hOldMap);//ѡ���ϴεķ���ֵ
	//�ͷ�
	::DeleteObject(hBitMap);
	::DeleteDC(hDCMem);
	::DeleteDC(hDC);
}