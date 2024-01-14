
//#pragma comment(lib,"lib\\atlimage.lib")
using namespace ATL;
bool collidePoint(POINT p, RECT rect) {
	return (rect.left <= p.x and p.x <= rect.right and rect.top <= p.y and p.y <= rect.bottom);
}

bool collideRect(RECT rect_1, RECT rect_2) {
	if ((rect_1.left <= rect_2.right and rect_2.left <= rect_1.right) and
		(rect_1.top <= rect_2.bottom and rect_2.top <= rect_1.bottom)) {
		return true;
	}
	else {
		return false;
	}
}

POINT middle(RECT rect) {
	POINT pt;
	pt.x = (rect.left + rect.right) / 2;
	pt.y = (rect.top + rect.bottom) /2;
	return pt;
}

RECT middle(RECT rect, RECT bg) {
	RECT r;
	POINT pt_1(middle(rect));
	POINT pt_2(middle(bg));
	r.left = rect.left - pt_1.x + pt_2.x;
	r.right = rect.right - pt_1.x + pt_2.x;
	r.top = rect.top - pt_1.y + pt_2.y;
	r.bottom = rect.bottom - pt_1.y + pt_2.y;
	return r;
}

void GetCImageBuf(CImage& image, char* &buf, int& len) {
	buf = (char*)image.GetBits();
	len = image.GetPitch();
}

void CImageLoadRGB(CImage& image, char* buf, int width, int height) {
	HRESULT hr;
	int nSize = width * height * 3; //BYTE*指向的数据的长度
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	IStream* iStream;

	bmpheader.bfType = 0x4d42; //'BM';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + nSize;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = 24;
	bmpinfo.biCompression = 0;
	bmpinfo.biSizeImage = 0;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = -height; //reverse the image

	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_NODISCARD, sizeof(bmpheader) + sizeof(BITMAPINFOHEADER) + nSize);
	iStream = NULL;
	hr = CreateStreamOnHGlobal(hMem, FALSE, &iStream);
	if (FAILED(hr)) { return;}

	hr = iStream->Write(&bmpheader, sizeof(bmpheader), NULL);
	hr = iStream->Write(&bmpinfo, sizeof(BITMAPINFOHEADER), NULL);
	hr = iStream->Write(buf, nSize, NULL);
	if (FAILED(hr)) { return;}

	image.Destroy();
	image.Load(iStream);

	iStream->Release();
	::GlobalUnlock(hMem);
	::GlobalFree(hMem);

}

void NewImage(CImage* &image, HDC& hdc, int w, int h) {
	DeleteDC(hdc);
	(*image).Destroy();
	delete (image);
	image = new CImage;
	(*image).Create(w, h, 32);
	hdc = (*image).GetDC();
}

class XFont {
public:
	int cHeight				= 30;
	int cWidth				= 0;
	int cEscapement			= 0;
	int cOrientation		= 0;
	int cWeight				= 600;
	DWORD bItalic			= 0;
	DWORD bUnderline		= 0;
	DWORD bStrikeOut		= 0;
	DWORD iCharSet			= ANSI_CHARSET;
	DWORD iOutPrecision		= OUT_DEFAULT_PRECIS;
	DWORD iClipPrecision	= CLIP_DEFAULT_PRECIS;
	DWORD iQuality			= 1000;
	DWORD iPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
	LPCSTR pszFaceName		= "宋体";
	COLORREF color			= RGB(0, 0, 0);

	HFONT hfont;
	void get_hfont() {
		DeleteObject(hfont);
		hfont = CreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic,
			bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision,
			iQuality, iPitchAndFamily,pszFaceName);
	}

	XFont(){
		get_hfont();
	}
};

void saveCImage(CImage* image, RECT rect, LPCSTR name) {
	CImage file;
	file.Create(rect.right, rect.bottom, 32);
	HDC hdc = file.GetDC();
	(*image).Draw(hdc, rect);
	file.Save(name);
	DeleteDC(file.GetDC());
	file.Destroy();
}
void saveHBITMAP(HBITMAP* image, RECT rect, LPCSTR name) {
	CImage file;
	file.Attach(*image);
	saveCImage(&file, rect, name);
	file.Destroy();
}
void copyImage(const CImage& src, CImage& dst)
{
	if (src.IsNull())
		return ;
	BYTE* srcPtr = (BYTE*)src.GetBits();
	int srcBitsCount = src.GetBPP();
	int srcWidth = src.GetWidth();
	int srcHeight = src.GetHeight();
	int srcPitch = src.GetPitch();
	if (!dst.IsNull())
	{
		dst.Destroy();
	}
	if (srcBitsCount == 32)
	{
		dst.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else
	{
		dst.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	if (srcBitsCount <= 8 && src.IsIndexed())
	{
		RGBQUAD pal[256];
		int nColors = src.GetMaxColorTableEntries();
		if (nColors > 0)
		{
			src.GetColorTable(0, nColors, pal);
			dst.SetColorTable(0, nColors, pal);
		}
	}
	BYTE* destPtr = (BYTE*)dst.GetBits();
	int destPitch = dst.GetPitch();
	if (srcPitch < 0) {
		memcpy(destPtr + destPitch * (srcHeight - 1), srcPtr + srcPitch * (srcHeight - 1), srcHeight * abs(srcPitch));
	}
	else {
		memcpy(destPtr, srcPtr, srcHeight * abs(srcPitch));
	}
	return ;
}

CImage _image(string ch, XFont* font, RECT r) {
	CImage letter;
	letter.Create(r.right, r.bottom, 32);
	HDC hdc(letter.GetDC());
	paintRECT(hdc, r, { 255,255,255 });
	SetTextColor(hdc, (*font).color);
	SelectObject(hdc, (HGDIOBJ)(HFONT)(*font).hfont);
	TextOutA(hdc, 0, 0, _char(ch), ch.length());
	letter.SetTransparentColor(RGB(255, 255, 255));

	return letter;
}

HDC _image(CImage* letter,string ch, XFont* font ,RECT r) {
	(*letter).Create(r.right,r.bottom, 32);

	HDC hdc((*letter).GetDC());
	paintRECT(hdc, r, { 255,255,255 });
	SetTextColor(hdc, (*font).color);
	SelectObject(hdc, (HGDIOBJ)(HFONT)(*font).hfont);
	TextOutA(hdc, 0, 0, _char(ch), ch.length());
	(*letter).SetTransparentColor(RGB(255, 255, 255));

	return hdc;
}

class XImages {
private:
	size_t len;
	char** name;
	CImage** image;
public:
	void init(int n) {
		len = n;
		name = new char* [n];
		image = new CImage * [n];
		for (int i = 0; i < n; i++) {
			image[i] = new CImage;
		}
	}
	void append(int n) {
		CImage** newImg = new CImage * [len + n];
		for (int i = 0; i < len; i++) {
			newImg[i] = image[i];
		}
		for (int i = n; i < len + n; i++) {
			newImg[i] = new CImage;
		}
		char** newStr = new char* [len + n];
		for (int i = 0; i < len; i++) {
			newStr[i] = name[i];
		}
		len = len + n;
		delete[] image;
		delete[] name;
		image = newImg;
		name = newStr;
	}
	void loadImage(int n, const char* str) {
		(*(image[n])).Destroy();
		(name[n]) = _char(str);
		(*(image[n])).Load(_char(str));
	}
	int searchImage(const char* str) {
		return posList(str, name, len);
	}
	bool isNull(int n) {
		return (*(image[n])).IsNull();
	}
	bool drawRECT(HDC hdc, RECT rect, COLOR color) {
		HBRUSH pbr = CreateSolidBrush(RGB(color.r, color.g, color.b));
		FillRect(hdc, &rect, pbr);
		DeleteObject(pbr);
		return true;
	}
	void drawImage(HDC hdc, int n, RECT rect) {
		(*(image[n])).Draw(hdc, rect);
	}
	void destroyImage(size_t n) {
		(*(image[n])).Destroy();
	}

	void drawStirng(HDC hdc, string str, POINT point, XFont* font) {
		int w ((*font).cHeight * str.length() * 3/5);
		int h ((*font).cHeight);
		CImage letter;
		HDC hdc_l(_image(&letter,str, font, { 0,0,w,h }));
		letter.Draw(hdc,{ point.x, point.y ,point.x+w, point.y+h });
		letter.Destroy();
		DeleteDC(hdc_l);
	}

	void destroyList() {
		//		delete name;
		for (int i = 0; i < len; i++) {
			if (!(*(image[i])).IsNull())
				(*(image[i])).Destroy();
			delete name[i];
			delete image[i];
		}
		delete[] name;
		delete[] image;
	}
};

class XButtons {
public:
	int num;
	RECT* Rect;
	CImage** Image;
	RECT* Rect_L;
	CImage** Image_L;

	void init(int n) {
		num = n;
		Rect = new RECT[n];
		Image = new CImage*[n];
		for (int i = 0; i < n; i++)
			Image[i] = new CImage;

		Rect_L = new RECT[n];
		Image_L = new CImage * [n];
		for (int i = 0; i < n; i++)
			Image_L[i] = new CImage;
	}
	void append(int n) {
		RECT* new_Rect = Rect;
		CImage** new_Image = Image;
		RECT* new_Rect_L = Rect_L;
		CImage** new_Image_L = Image_L;

		Rect = new RECT[num+n];
		for (int i = 0; i < num; i++)
			Rect[i] = new_Rect[i];
		
		Image = new CImage*[num+n];
		for (int i = 0; i < num; i++)
			Image[i] = new_Image[i];
		for (int i = 0; i < n; i++)
			Image[num + i] = new CImage;

		Rect_L = new RECT[num + n];
		for (int i = 0; i < num; i++)
			Rect_L[i] = new_Rect_L[i];

		Image_L = new CImage * [num + n];
		for (int i = 0; i < num; i++)
			Image_L[i] = new_Image_L[i];
		for (int i = 0; i < n; i++)
			Image_L[num + i] = new CImage;

		num = num + n;
	}
	void set(int n, const char* name, RECT rect, string str, XFont* font) {
		if (n < num) {
			Rect[n] = rect;
			(*(Image[n])).Destroy();
			(*(Image[n])).Load(_char(name));
			RECT r = { 0,0,
				(*font).cHeight * str.length() * 3 / 5,
				(*font).cHeight};
			Rect_L[n] = middle(r, rect);
			(*(Image_L[n])).Destroy();
			_image(Image_L[n],str, font, r);
		}
	}
	void destroy(int n) {
		(*(Image_L[n])).Destroy();
		(*(Image[n])).Destroy();
	}
	int collidepoint(POINT pt) {
		int value;
		for (int i = 0; i < num; i++) {
			if ( !((*(Image[i])).IsNull()) )
			if (collidePoint(pt, Rect[i])) {
				value = i;
				break;
			}
		}
		return value;
	}
	void draw(HDC hdc, int n) {
		(*(Image[n])).Draw(hdc, Rect[n]);
		(*(Image_L[n])).Draw(hdc, Rect_L[n]);

	}
	void draw(HDC hdc) {
		for (int i = 0; i < num; i++) {
			if (!((*(Image[i])).IsNull())) {
				(*(Image[i])).Draw(hdc, Rect[i]);
				(*(Image_L[i])).Draw(hdc, Rect_L[i]);
			}
		}
	}
};
