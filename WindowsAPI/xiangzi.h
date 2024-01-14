
//socket===========================================================
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
//windows===========================================================
#include <windows.h>

#include <cstring>
#include <imm.h>  
#pragma comment (lib ,"imm32.lib")
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")
#include <string>

#include <memory.h>
#include <atlimage.h>

#include <fstream>

#include <ctime>
#include <chrono>   

#include <uuids.h>
#include <amvideo.h>
#include <vector>
#include <strmif.h>
#include <dshow.h>
#pragma comment(lib, "strmiids")
#include <Digitalv.h>
//math===============================================================
#include <cmath>
//===============================================================
using namespace std;
using namespace std::chrono;
//===============================================================

#include "xiangzi\XiangZiSocketBase.h"
#include "xiangzi\XiangZiWinBase.h"
#include "xiangzi\XiangZiMathBase.h"
#include "xiangzi\XiangZiFFmpeg.h"
