#include <atlcomcli.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>

class AudioPlayer {
private:
	WORD nChannels;
	DWORD nSamplesPerSec;
	int maxSampleCount; // ��������С����������

	WAVEFORMATEX* pwfx;
	CComPtr<IMMDeviceEnumerator> pEnumerator;
	CComPtr<IMMDevice> pDevice;
	CComPtr<IAudioClient> pAudioClient;
	CComPtr<IAudioRenderClient> pRenderClient;
	CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;

	DWORD flags = 0;

	HRESULT Init() {
		constexpr auto REFTIMES_PER_SEC = 10000000; // 1s�Ļ�����

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

		// ���ǿ�����������Ƶ�豸��ͬ�Ĳ�����
		pwfx->nSamplesPerSec = nSamplesPerSec;
		// �̶�˫����
		pwfx->nAvgBytesPerSec = pwfx->nSamplesPerSec * 2 * (pwfx->wBitsPerSample / 8);
		// ����ʹ�����ָ�ʽ
		pwfx->wFormatTag = WAVE_FORMAT_EXTENSIBLE;

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY, // �����flag����ϵͳ��Ҫ�ز���
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

	// FLTP ��ʽ���������ֿ������ǰ����Ǻϲ���һ�𣬡������������ҡ�����
	HRESULT WriteFLTP(float* left, float* right, UINT32 sampleCount) {
		UINT32 padding;
		pAudioClient->GetCurrentPadding(&padding);
		if ((maxSampleCount - padding) < sampleCount) { // ��Ƶд��̫���ˣ�����������������ֱ��������л���������֤ʱ��Ե���
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
		if ((maxSampleCount - padding) < sampleCount) { // ��Ƶд��̫���ˣ�����������������ֱ��������л���������֤ʱ��Ե���
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


	// �������Ҳ�������ֻ����������������Ȼ᲻����
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

	// ��������
	HRESULT SetVolume(float v) {
		return pSimpleAudioVolume->SetMasterVolume(v, NULL);
	}

};
/*
void play_sound() {
	HWAVEIN         hWaveIn;		        //�����豸
	HWAVEOUT        hWaveOut;		        //����豸
	WAVEFORMATEX    waveform;	            //������Ƶ����ʽ
	BYTE* pBuffer1, * pBuffer2;				//������Ƶ������������������
	WAVEHDR         whdr_i1, whdr_i2;       //������Ƶͷ
	WAVEHDR         whdr_o;                //�����Ƶͷ

	// �豸����
	int count = waveInGetNumDevs();

	// �豸����
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2

	// ������Ƶ����ʽ
	waveform.nSamplesPerSec = 44100;												// ������
	waveform.wBitsPerSample = 16;												// ��������
	waveform.nChannels = 2;                                                     // ��������
	waveform.cbSize = 0;														// ����ռ�	
	waveform.wFormatTag = WAVE_FORMAT_PCM;										// ��Ƶ��ʽ
	waveform.nBlockAlign = (waveform.wBitsPerSample * waveform.nChannels) / 8;  // �����
	waveform.nAvgBytesPerSec = waveform.nBlockAlign * waveform.nSamplesPerSec;  // ��������

	//�����ڴ�
	pBuffer1 = new BYTE[1024 * 10000];
	pBuffer2 = new BYTE[1024 * 10000];
	memset(pBuffer1, 0, 1024 * 10000);   // �ڴ���0
	memset(pBuffer2, 0, 1024 * 10000);   // �ڴ���0

	// ������Ƶͷ
	whdr_i1.lpData = (LPSTR)pBuffer1; // ָ��buffer
	whdr_i1.dwBufferLength = 1024 * 10000;     // buffer��С
	whdr_i1.dwBytesRecorded = 0;      // buffer��Ŵ�С
	whdr_i1.dwUser = 0;
	whdr_i1.dwFlags = 0;
	whdr_i1.dwLoops = 1;
	whdr_i2.lpData = (LPSTR)pBuffer1; // ָ��buffer
	whdr_i2.dwBufferLength = 1024 * 10000;     // buffer��С
	whdr_i2.dwBytesRecorded = 0;      // buffer��Ŵ�С
	whdr_i2.dwUser = 0;
	whdr_i2.dwFlags = 0;
	whdr_i2.dwLoops = 1;

	// ����¼��
	MMRESULT mRet = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)callback, (DWORD_PTR)user, CALLBACK_FUNCTION);
	waveInPrepareHeader(hWaveIn, &whdr_i1, sizeof(WAVEHDR));//׼��buffer
	waveInPrepareHeader(hWaveIn, &whdr_i2, sizeof(WAVEHDR));//׼��buffer
	waveInAddBuffer(hWaveIn, &whdr_i1, sizeof(WAVEHDR));    //���buffer
	waveInAddBuffer(hWaveIn, &whdr_i2, sizeof(WAVEHDR));    //���buffer

	waveInStart(hWaveIn);
	getchar();
	recurr = FALSE;
	//waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);

	HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	// ����¼��
	whdr_o.lpData = (LPSTR)file;			// ָ��buffer
	whdr_o.dwBufferLength = hasRecorded;    // buffer��С
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