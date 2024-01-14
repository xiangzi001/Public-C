
extern "C" {
#include "libavcodec/avcodec.h"
#pragma comment(lib, "avcodec.lib")

#include "libavformat/avformat.h"
#pragma comment(lib, "avformat.lib")

#include "libavutil/avutil.h"
#include "libavutil/frame.h"
#pragma comment(lib, "avutil.lib")

#include "libavutil/imgutils.h"
#include "libavutil/opt.h"

#include "libavdevice/avdevice.h"
#pragma comment(lib, "avdevice.lib")

#include "libavfilter/avfilter.h"
#include "libpostproc/postprocess.h"

#include "libswresample/swresample.h"
#pragma comment(lib, "swresample.lib")

#include "libswscale/swscale.h"
#pragma comment(lib, "swscale.lib")
}


#include"XiangZiFFmpeg/PlaySound.h"
#include"XiangZiFFmpeg/ReadVideo.h"