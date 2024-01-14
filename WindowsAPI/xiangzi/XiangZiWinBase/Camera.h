
#define CAMERA_START 10
#define CAMERA_STOP 11

void _SendMessage(void* obj, int msg);
void MainMessage(void* msg);

void getCameraSupportResolutions(IBaseFilter* pBaseFilter, int& w, int& h)
{
	HRESULT hr = 0;
	std::vector<IPin*> pins;
	IEnumPins* EnumPins;
	pBaseFilter->EnumPins(&EnumPins);
	pins.clear();

	for (;;)
	{
		IPin* pin;
		hr = EnumPins->Next(1, &pin, NULL);
		if (hr != S_OK)
		{
			break;
		}
		pins.push_back(pin);
		pin->Release();
	}

	EnumPins->Release();

	PIN_INFO pInfo;
	for (int i = 0; i < pins.size(); i++)
	{
		if (nullptr == pins[i])
		{
			break;
		}
		pins[i]->QueryPinInfo(&pInfo);

		IEnumMediaTypes* emt = NULL;
		pins[i]->EnumMediaTypes(&emt);
		AM_MEDIA_TYPE* pmt;

		for (;;)
		{
			hr = emt->Next(1, &pmt, NULL);
			if (hr != S_OK)
			{
				break;
			}
			if ((pmt->formattype == FORMAT_VideoInfo)
				//&& (pmt->subtype == MEDIASUBTYPE_RGB24)
				&& (pmt->cbFormat >= sizeof(VIDEOINFOHEADER))
				&& (pmt->pbFormat != NULL)) {

				VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
				w = pVIH->bmiHeader.biWidth;
				h = pVIH->bmiHeader.biHeight;
			}

			if (pmt->cbFormat != 0)
			{
				CoTaskMemFree((PVOID)pmt->pbFormat);
				pmt->cbFormat = 0;
				pmt->pbFormat = NULL;
			}
			if (pmt->pUnk != NULL)
			{
				// pUnk should not be used.
				pmt->pUnk->Release();
				pmt->pUnk = NULL;
			}
		}
		break;
		emt->Release();
	}
}

#define SAFE_RELEASE(x) {	if (x) x->Release(); x = NULL;	}

class XCamera {
private:
	HWND hwnd;
	RECT show;

	IGraphBuilder* pGraph = NULL;
	ICreateDevEnum* pSysDevEnum = NULL;
	IEnumMoniker* pEnumCat = NULL;
	IMoniker* pMoniker = NULL;
	ULONG cFetched;
	IBaseFilter* pFilter = NULL;
	ICaptureGraphBuilder2* pCapture;
	IVideoWindow* pvideo = NULL;
	IMediaControl* pControl;
	IPin* pOut = 0;
	IPin* pIn = 0;
	IBasicVideo* basevideo = NULL;

	long nSize;
public:
	string cameraName;
	int h;
	int w;
	CImage image;

	XCamera() {
		HRESULT hr;
		hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); //Initializes COM
	}
	~XCamera() {
		CoUninitialize();
	}
	void start(HWND _hwnd, RECT _show) {
		hwnd = _hwnd;
		show = _show;
		//MainMessage(_start);
		//_SendMessage(CAMERA_STOP);
		_SendMessage(&(*this),CAMERA_START);
	}
	HRESULT _Start() {
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

		ULONG cFetched;
		auto index = 0;
		IPropertyBag* pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		if (SUCCEEDED(hr))
		{
			getCameraSupportResolutions(pFilter, w, h);
			cameraName = _com_util::ConvertBSTRToString(varName.bstrVal);
		}
		VariantClear(&varName);
		pPropBag->Release();

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
	HRESULT _Start(HWND _hwnd, RECT _show) {
		hwnd = _hwnd;
		show = _show;
		return _Start();
	}
	void stop() {
		//MainMessage(_stop);
		_SendMessage(&(*this), CAMERA_STOP);
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
	void changePosition(RECT show) {
		if (pvideo)
			pvideo->SetWindowPosition(show.left, show.top, show.right, show.bottom);
	}
	HRESULT initImage() {
		HRESULT hr;
		hr = basevideo->GetCurrentImage(&nSize, 0);
		image.Destroy();
		image.Create(w, h, 32); //1280x720
		return hr;
	}
	HRESULT getImage() {
		HRESULT hr;
		hr = basevideo->GetCurrentImage(&nSize, (long*)(image.GetBits()) );
		//memcpy(image.GetBits(), buffer, image.GetHeight() * abs((image).GetPitch()));
		return hr;
	}
};

