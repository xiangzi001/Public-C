
/*
class Camera
{
public:
	HWND					m_hwnd;

	IGraphBuilder*			m_pGraph; // filter granph(manager)
	ICaptureGraphBuilder2*	m_pCapture; // capture granph
	IMediaControl*			m_pMediaC; // 媒体控制接口
	IMediaEventEx*			m_pMediaE; // 媒体事件接口
	IVideoWindow*			m_pVideoW; // 视频窗口接口
	IBaseFilter*			m_pFilter; // 基类filter
	IBasicVideo*			basevideo = NULL;

	HRESULT Init(HWND hwnd)
	{
		HRESULT hr;
		// 创建filter graph manager
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&m_pGraph);
		if (FAILED(hr))
			return hr;
		// 创建capture granph
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void**)&m_pCapture);
		if (FAILED(hr))
			return hr;
		// 查询graph中各IID参数标识的接口指针
		hr = m_pGraph->QueryInterface(IID_IMediaControl, (LPVOID*)&m_pMediaC);
		hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID*)&m_pMediaE);
		hr = m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID*)&m_pVideoW);

		hr = m_pCapture->SetFiltergraph(m_pGraph);

		if (FAILED(hr)) {
			MessageBox(m_hwnd, "Faild Init", "demo", MB_ICONEXCLAMATION | MB_OK);
			return hr;
		}
		// 将Win32窗口句柄赋给m_hwnd
		m_hwnd = hwnd;
		return hr;
	}
	HRESULT FindCaptureDevice(int camera)
	{
		HRESULT hr = S_OK;
		ICreateDevEnum* pDevEnum = NULL;
		IEnumMoniker* pClassEnum = NULL; // 用于视频采集设备的枚举
		IMoniker* pMoniker = NULL; // 设备Moniker号
		// 创建系统设备枚举
		hr = CoInitialize(NULL);
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, (LPUNKNOWN)CLSCTX_INPROC_SERVER, CLSCTX_INPROC, IID_ICreateDevEnum, (void**)&pDevEnum);
		//hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&m_pvideo));
		if (FAILED(hr)) {
			MessageBox(m_hwnd, _char(_str(hr)), "demo", MB_ICONEXCLAMATION | MB_OK);
			//return hr;
		}
		CoUninitialize();
		// 创建一个指定视频采集设备的枚举
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr) || pClassEnum == NULL)
		{
			MessageBox(m_hwnd, "Faild CreateClassEnumerator", "demo", MB_ICONEXCLAMATION | MB_OK);
			SAFE_RELEASE(pDevEnum);
			return hr;
		}
		// 使用第一个找到的视频采集设备（只适用于单摄像头的情况）
		for (int i = 0; i < camera; i++)
		{
			hr = pClassEnum->Next(1, &pMoniker, NULL);
		}
		if (hr == S_FALSE)
		{
			MessageBox(m_hwnd, "Faild FindCaptureDevice", "demo", MB_ICONEXCLAMATION | MB_OK);
			SAFE_RELEASE(pDevEnum);
			SAFE_RELEASE(pClassEnum);
			return hr;
		}
		// 绑定找到摄像头的moniker到filter graph
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pFilter);
		if (FAILED(hr))
		{
			MessageBox(m_hwnd, "Faild BindToObject", "demo", MB_ICONEXCLAMATION | MB_OK);
			SAFE_RELEASE(pDevEnum);
			SAFE_RELEASE(pClassEnum);
			SAFE_RELEASE(pMoniker);
			return hr;
		}
		// 增加filter graph的引用计数
		m_pFilter->AddRef();
		return hr;
	}
	HRESULT Render(LPCOLESTR file, RECT show)
	{
		HRESULT hr;

		hr = m_pGraph->AddFilter(m_pFilter, L"Video capture"); 
		//hr = m_pGraph->AddFilter(m_pvideo, L"pVideo");
		if (FAILED(hr))
		{
			MessageBox(m_hwnd, "Faild AddFilter", "demo", MB_ICONEXCLAMATION | MB_OK);
			m_pFilter->Release();
			return hr;
		}
		if (file != NULL) {
			IBaseFilter* pMux;
			hr = m_pCapture->SetOutputFileName(&MEDIASUBTYPE_Avi, file, &pMux, NULL); // 设置输出视频文件位置
			hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, m_pFilter, NULL, pMux); // 将m_pFilter的输出pin连接到pMux
			pMux->Release();
			m_pFilter->Release();
		}
		m_pVideoW->QueryInterface(IID_IBasicVideo, (void**)&basevideo);
		if (show.bottom != 0 or show.right != 0) {
			hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pFilter, NULL, NULL);
			if (FAILED(hr))
			{
				MessageBox(m_hwnd, "Faild RenderStream", "demo", MB_ICONEXCLAMATION | MB_OK);
				m_pFilter->Release();
				return hr;
			}
			hr = m_pVideoW->put_Owner((OAHWND)m_hwnd);
			hr = m_pVideoW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
			hr = m_pVideoW->put_Visible(OATRUE);
			hr = m_pVideoW->SetWindowPosition(show.left,show.top,show.right,show.bottom);
			if (FAILED(hr))
				return hr;
		}
		hr = m_pMediaC->Run();
		return hr;
	}
	void DestroyGraph()
	{
		if (m_pMediaC)
			m_pMediaC->StopWhenReady();
		if (m_pVideoW)
		{
//			m_pVideoW->Release();
			m_pVideoW->put_Visible(OAFALSE);
			m_pVideoW->put_Owner(NULL);
		}

		SAFE_RELEASE(m_pMediaC);
		SAFE_RELEASE(m_pMediaE);
		SAFE_RELEASE(m_pVideoW);
		SAFE_RELEASE(m_pGraph);
		SAFE_RELEASE(m_pCapture);
	}
	~Camera() {
		DestroyGraph();
	}
}cam;
*/

/*
#include <atlbase.h>
#include <dshow.h>
#pragma comment(lib,"Strmiids.lib")
*/
/*
#define SAFE_RELEASE(x) {	if (x) x->Release(); x = NULL;	}

class XCamera {
private:
	IGraphBuilder* pGraph = NULL;
	ICreateDevEnum* pSysDevEnum = NULL;
	IEnumMoniker* pEnumCat = NULL;
	IMoniker* pMoniker = NULL;
	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	//IBaseFilter* pvideo = NULL;
	ICaptureGraphBuilder2* pCapture;
	IVideoWindow* pvideo = NULL;
	IMediaControl* pControl;
	IPin* pOut = 0;
	IPin* pIn = 0;
	IBasicVideo* basevideo = NULL;
public:
	long bitmapsize;
	CImage image;

	XCamera() {
		HRESULT hr;
		hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); //Initializes COM
	}
	~XCamera() {
		CoUninitialize();
	}
	HRESULT start(HWND hwnd, RECT show) {
		HRESULT hr;
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSysDevEnum);
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void**)&pCapture);
		hr = pCapture->SetFiltergraph(pGraph);
		if (FAILED(hr)) {
			MessageBoxA(NULL, "Init Wrong", "demo", S_OK);
			//return hr;
		}
		hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
		hr = pEnumCat->Next(1, &pMoniker, &cFetched);
		hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
		hr = pGraph->AddFilter(pFilter, L"pFilter");
		hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
		hr = CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast<void**>(&pvideo));
		hr = pGraph->QueryInterface(IID_IVideoWindow, (LPVOID*)&pvideo);
		IEnumPins* pEnum = 0;
		hr = pFilter->EnumPins(&pEnum);
		hr = pEnum->Next(1, &pOut, NULL);
		pEnum->Release();
		pvideo->QueryInterface(IID_IBasicVideo, (void**)&basevideo);
		if (FAILED(hr)) {
			MessageBoxA(NULL, "Find Device Wrong", "demo", S_OK);
			//return hr;
		}
		hr = pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pFilter, NULL, NULL);
		hr = pvideo->put_Owner((OAHWND)hwnd);
		hr = pvideo->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
		hr = pvideo->put_Visible(OATRUE);
		hr = pvideo->SetWindowPosition(show.left, show.top, show.right, show.bottom);
		hr = pControl->Run();
		if (FAILED(hr)) {
			MessageBoxA(NULL, "Render Wrong", "demo", S_OK);
			//return hr;
		}
		return hr;
	}
	void destroy() {
		if (pControl)
			pControl->StopWhenReady();
		if (pvideo)
		{
			//			m_pVideoW->Release();
			pvideo->put_Visible(OAFALSE);
			pvideo->put_Owner(NULL);
		}

		SAFE_RELEASE(pControl);
		SAFE_RELEASE(pvideo);
		SAFE_RELEASE(pGraph);
		SAFE_RELEASE(pCapture);
		image.Destroy();
	}
	HRESULT initImage(){
		HRESULT hr;
		hr = basevideo->GetCurrentImage(&bitmapsize, 0);
		image.Destroy();
		image.Create(1280, 720, 32); //1280x720
		return hr;
	}
	HRESULT getImage(){
		HRESULT hr;
		hr = basevideo->GetCurrentImage(&bitmapsize, (long*)(image.GetBits()));
		//memcpy(image.GetBits(), buffer, image.GetHeight() * abs((image).GetPitch()));
		return hr;
	}
}camera;

*/

IPin* GetOutPin(IBaseFilter* pFilter, int PinNum)
{
	IEnumPins* pEnum = 0;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	pEnum->Reset();
	ULONG Fetched;
	do
	{
		Fetched = 0;
		IPin* pPin = 0;
		pEnum->Next(1, &pPin, &Fetched);
		if (Fetched)
		{
			PIN_DIRECTION pd;
			pPin->QueryDirection(&pd);
			pPin->Release();
			if (pd == PINDIR_OUTPUT)
			{
				if (PinNum == 0)
				{
					pEnum->Release();
					return pPin;
				}
				PinNum--;
			}
		}
	} while (Fetched);
	pEnum->Release();
	return NULL;
}

HRESULT findPin(IBaseFilter* pFilter, int dir, IPin** pOutPin)
{
	IEnumPins* pEnumPins = NULL;
	IPin* pPin = NULL;
	PIN_INFO pinInfo;
	//FILTER_INFO filterInfo;

	//HRESULT hr = pFilter->QueryFilterInfo(&filterInfo);
	//if(hr == S_OK)
	//{
	//	ctrace(L"%s Pins:\n", filterInfo.achName);
	//}

	HRESULT hr = pFilter->EnumPins(&pEnumPins);
	if (FAILED(hr)) return hr;


	while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
	{
		hr = pPin->QueryPinInfo(&pinInfo);
		if (FAILED(hr)) continue;

		if (pinInfo.dir == dir)
		{
			*pOutPin = pPin;
			//ctrace(L"[%s] %s\n", (dir == PINDIR_INPUT)? L"INPUT": L"OUTPUT", pinInfo.achName);
			return S_OK;
		}
	}
	pEnumPins->Release();

	return -1;
}

class XDshow {
public:
	/*void ___1() {
		IGraphBuilder* pGraphBuilder;
		IBaseFilter* pSourceFilter;
		IMediaControl* pMediaCtrl = NULL;
		IBaseFilter* pRenderFilter = NULL;

		pSourceFilter = NULL;

		// 创建视频捕捉过滤器实例
		HRESULT hr = CoCreateInstance(CLSID_VideoInputDeviceCategory, NULL,
			CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pSourceFilter);
		// 将该过滤器添加到DirectShow过滤器图形中
		hr = pGraphBuilder->AddFilter(pSourceFilter, L"Source Filter");
		// 创建IMediaControl接口实例
		hr = pGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&pMediaCtrl);
		// 创建Video Renderer Filter标准渲染器
		hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&pRenderFilter);
		// 将视频捕捉过滤器的输出端口连接到渲染器的输入端口
		hr = pGraphBuilder->ConnectDirect(GetOutPin(pSourceFilter,1), GetInPin(pRenderFilter), NULL);
		// 开始播放视频
		hr = pMediaCtrl->Run();

	}*/
	void Open(LPCWSTR file) {
		IGraphBuilder* pGraph = NULL;
		IMediaControl* pControl = NULL;
		IMediaEvent* pEvent = NULL;

		// Initialize the COM library.
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			printf("ERROR - Could not initialize COM library");
			return;
		}

		// Create the filter graph manager and query for interfaces.
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
			IID_IGraphBuilder, (void**)&pGraph);
		if (FAILED(hr))
		{
			printf("ERROR - Could not create the Filter Graph Manager.");
			return;
		}

		hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
		hr = pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);

		// Build the graph. IMPORTANT: Change this string to a file on your system.
		hr = pGraph->RenderFile(file, NULL);//视频用avi的测试
		if (SUCCEEDED(hr))
		{
			// Run the graph.
			hr = pControl->Run();
			if (SUCCEEDED(hr))
			{
				// Wait for completion.
				long evCode;
				pEvent->WaitForCompletion(INFINITE, &evCode);

				// Note: Do not use INFINITE in a real application, because it
				// can block indefinitely.
			}
		}
		pControl->Release();
		pEvent->Release();
		pGraph->Release();
		CoUninitialize();
	}
}Dshow;