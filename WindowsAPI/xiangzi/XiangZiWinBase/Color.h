
//Ò»°ãÑÕÉ« 
#define	BLACK			0
#define	BLUE			0xAA0000
#define	GREEN			0x00AA00
#define	CYAN			0xAAAA00
#define	RED				0x0000AA
#define	MAGENTA			0xAA00AA
#define	BROWN			0x0055AA
#define	LIGHTGRAY		0xAAAAAA
#define	DARKGRAY		0x555555
#define	LIGHTBLUE		0xFF5555
#define	LIGHTGREEN		0x55FF55
#define	LIGHTCYAN		0xFFFF55
#define	LIGHTRED		0x5555FF
#define	LIGHTMAGENTA	0xFF55FF
#define	YELLOW			0x55FFFF
#define	WHITE			0xFFFFFF
//RGB
//#define BGR(color)	( (((color) & 0xFF) << 16) | ((color) & 0xFF00FF00) | (((color) & 0xFF0000) >> 16) )

typedef struct COLOR
{
    BYTE r;
    BYTE g;
    BYTE b;
}COLOR;

COLOR rgbCOLORREF(COLORREF color) {
    COLOR rgb = {};
    rgb.r = GetRValue(color);
    rgb.g = GetGValue(color);
    rgb.b = GetBValue(color);
    return rgb;
}
