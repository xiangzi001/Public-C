
HBITMAP ScreenCapture(WORD BitCount, RECT rect)
{
//	rect = { rect.left * 2,rect.top * 2,rect.bottom * 2,rect.right * 2 };
	LPRECT lpRect(&rect);
	HBITMAP hBitmap;
	// 显示器屏幕DC
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hmemDC = CreateCompatibleDC(hScreenDC);
	// 显示器屏幕的宽和高
//	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
//	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	int ScreenWidth (GetSystemMetrics(SM_CXSCREEN));
	int ScreenHeight(GetSystemMetrics(SM_CYSCREEN));
	// 旧的BITMAP，用于与所需截取的位置交换
	HBITMAP hOldBM;
	// 保存位图数据
	PVOID lpvpxldata = nullptr;
	// 截屏获取的长宽及起点
	INT ixStart;
	INT iyStart;
	INT iX;
	INT iY;
	// 位图数据大小
	DWORD dwBitmapArraySize=0;
	// 几个大小
	DWORD nBitsOffset=0;
	DWORD lImageSize=0;
	DWORD lFileSize=0;
	// 位图信息头
	BITMAPINFO bmInfo{};
	// 位图文件头
	BITMAPFILEHEADER bmFileHeader{};
	// 写文件用
	HANDLE hbmfile = nullptr;
	DWORD dwWritten=0;

	// 如果LPRECT 为NULL 截取整个屏幕
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
	// 创建BTIMAP
	hBitmap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	// 将BITMAP选择入内存DC。
	hOldBM = (HBITMAP)SelectObject(hmemDC, hBitmap);
	// BitBlt屏幕DC到内存DC，根据所需截取的获取设置参数
	BitBlt(hmemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	// 将旧的BITMAP对象选择回内存DC，返回值为被替换的对象，既所截取的位图
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
	hWnd = (hWnd == NULL) ? GetDesktopWindow() : hWnd; //判断窗口句柄是否为NULL,如果为NULL则默认获取桌面DC
	hDc = GetDC(hWnd); //获取DC
	if (hDc == NULL) return false;
	int bitOfPix = GetDeviceCaps(hDc, BITSPIXEL); //获取DC像素的大小
	int width = GetDeviceCaps(hDc, HORZRES);  //获取DC宽度
	int hight = GetDeviceCaps(hDc, VERTRES);  //获取DC高度
	UINT dpi = GetDpiForWindow(hWnd); //获取dpi
	float fold = dpi/96; //根据dpi计算放大倍数
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
	width *= fold; //复原宽度
	hight *= fold; //复原高度
	CImage image;
	image.Create(width, hight, bitOfPix); //为图像类创建与窗口DC相同大小的DC
	BitBlt(image.GetDC(), 0, 0, width, hight, hDc, 0, 0, SRCCOPY); //将窗口DC图像复制到image
	image.Save(_char(name)); //保存为png格式图片文件
	image.ReleaseDC(); //释放DC
	ReleaseDC(hWnd, hDc); //释放DC资源
}

void SaveWindow(string name, HWND hwnd) {
	HDC hDC = ::GetDC(hwnd);//获取屏幕DC
	RECT rect;
	::GetClientRect(hwnd, &rect);//获取屏幕大小
	HDC hDCMem = ::CreateCompatibleDC(hDC);//创建兼容DC
	HBITMAP hBitMap = ::CreateCompatibleBitmap(hDC, rect.right, rect.bottom);//创建兼容位图
	HBITMAP hOldMap = (HBITMAP)::SelectObject(hDCMem, hBitMap);//将位图选入DC，并保存返回值
	::BitBlt(hDCMem, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);//将屏幕DC的图象复制到内存DC中
	CImage image;
	image.Attach(hBitMap);
	image.Save(_char(name));//如果文件后缀为.bmp，则保存为为bmp格式
	image.Detach();
	::SelectObject(hDCMem, hOldMap);//选入上次的返回值
	//释放
	::DeleteObject(hBitMap);
	::DeleteDC(hDCMem);
	::DeleteDC(hDC);
}