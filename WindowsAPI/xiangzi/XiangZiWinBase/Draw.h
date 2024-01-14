
bool paintRECT(HDC hdc, RECT rect, COLOR color) {
	HBRUSH pbr = CreateSolidBrush(RGB(color.r, color.g, color.b));
	FillRect(hdc, &rect, pbr);
	DeleteObject(pbr);
	return true;
}

bool paintHBITMAP(HDC hdc, HBITMAP image, RECT pic)
{
	if (image) {
		HDC hdcmem = CreateCompatibleDC(hdc);
		BITMAP bmp{};
		GetObject(image, sizeof(bmp), &bmp);
		SelectObject(hdcmem, image);
		StretchBlt(hdc, pic.left, pic.top, pic.right - pic.left, pic.bottom - pic.top, hdcmem, bmp.bmWidth, bmp.bmHeight, 0, 0, SRCCOPY);

		DeleteObject(&bmp);
		DeleteDC(hdcmem);
		return true;
	}
	else {
		PatBlt(hdc, pic.left, pic.top, pic.right - pic.left, pic.bottom - pic.top, BLACKNESS);
		return false;
	}
}
/*
bool paintCImage(HDC hdc, LPCTSTR file, RECT rect) {
	CImage Image= loadPNG(file);

	Image.Draw(hdc, rect);
	Image.Destroy();

	return true;
}
*/
