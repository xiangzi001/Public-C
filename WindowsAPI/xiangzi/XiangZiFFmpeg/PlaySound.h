#include <atlcomcli.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>

class AudioPlayer {
private:
	WORD nChannels;
	DWORD nSamplesPerSec;
	int maxSampleCount; // 缓冲区大小（样本数）

	WAVEFORMATEX* pwfx;
	CComPtr<IMMDeviceEnumerator> pEnumerator;
	CComPtr<IMMDevice> pDevice;
	CComPtr<IAudioClient> pAudioClient;
	CComPtr<IAudioRenderClient> pRenderClient;
	CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;

	DWORD flags = 0;

	HRESULT Init() {
		constexpr auto REFTIMES_PER_SEC = 10000000; // 1s的缓冲区

		HRESULT hr;

		hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDevice);

		hr = pDevice->Activate(
			__uuidof(IAudioClient), CLSCTX_ALL,
			NULL, (void**)&pAudioClient);

		CComPtr<IAudioSessionManager> pAudioSessionManager;
		hr = pDevice->Activate(
			__uuidof(IAudioSessionManager), CLSCTX_INPROC_SERVER,
			NULL, (void**)&pAudioSessionManager
		);

		CComPtr<IAudioSessionControl> pAudioSession;
		hr = pAudioSessionManager->GetAudioSessionControl(
			&GUID_NULL,
			FALSE,
			&pAudioSession
		);

		hr = pAudioSessionManager->GetSimpleAudioVolume(
			&GUID_NULL,
			0,
			&pSimpleAudioVolume
		);

		hr = pAudioClient->GetMixFormat(&pwfx);

		// 我们可以设置与音频设备不同的采样率
		pwfx->nSamplesPerSec = nSamplesPerSec;
		// 固定双声道
		pwfx->nAvgBytesPerSec = pwfx->nSamplesPerSec * 2 * (pwfx->wBitsPerSample / 8);
		// 必须使用这种格式
		pwfx->wFormatTag = WAVE_FORMAT_EXTENSIBLE;

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, // 这里的flag告诉系统需要重采样
			REFTIMES_PER_SEC,
			0,
			pwfx,
			NULL);

		hr = pAudioClient->GetService(
			__uuidof(IAudioRenderClient),
			(void**)&pRenderClient);

		maxSampleCount = pwfx->nSamplesPerSec;

		return hr;
	}

public:

	AudioPlayer(WORD nChannels_, DWORD nSamplesPerSec_)
		: nChannels(nChannels_), nSamplesPerSec(nSamplesPerSec_), pwfx(nullptr), flags(0)
	{
		Init();
	}
	

	HRESULT Start() {
		return pAudioClient->Start();
	}
	HRESULT Stop() {
		return pAudioClient->Stop();
	}

	BYTE* GetBuffer(UINT32 wantFrames) {
		BYTE* buffer;
		pRenderClient->GetBuffer(wantFrames, &buffer);
		return buffer;
	}

	HRESULT ReleaseBuffer(UINT32 writtenFrames) {
		return pRenderClient->ReleaseBuffer(writtenFrames, flags);
	}

	// FLTP 格式左右声道分开，我们把他们合并到一起，“左右左右左右”这样
	HRESULT WriteFLTP(float* left, float* right, UINT32 sampleCount) {
		UINT32 padding;
		pAudioClient->GetCurrentPadding(&padding);
		if ((maxSampleCount - padding) < sampleCount) { // 音频写入太快了，超出缓冲区，我们直接清空现有缓冲区，保证时间对的上
			pAudioClient->Stop();
			pAudioClient->Reset();
			pAudioClient->Start();
		}

		if (left && right) {
			auto pData = GetBuffer(sampleCount);
			for (int i = 0; i < sampleCount; i++) {
				int p = i * 2;
				((float*)pData)[p] = left[i];
				((float*)pData)[p + 1] = right[i];
			}
		}
		else if (left) {
			auto pData = GetBuffer(sampleCount);
			for (int i = 0; i < sampleCount; i++) {
				int p = i * 2;
				((float*)pData)[p] = left[i];
				((float*)pData)[p + 1] = left[i];
			}
		}


		return ReleaseBuffer(sampleCount);
	}

	HRESULT WriteS16(short* data, UINT32 sampleCount)
	{
		UINT32 padding;
		pAudioClient->GetCurrentPadding(&padding);
		if ((maxSampleCount - padding) < sampleCount) { // 音频写入太快了，超出缓冲区，我们直接清空现有缓冲区，保证时间对的上
			pAudioClient->Stop();
			pAudioClient->Reset();
			pAudioClient->Start();
		}

		if (data) {
			auto pData = GetBuffer(sampleCount);
			for (size_t i = 0; i < sampleCount * this->nChannels; i++) {
				float s = (float)data[i];
				if (s > 0) {
					s = s / SHRT_MAX;
				}
				else if (s < 0) {
					s = -s / SHRT_MIN;
				}
				((float*)pData)[i] = s;
			}
			return ReleaseBuffer(sampleCount);
		}

		return -1;
	}


	// 播放正弦波，仅仅只是用来测试你的喇叭会不会响
	HRESULT PlaySinWave(int nb_samples) {
		auto m_time = 0.0;
		auto m_deltaTime = 1.0 / nb_samples;

		auto pData = GetBuffer(nb_samples);

		for (int sample = 0; sample < nb_samples; ++sample) {
			float value = 0.05 * std::sin(5000 * m_time);
			int p = sample * nChannels;
			((float*)pData)[p] = value;
			((float*)pData)[p + 1] = value;
			m_time += m_deltaTime;
		}

		return ReleaseBuffer(nb_samples);
	}

	// 设置音量
	HRESULT SetVolume(float v) {
		return pSimpleAudioVolume->SetMasterVolume(v, NULL);
	}

};
/*
void play_sound() {
	HWAVEIN         hWaveIn;		        //输入设备
	HWAVEOUT        hWaveOut;		        //输出设备
	WAVEFORMATEX    waveform;	            //定义音频流格式
	BYTE* pBuffer1, * pBuffer2;				//输入音频缓冲区（左右声道）
	WAVEHDR         whdr_i1, whdr_i2;       //输入音频头
	WAVEHDR         whdr_o;                //输出音频头

	// 设备数量
	int count = waveInGetNumDevs();

	// 设备名称
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2

	// 设置音频流格式
	waveform.nSamplesPerSec = 44100;												// 采样率
	waveform.wBitsPerSample = 16;												// 采样精度
	waveform.nChannels = 2;                                                     // 声道个数
	waveform.cbSize = 0;														// 额外空间	
	waveform.wFormatTag = WAVE_FORMAT_PCM;										// 音频格式
	waveform.nBlockAlign = (waveform.wBitsPerSample * waveform.nChannels) / 8;  // 块对齐
	waveform.nAvgBytesPerSec = waveform.nBlockAlign * waveform.nSamplesPerSec;  // 传输速率

	//分配内存
	pBuffer1 = new BYTE[1024 * 10000];
	pBuffer2 = new BYTE[1024 * 10000];
	memset(pBuffer1, 0, 1024 * 10000);   // 内存置0
	memset(pBuffer2, 0, 1024 * 10000);   // 内存置0

	// 设置音频头
	whdr_i1.lpData = (LPSTR)pBuffer1; // 指向buffer
	whdr_i1.dwBufferLength = 1024 * 10000;     // buffer大小
	whdr_i1.dwBytesRecorded = 0;      // buffer存放大小
	whdr_i1.dwUser = 0;
	whdr_i1.dwFlags = 0;
	whdr_i1.dwLoops = 1;
	whdr_i2.lpData = (LPSTR)pBuffer1; // 指向buffer
	whdr_i2.dwBufferLength = 1024 * 10000;     // buffer大小
	whdr_i2.dwBytesRecorded = 0;      // buffer存放大小
	whdr_i2.dwUser = 0;
	whdr_i2.dwFlags = 0;
	whdr_i2.dwLoops = 1;

	// 开启录音
	MMRESULT mRet = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)callback, (DWORD_PTR)user, CALLBACK_FUNCTION);
	waveInPrepareHeader(hWaveIn, &whdr_i1, sizeof(WAVEHDR));//准备buffer
	waveInPrepareHeader(hWaveIn, &whdr_i2, sizeof(WAVEHDR));//准备buffer
	waveInAddBuffer(hWaveIn, &whdr_i1, sizeof(WAVEHDR));    //添加buffer
	waveInAddBuffer(hWaveIn, &whdr_i2, sizeof(WAVEHDR));    //添加buffer

	waveInStart(hWaveIn);
	getchar();
	recurr = FALSE;
	//waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);

	HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	// 播放录音
	whdr_o.lpData = (LPSTR)file;			// 指向buffer
	whdr_o.dwBufferLength = hasRecorded;    // buffer大小
	whdr_o.dwBytesRecorded = hasRecorded;
	whdr_o.dwFlags = 0;
	whdr_o.dwLoops = 1;


	ResetEvent(wait);
	waveOutPrepareHeader(hWaveOut, &whdr_o, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &whdr_o, sizeof(WAVEHDR));

	DWORD dw = WaitForSingleObject(wait, INFINITE);
	if (dw == WAIT_OBJECT_0)
	{
		std::cout << "jieshu" << std::endl;
		return 0;
	}

}*/