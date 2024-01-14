
void saveBmp(AVFrame* frame, int bpp)
{
	BITMAPFILEHEADER bmpHeader = { 0 };
	bmpHeader.bfType = 'M' << 8 | 'B';
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfSize = bmpHeader.bfOffBits + frame->width * frame->height * bpp / 8;

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = frame->width;
	bmpInfo.bmiHeader.biHeight = -frame->height;  // 图像上下颠倒
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 100;
	bmpInfo.bmiHeader.biYPelsPerMeter = 100;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	//std::ofstream fout("1.bmp", std::ofstream::out | std::ofstream::binary);
	//std::shared_ptr<std::ofstream> foutCloser(&fout, [](std::ofstream* f) { f->close(); });
	//fout.write(reinterpret_cast<const char*>(&bmpHeader), sizeof(BITMAPFILEHEADER));
	//fout.write(reinterpret_cast<const char*>(&bmpInfo.bmiHeader), sizeof(BITMAPINFOHEADER));
	//fout.write(reinterpret_cast<const char*>(frame->data[0]), frame->width * frame->height * bpp / 8);
	///*
	FILE* fp = fopen("1.png", "wb");
	fwrite(&bmpHeader, 1, sizeof(bmpHeader), fp);
	fwrite(&bmpInfo.bmiHeader, 1, sizeof(BITMAPINFOHEADER), fp);
	fwrite(frame->data[0], 1, frame->width * frame->height * bpp / 8, fp);
	fclose(fp);
	//*/
	HRESULT hr;
	/*
	int nSize = frame->width * frame->height * bpp / 8; //BYTE*指向的数据的长度
	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_NODISCARD, sizeof(bmpHeader) + sizeof(BITMAPINFOHEADER) + nSize);
	iStream = NULL;
	hr = CreateStreamOnHGlobal(hMem, TRUE, &iStream);
	if (FAILED(hr)) {  return; }
	hr = iStream->Write(&bmpHeader, sizeof(bmpHeader), NULL);
	hr = iStream->Write(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), NULL);
	hr = iStream->Write(frame->data[0], nSize, NULL);
	//if (FAILED(hr)) { Windows_h.windows_num = 0; return; }
	ULARGE_INTEGER pos;
	LARGE_INTEGER iMove;
	iMove.QuadPart = 0;
	hr = iStream->Seek(iMove, STREAM_SEEK_SET, &pos);
	//*/
	/*
	int len = sizeof(bmpHeader) + sizeof(BITMAPINFOHEADER) + frame->width * frame->height * bpp / 8;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
	void* pData = GlobalLock(hGlobal);
	memcpy((char*)pData, &bmpHeader, sizeof(bmpHeader));
	memcpy((char*)pData + sizeof(bmpHeader), &bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	memcpy((char*)pData + sizeof(bmpHeader) + sizeof(BITMAPINFOHEADER), frame->data[0], frame->width * frame->height * bpp / 8);
	GlobalUnlock(hGlobal);

	CreateStreamOnHGlobal(hGlobal, TRUE, &iStream); 

	//*/

}

int saveAsBitmap(AVFrame* pFrameRGB, int width, int height, const char* fileName)
{
	FILE* pFile = NULL;
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;

	int bpp = 24;

	bmpheader.bfType = 0x4d42; //'BM';
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width * height * bpp / 8;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = -height; //reverse the image
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = bpp;
	bmpinfo.biCompression = 0;
	bmpinfo.biSizeImage = 0;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;
	
	pFile = fopen(fileName, "wb");
	if (!pFile)
		return 0;

	fwrite(&bmpheader, sizeof(BITMAPFILEHEADER), 1, pFile);
	fwrite(&bmpinfo, sizeof(BITMAPINFOHEADER), 1, pFile);
	uint8_t* buffer = pFrameRGB->data[0];
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			fwrite(buffer, 1, 1, pFile);
			fwrite(buffer + 1, 1, 1, pFile);
			fwrite(buffer + 2, 1, 1, pFile);
			buffer += 3;
		}
	}
	fclose(pFile);
	return 1;
}

class XVideo {
private:
	AVFormatContext* formatContext = avformat_alloc_context();
	// 寻找音频流和视频流
	int audioStreamIndex = -1;
	int videoStreamIndex = -1;
	// 获取音频解码器和视频解码器
	AVCodecParameters* audioCodecParameters;
	AVCodecParameters* videoCodecParameters;
	AVCodec* audioCodec;
	AVCodec* videoCodec;
	// 打开音频解码器和视频解码器
	AVCodecContext* audioCodecContext;
	AVCodecContext* videoCodecContext;

	AVPacket packet;
	AVFrame* frame;

	SwrContext* swrContext;
	AVFrame* convertedFrame;

	SwsContext* sws_ctx;
	AVFrame* rgb_frame;
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
public:
	int bpp = 24;
	int width;
	int height;
	CImage video;
	shared_ptr<AudioPlayer> sound;

	HRESULT Open(const char* path) {
		HRESULT hr = CoInitialize(NULL);

		formatContext = avformat_alloc_context();

		// 打开输入文件
		if (avformat_open_input(&formatContext, path, NULL, NULL) != 0) {
			return -1;
		}

		// 检索流信息
		if (avformat_find_stream_info(formatContext, NULL) < 0) {
			return -1;
		}

		// 寻找音频流和视频流
		audioStreamIndex = -1;
		videoStreamIndex = -1;

		for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
			if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				audioStreamIndex = i;
			}
			else if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStreamIndex = i;
			}
		}

		// 检查是否找到音频流和视频流
		if (audioStreamIndex == -1 || videoStreamIndex == -1) {
			return -1;
		}

		// 获取音频解码器和视频解码器
		audioCodecParameters = formatContext->streams[audioStreamIndex]->codecpar;
		videoCodecParameters = formatContext->streams[videoStreamIndex]->codecpar;

		audioCodec = const_cast<AVCodec*>(avcodec_find_decoder(audioCodecParameters->codec_id));
		videoCodec = const_cast<AVCodec*>(avcodec_find_decoder(videoCodecParameters->codec_id));

		// 打开音频解码器和视频解码器
		audioCodecContext = avcodec_alloc_context3(audioCodec);
		videoCodecContext = avcodec_alloc_context3(videoCodec);

		if (!audioCodecContext || !videoCodecContext) {
			return -1;
		}

		if (avcodec_parameters_to_context(audioCodecContext, audioCodecParameters) < 0 ||
			avcodec_parameters_to_context(videoCodecContext, videoCodecParameters) < 0) {
			return -1;
		}

		if (avcodec_open2(audioCodecContext, audioCodec, NULL) < 0 ||
			avcodec_open2(videoCodecContext, videoCodec, NULL) < 0) {
			return -1;
		}

		// 初始化音频重采样上下文
		swrContext = swr_alloc_set_opts(NULL,
			av_get_default_channel_layout(audioCodecContext->channels),
			AV_SAMPLE_FMT_S16,
			audioCodecContext->sample_rate,
			av_get_default_channel_layout(audioCodecContext->channels),
			audioCodecContext->sample_fmt,
			audioCodecContext->sample_rate,
			0, NULL);
		if (!swrContext) {
			return -1;
		}
		if (swr_init(swrContext) < 0) {
			return -1;
		}
		/*
		// 打开音频设备
		SDL_AudioSpec desiredSpec, obtainedSpec;
		desiredSpec.freq = audioCodecContext->sample_rate;
		desiredSpec.format = AUDIO_S16SYS;
		desiredSpec.channels = audioCodecContext->channels;
		desiredSpec.silence = 0;
		desiredSpec.samples = 1024;
		desiredSpec.callback = audio_callback;
		desiredSpec.userdata = &audioParams;
		*/

		// 循环读取帧数据并播放

		frame = av_frame_alloc();
		convertedFrame = av_frame_alloc();

		//=======================================================================================================
		// 创建用于缩放和像素格式转换的SwsContext
		sws_ctx = sws_getContext(videoCodecContext->width, videoCodecContext->height, videoCodecContext->pix_fmt,
			videoCodecContext->width, videoCodecContext->height, AV_PIX_FMT_RGB24,
			SWS_BILINEAR, NULL, NULL, NULL);

		// 创建用于存储RGB数据的AVFrame
		rgb_frame = av_frame_alloc();
		rgb_frame->format = AV_PIX_FMT_RGB24;
		rgb_frame->width = videoCodecContext->width;
		rgb_frame->height = videoCodecContext->height;
		av_frame_get_buffer(rgb_frame, 32);

		bmpheader.bfType = 0x4d42; //'BM';
		bmpheader.bfReserved1 = 0;
		bmpheader.bfReserved2 = 0;
		bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
		bmpinfo.biPlanes = 1;
		bmpinfo.biBitCount = bpp;
		bmpinfo.biCompression = 0;
		bmpinfo.biSizeImage = 0;
		bmpinfo.biXPelsPerMeter = 100;
		bmpinfo.biYPelsPerMeter = 100;
		bmpinfo.biClrUsed = 0;
		bmpinfo.biClrImportant = 0;

		width = videoCodecContext->width;
		height = videoCodecContext->height;

		sound = make_shared<AudioPlayer>(2, audioCodecContext->sample_rate);

		return 1;
	}
	double Next() {
		auto start = std::chrono::system_clock::now();

		if (av_read_frame(formatContext, &packet) < 0)
			return -1;
		if (packet.stream_index == videoStreamIndex) {
			// 解码视频帧
			avcodec_send_packet(videoCodecContext, &packet);
			while (avcodec_receive_frame(videoCodecContext, frame) >= 0) {

				// 将解码后的YUV数据转换为RGB格式
				sws_scale(sws_ctx, frame->data, frame->linesize, 0, videoCodecContext->height,
					rgb_frame->data, rgb_frame->linesize);

				width = frame->width;
				height = frame->height;

				bmpheader.bfSize = bmpheader.bfOffBits + width * height * bpp / 8;
				bmpinfo.biWidth = width;
				bmpinfo.biHeight = -height; //reverse the image

				HRESULT hr;

				int nSize = width * height * 3; //BYTE*指向的数据的长度
				HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_NODISCARD, sizeof(bmpheader) + sizeof(BITMAPINFOHEADER) + nSize);
				IStream* iStream = NULL;
				hr = CreateStreamOnHGlobal(hMem, FALSE, &iStream);
				if (FAILED(hr)) { return -1; }

				hr = iStream->Write(&bmpheader, sizeof(bmpheader), NULL);
				hr = iStream->Write(&bmpinfo, sizeof(BITMAPINFOHEADER), NULL);
				hr = iStream->Write(rgb_frame->data[0], nSize, NULL);
				if (FAILED(hr)) { return -1; }

				video.Destroy();
				video.Load(iStream);

				iStream->Release();
				::GlobalUnlock(hMem);
				::GlobalFree(hMem);
			}
		}
		else if (packet.stream_index == audioStreamIndex) {
			// 解码音频帧
			avcodec_send_packet(audioCodecContext, &packet);
			while (avcodec_receive_frame(audioCodecContext, frame) >= 0) {

				// 音频重采样
				int dst_nb_samples = av_rescale_rnd(swr_get_delay(swrContext, frame->sample_rate) +
					frame->nb_samples,
					audioCodecContext->sample_rate,
					frame->sample_rate,
					AV_ROUND_UP);
				av_frame_make_writable(convertedFrame);
				convertedFrame->format = AV_SAMPLE_FMT_S16;
				convertedFrame->channel_layout = audioCodecContext->channel_layout;
				convertedFrame->sample_rate = audioCodecContext->sample_rate;
				convertedFrame->channels = audioCodecContext->channels;
				av_samples_alloc(convertedFrame->data, convertedFrame->linesize, audioCodecContext->channels,
					dst_nb_samples, AV_SAMPLE_FMT_S16, 0);
				swr_convert(swrContext, convertedFrame->data, dst_nb_samples,
					(const uint8_t**)frame->data, frame->nb_samples);

				if (convertedFrame->format == AV_SAMPLE_FMT_FLTP) {
					sound->WriteFLTP((float*)convertedFrame->data[0], (float*)convertedFrame->data[1], frame->nb_samples);
				}
				else if (convertedFrame->format == AV_SAMPLE_FMT_S16) {
					sound->WriteS16((short*)convertedFrame->data[0], frame->nb_samples);
				}

				av_freep(&convertedFrame->data[0]);
				av_freep(&convertedFrame->data[1]);
			}
		}
		av_packet_unref(&packet);
		return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count());
	}

	void Destroy() {
		// 释放资源
		av_frame_free(&frame);
		av_frame_free(&convertedFrame);
		avcodec_close(audioCodecContext);
		avcodec_close(videoCodecContext);
		avformat_close_input(&formatContext);
		avformat_free_context(formatContext);
		swr_free(&swrContext);

		sws_freeContext(sws_ctx);
		av_frame_free(&rgb_frame);

		CoUninitialize();

	}

};