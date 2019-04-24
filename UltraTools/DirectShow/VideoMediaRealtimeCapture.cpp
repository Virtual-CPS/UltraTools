//////////////////////////////////////////////////////////////////////
//
// 本类利用DirectShow实现视频同步捕捉与显示的基本接口
//
// VideoMediaRealtimeCapture.cpp: implementation of the CVideoMediaRealtimeCapture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoMediaRealtimeCapture.h"
#include <ks.h>
#include <initguid.h>
#include "xprtdefs.h"
#include "FlirRtspInterface.h"
#include "SampleGrabberCbInterface.h"

typedef enum {
    KSPROPERTY_FLIRDCAM_EXT1_EXT1,
    KSPROPERTY_FLIRDCAM_EXT1_INFO,
    KSPROPERTY_FLIRDCAM_EXT1_SERIAL
} KSPROPERTY_FLIRDCAM_EXT1;

typedef struct {
    ULONG Length;
	BYTE DataBuffer[256];
} KSPROPERTY_FLIRDCAM_EXT1_S, *PKSPROPERTY_FLIRDCAM_EXT1_S;

#define STATIC_PROPSETID_FLIRDCAM_EXT1\
    0xffc75aedL, 0x46d5, 0x44e6, 0x86, 0x3c, 0x3b, 0x6c, 0x6c, 0x49, 0x09, 0xe9
DEFINE_GUIDSTRUCT("FFC75AED-46D5-44e6-863C-3B6C6C4909E9", PROPSETID_FLIRDCAM_EXT1);
#define PROPSETID_FLIRDCAM_EXT1 DEFINE_GUIDNAMED(PROPSETID_FLIRDCAM_EXT1)
//---------------------------
// H264
//
#define FOURCC_H264          mmioFOURCC('H','2','6','4') // 7634706d
#define KS_MEDIASUBTYPE_H264 FOURCC_H264, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("34363248-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_H264);
#define GUID_MEDIASUBTYPE_H264 DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_H264)
//---------------------------
// MP4V
//
#define FOURCC_MP4V          mmioFOURCC('m','p','4','v') // 7634706d
#define KS_MEDIASUBTYPE_MP4V FOURCC_MP4V, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("7634706d-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_MP4V);
#define GUID_MEDIASUBTYPE_MP4V DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_MP4V)
//---------------------------
// Researcher format F7M2
//
#define FOURCC_F7M2          mmioFOURCC('F','7','M','2')  // 324d3746
#define KS_MEDIASUBTYPE_F7M2 FOURCC_F7M2, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("324d3746-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_F7M2);
#define GUID_MEDIASUBTYPE_F7M2 DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_F7M2)
//---------------------------
// Researcher format F7M0
//
#define FOURCC_F7M0          mmioFOURCC('F','7','M','0')  // 304d3746
#define KS_MEDIASUBTYPE_F7M0 FOURCC_F7M0, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("304d3746-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_F7M0);
#define GUID_MEDIASUBTYPE_F7M0 DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_F7M0)
//---------------------------
// Y800 8-bit mono
//
#define FOURCC_Y800          mmioFOURCC('Y','8','0','0')  // 30303859
#define KS_MEDIASUBTYPE_Y800 FOURCC_Y800, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("30303859-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_Y800);
#define GUID_MEDIASUBTYPE_Y800 DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_Y800)
//-------------------------------------------------
// RAW format Y160 (monochrome Y 16-bit per pixel)
//
#define FOURCC_Y160          mmioFOURCC('Y','1','6','0')  // 30363159
#define KS_MEDIASUBTYPE_Y160 FOURCC_Y160, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71
DEFINE_GUIDSTRUCT("30363159-0000-0010-8000-00aa00389b71", GUID_MEDIASUBTYPE_Y160);
#define GUID_MEDIASUBTYPE_Y160 DEFINE_GUIDNAMED(GUID_MEDIASUBTYPE_Y160)

// {04FE9017-F873-410E-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW_DECODER,
0x04FE9017, 0xF873, 0x410E, 0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7);

// {33D9A760-90C8-11D0-BD43-00A0C911CE86}
DEFINE_GUID(CLSID_FFDSHOW_CODEC,
0x33D9A760, 0x90C8, 0x11D0, 0xBD, 0x43, 0x00, 0xA0, 0xC9, 0x11, 0xCE, 0x86);

#define ABS(x) (((x) > 0) ? (x) : -(x))
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

////////////////////////////////////////////////////////
// Sample grabber callback
//
// Note: this object is a SEMI-COM object, and can only be created statically.
// We use this little semi-com object to handle the sample-grab-callback,
// since the callback must provide a COM interface. We could have had an interface
// where you provided a function-call callback, but that's really messy, so we
// did it this way. You can put anything you want into this C++ object, even
// a pointer to a CDialog. Be aware of multi-thread issues though.
//
class CGrabCB: public ISampleGrabberCB
{
public:
   // Constructor
   //
   CGrabCB(CVideoMediaRealtimeCapture* pCap) {	m_pCap = pCap; }

   ~CGrabCB() {}

private:
	CVideoMediaRealtimeCapture* m_pCap;

public:
   // fake out any COM ref counting
   //
   STDMETHODIMP_(ULONG) AddRef() { return 2; }
   STDMETHODIMP_(ULONG) Release() { return 1; }

    // fake out any COM QI'ing
    //
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
    {
        if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) 
        {
            *ppv = (void *) static_cast<ISampleGrabberCB *>(this);
            return NOERROR;
        }    
        return E_NOINTERFACE;
    }

   //DECLARE_IUNKNOWN;

   // ISampleGrabberCB methods
   STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample)
   {
      pSample->AddRef();
      m_pCap->SampleCB(pSample);
      pSample->Release();
      return S_OK;
   }

   STDMETHODIMP BufferCB(double SampleTime, BYTE* pBuffer, long BufferLen)
   {
      return E_NOTIMPL;
   }

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVideoMediaRealtimeCapture::CVideoMediaRealtimeCapture()
{
	CoInitialize(NULL);
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEventEx = NULL;			
	m_pDownstreamFilter = NULL;
	m_pDecoder = NULL;
	m_pVideoRenderer = NULL;
    m_pSampleGrabber = NULL;
	m_pGrabCB = NULL;
	m_pBaseFilterMux = NULL;
    m_pFileSinkFilter = NULL;
    m_pCaptureGraphBuilder2 = NULL;
	m_pCamOutPin = NULL;
	m_pBaseFilterST = NULL;
	m_pFrame = NULL;
	m_nFrameLen = 0;
	m_pSampleCbFrame = NULL;
	m_nSampleCbFrameLen = 0;
	m_eCurPlayState = PLAY_STATE_NOT_INIT;
	m_nCameraType = FCAM_BTH;				// Default to ThermoVision camera
	m_nCurrentStreamFormat = 0;				// index of current format
	m_PixelValue = 0;
}

CVideoMediaRealtimeCapture::~CVideoMediaRealtimeCapture()
{
	CloseInterfaces();
	CoUninitialize( );
}

// 在启动视频之前调用
void CVideoMediaRealtimeCapture::PresetCamera(HWND hWnd, CString& sIpAddress)
{
	ASSERT(hWnd);

	m_hWnd = hWnd;
	m_sIpAddress = sIpAddress;
}

void CVideoMediaRealtimeCapture::GetCaps()
{
	HRESULT hr;
	IAMStreamConfig* pConfig = NULL;
    AM_MEDIA_TYPE* pMediaType = NULL;

	// Get stream config interface (used for device control)
    hr = m_pCaptureGraphBuilder2->FindInterface(
                &PIN_CATEGORY_CAPTURE,
                0,
                m_pDownstreamFilter, 
                IID_IAMStreamConfig, 
                reinterpret_cast<PVOID*>(&pConfig));
	if (FAILED(hr))
		return;

	int iCount = 0, iSize = 0, iFormat;
	hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

    hr = pConfig->GetFormat( &pMediaType ); // Get current format

	// Check the size to make sure we pass in the correct structure.
	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		VIDEO_STREAM_CONFIG_CAPS scc;

		// Use the video capabilities structure.

		for (iFormat=0; iFormat<iCount; iFormat++)
		{
			AM_MEDIA_TYPE* pMediaTypeTemp;
			hr = pConfig->GetStreamCaps(iFormat, &pMediaTypeTemp, (BYTE*)&scc);
			LONGLONG minr, maxr;

			if (SUCCEEDED(hr))
			{
				minr = scc.MinFrameInterval;
				maxr = scc.MaxFrameInterval;

				if (pMediaTypeTemp->subtype == pMediaType->subtype)
				{
					float f;
					float frmax = (float)(10000000.0 / minr);
					float frmin = (float)(10000000.0 / maxr);
					// Remember min and max frame rate
					m_fMaxFrameRate = frmax;
					m_fMinFrameRate = frmin;
					TRACE("Max frame rate: %.2f Hz\n", frmax);
					TRACE("Min frame rate: %.2f Hz\n", frmin);

					m_arrFrameRates.RemoveAll();
					int i = 0;
					f = frmax;
					m_arrFrameRates.Add(f);
					f = f / 2.0f;
					i++;

					while (f >= frmin)
					{
						if (f >= 1)
						{
							f = (float)((int)(f + 0.5));
						}
						else
						{
							int tmp = (int)(f * 10.0f);
							f = (float)tmp / 10.0f;
						}

						m_arrFrameRates.Add(f);
						f = f / 2.0f;
						i++;
					}
				}

				// 删除已处理MediaType
		        CoTaskMemFree(pMediaTypeTemp);
			}
		}
	}

	DeleteMediaType( pMediaType );
	pConfig->Release();
}

double CVideoMediaRealtimeCapture::GetFrameRate()
{
	double frameRate = 60.0f;
	HRESULT hr;

	if (!m_pCaptureGraphBuilder2)
	{
        TRACE("No graph builder created\n");
		return frameRate;
	}

	AM_MEDIA_TYPE* pMediaType = NULL;
	IAMStreamConfig* pVStreamConfig = NULL;

    hr = m_pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE,
                                   &MEDIATYPE_Video,
                                   m_pDownstreamFilter, IID_IAMStreamConfig, (void **)&pVStreamConfig);
	if  (FAILED( hr )) 
	{
        TRACE("GetFrameRate: Did not get stream config interface (%s)\n", GetError(hr));
        return frameRate;
    }

	hr = pVStreamConfig->GetFormat(&pMediaType);

    if (SUCCEEDED ( hr ))
    {
		if (pMediaType->formattype == FORMAT_VideoInfo)
        {
			VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER *)pMediaType->pbFormat;
			if (pvi->AvgTimePerFrame > 0) 
			{
				frameRate = 10000000.0 / pvi->AvgTimePerFrame;
				TRACE("Frame rate: (%d)\n", (int)frameRate);
			}
		}

		// Delete the media type
		CoTaskMemFree(pMediaType->pbFormat);
		CoTaskMemFree(pMediaType);
	}

	SAFE_RELEASE( pVStreamConfig );
	return frameRate;
}

// You need to set the proper media type before calling this function!
HRESULT CVideoMediaRealtimeCapture::SetFrameRate(double dFrameRate)
{
    HRESULT hr = S_OK;

	// Sanity check
	if (dFrameRate >= m_fMaxFrameRate) dFrameRate = m_fMaxFrameRate;
	if (dFrameRate <= m_fMinFrameRate) dFrameRate = m_fMinFrameRate;

    AM_MEDIA_TYPE* pMediaType = NULL;
	IAMStreamConfig* pVStreamConfig = NULL;

    hr = m_pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE,
                                   &MEDIATYPE_Video,
                                   m_pDownstreamFilter, IID_IAMStreamConfig, (void **)&pVStreamConfig);
	if  (FAILED( hr ))
	{
        TRACE("SetFrameRate: 未获取到流配置接口 (%s)\n", GetError(hr));
        return hr;
    }

    hr = pVStreamConfig->GetFormat( &pMediaType );
    if (SUCCEEDED(hr))
    {
		if (pMediaType->formattype == FORMAT_VideoInfo) 
		{
			VIDEOINFOHEADER* pvi = (VIDEOINFOHEADER *)pMediaType->pbFormat;
			pvi->AvgTimePerFrame = (REFERENCE_TIME)((1.0 / dFrameRate) * 10000000.0);
			hr = pVStreamConfig->SetFormat( pMediaType );
			if  (FAILED( hr )) 
				TRACE("SetFrameRate: 设置帧频失败 (%s)\n", GetError(hr));
		}
		DeleteMediaType( pMediaType );
    }

	SAFE_RELEASE( pVStreamConfig );
	return hr;
}

HRESULT CVideoMediaRealtimeCapture::EnumStreamFormats(int fmt)
{
	HRESULT hr;
	CString strFmt;
	IAMStreamConfig* pVStreamConfig = NULL;      // for video cap
    hr = m_pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE,
		                           &MEDIATYPE_Video,
								   m_pDownstreamFilter,
								   IID_IAMStreamConfig, 
								   (void **)&pVStreamConfig);
    if (pVStreamConfig && SUCCEEDED(hr))
	{
		// 若可能，将FLIR的属性格式作为缺省格式
		int iCount = 0, iSize = 0;
		hr = pVStreamConfig->GetNumberOfCapabilities(&iCount, &iSize);

		m_saStreamFormats.RemoveAll();
		// Check the size to make sure we pass in the correct structure.
		if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
		{
			// Use the video capabilities structure.
			m_nCurrentStreamFormat = 0; // index of current format
			for (int iFormat = 0; iFormat < iCount; iFormat++)
			{
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE* pMediaType;
				hr = pVStreamConfig->GetStreamCaps(iFormat, &pMediaType, (BYTE*)&scc);
				if (SUCCEEDED(hr))
				{
					// Examine the formats, and possibly use it.
					if (pMediaType->subtype == GUID_MEDIASUBTYPE_F7M2)
					{
						m_saStreamFormats.InsertAt(iFormat, _T("仅红外[640×480]"));
					}
					else if (pMediaType->subtype == GUID_MEDIASUBTYPE_F7M0) 
					{
						strFmt = _T("仅红外");
						m_nCameraType = FCAM_BTH;
						if (pMediaType->formattype == FORMAT_VideoInfo)
						{
							// Check the format size.
							if (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
							{
								VIDEOINFOHEADER* pVih = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
								// Access VIDEOINFOHEADER members through pVih.
								strFmt.Format(_T("仅红外[%d×%d]"), pVih->bmiHeader.biWidth, pVih->bmiHeader.biHeight);
							}
						}
						m_saStreamFormats.InsertAt(iFormat, strFmt);
					}
					else if (pMediaType->subtype == GUID_MEDIASUBTYPE_Y160)
					{
						strFmt = _T("原始红外");
						m_nCameraType = FCAM_BTH;
						if (pMediaType->formattype == FORMAT_VideoInfo)
						{
							// Check the format size.
							if (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
							{
								VIDEOINFOHEADER* pVih = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
								// Access VIDEOINFOHEADER members through pVih.
								strFmt.Format(_T("原始红外[%d×%d]"), pVih->bmiHeader.biWidth, pVih->bmiHeader.biHeight);
							}
						}
						m_saStreamFormats.InsertAt(iFormat, strFmt);
					}
					else if (pMediaType->subtype == GUID_MEDIASUBTYPE_Y800)
					{
						m_saStreamFormats.InsertAt(iFormat, _T("黑白视频[320×240]"));
					}
					else if (pMediaType->subtype == MEDIASUBTYPE_UYVY)
					{
						if (fmt < 0)
						{
							pVStreamConfig->SetFormat(pMediaType);
							m_nCurrentStreamFormat = iFormat; // index of current format
						}
						m_saStreamFormats.InsertAt(iFormat, _T("视频[640×480]"));
					}
					else if (pMediaType->subtype == MEDIASUBTYPE_RGB8)
					{
						m_saStreamFormats.InsertAt(iFormat, _T("RGB8[160×120]"));
					}
					else if (pMediaType->subtype == GUID_MEDIASUBTYPE_MP4V) 
					{
						strFmt = _T("MPEG4");
						m_nCameraType = FCAM_BTH;
						if (pMediaType->formattype == FORMAT_VideoInfo)
						{
							// Check the format size.
							if (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
							{
								VIDEOINFOHEADER* pVih = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
								// Access VIDEOINFOHEADER members through pVih.
								strFmt.Format(_T("MPEG4[%d×%d]"), pVih->bmiHeader.biWidth, pVih->bmiHeader.biHeight);
							}
						}
						m_saStreamFormats.InsertAt(iFormat, strFmt);
					}
					else if (pMediaType->subtype == GUID_MEDIASUBTYPE_H264) 
					{
						CString strFmt = _T("H.264");
						m_nCameraType = FCAM_BTH; // Balthazar camera
						if (pMediaType->formattype == FORMAT_VideoInfo)
						{
							// Check the format size.
							if (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER))
							{
								VIDEOINFOHEADER* pVih = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
								// Access VIDEOINFOHEADER members through pVih.
								strFmt.Format(_T("H.264[%d×%d]"), pVih->bmiHeader.biWidth, pVih->bmiHeader.biHeight);
							}
						}
						m_saStreamFormats.InsertAt(iFormat, strFmt);
					}
					else
					{
						m_saStreamFormats.InsertAt(iFormat, _T("未知"));
					}

					if (fmt >= 0 && iFormat == fmt)
					{
						pVStreamConfig->SetFormat(pMediaType);
						m_nCurrentStreamFormat = iFormat; // index of current format
					}
					// Delete the media type when you are done.
					DeleteMediaType(pMediaType);
				}
			}
		}
	    
	    // get capture format
		AM_MEDIA_TYPE* pMediaType;
		hr = pVStreamConfig->GetFormat(&pMediaType);
		if (SUCCEEDED(hr))
		{
			if (pMediaType->formattype == FORMAT_VideoInfo)
			{
				m_nWidth = HEADER(pMediaType->pbFormat)->biWidth;
				// resize our window to the default capture size
				m_nHeight = ABS(HEADER(pMediaType->pbFormat)->biHeight);
			}
			m_CurrentMediaSubType = pMediaType->subtype;
			DeleteMediaType(pMediaType);
		}

		pVStreamConfig->Release();
	}

	return hr;
}

HRESULT CVideoMediaRealtimeCapture::BuildFlirRtspGraph(bool bMulticast, int fmt)
{
	ASSERT(m_hWnd);

    HRESULT hr;

    // 若已启动，则停止
	if (m_eCurPlayState != PLAY_STATE_NOT_INIT)
		CloseInterfaces();

	m_nWidth = 320;			// 缺省
	m_nHeight = 240;

    if (!MakeBuilder())
    {
        TRACE(TEXT("Cannot instantiate graph builder"));
        return E_FAIL;
    }

    // make a filtergraph
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
                          IID_IGraphBuilder, (void **)&m_pGraphBuilder);
    //
    // give it to the graph builder and put the video
    // capture filter in the graph
    //
    hr = m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraphBuilder);
    if (FAILED(hr))
    {
        TRACE(TEXT("Cannot give graph to builder"));
		CloseInterfaces();
		return hr;
    }

    // make a filtergraph
    hr = CoCreateInstance(CLSID_FLIRRTSP, NULL, CLSCTX_INPROC, 
                          IID_IBaseFilter, (void **)&m_pDownstreamFilter);

	if (FAILED(hr))
	{
        TRACE(TEXT("Failed to create RTSP source filter"));
		CloseInterfaces();
		return hr;
	}

	hr = m_pGraphBuilder->AddFilter(m_pDownstreamFilter, L"Video Capture");
	if (FAILED(hr))
	{
        TRACE(TEXT("Failed to add RTSP source filter to graph"));
		CloseInterfaces();
		return hr;
	}

    // Now tell the RTSP filter to scan for video formats.
	CComPtr<IRTSPConfig> pCfg;
    hr = m_pDownstreamFilter->QueryInterface(IID_IRTSPConfig, (void **)&pCfg);
    if (SUCCEEDED(hr))
	{
		hr = pCfg->SetIP(inet_addr(m_sIpAddress));
		if (FAILED(hr))
		{
#ifdef _DEBUG
		    AfxMessageBox(TEXT("扫描RTSP源失败！"));
#endif
			CloseInterfaces();
			return hr;
		}

		hr = pCfg->SetMulticast(bMulticast);
		if (FAILED(hr))
		{
#ifdef _DEBUG
		    AfxMessageBox(TEXT("设置多播模式失败！"));
#endif
			hr = S_OK;
		}
    }

	hr = EnumStreamFormats(fmt);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("无法枚举流格式!"));
#endif
		CloseInterfaces();
		return hr;
	}

	// Create the Video Renderer and add it to the graph
    hr = AddVideoRenderer(m_hWnd);
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("无法创建VideoRenderer！"));
#endif
		CloseInterfaces();
		return hr;
    }

	if (m_CurrentMediaSubType == GUID_MEDIASUBTYPE_MP4V ||
		m_CurrentMediaSubType == GUID_MEDIASUBTYPE_H264)
	{
		// create a video decoder
		hr = CoCreateInstance(CLSID_FFDSHOW_DECODER, NULL, CLSCTX_INPROC, 
			                  IID_IBaseFilter, (void **)&m_pDecoder);
		if (FAILED(hr))
		{
			TRACE(TEXT("Failed to create FFDSHOW decoder filter"));
		}
		else
		{
			hr = m_pGraphBuilder->AddFilter(m_pDecoder, L"Video Decoder");
			if (FAILED(hr))
			{
				TRACE(TEXT("Failed to add decoder filter to graph"));
			}
		}
	}

	// Create the Sample Grabber and add it to the graph
    hr = AddSampleGrabber();
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("无法创建SampleGrabber！"));
#endif
		CloseInterfaces();
		return hr;
    }

	hr = m_pCaptureGraphBuilder2->RenderStream(NULL, &MEDIATYPE_Video, m_pDownstreamFilter,
										m_pDecoder, m_pVideoRenderer);
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("无法描述视频捕捉流！"));
#endif
		TRACE(_T("Cannot render video capture stream\r\n"));
		CloseInterfaces();
		return hr;
    }

	// QueryInterface for DirectShow interfaces
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEventEx);     

	CComPtr<IMediaFilter> pMF;
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaFilter, (void **)&pMF);
	if (SUCCEEDED(hr))
		pMF->SetSyncSource(NULL);

	CComPtr<IEnumPins> pEnum;
	m_pDownstreamFilter->EnumPins(&pEnum);
	hr = pEnum->Reset();
	hr = pEnum->Next(1, &m_pCamOutPin, NULL); 

	hr = InitVideoWindow(m_hWnd, m_nWidth, m_nHeight);

	if(NULL != m_pFrame)
		delete []m_pFrame;
	m_nFrameLen = m_nWidth * m_nHeight * 3;
	m_pFrame = (BYTE*) new BYTE[m_nFrameLen];
	m_eCurPlayState = PLAY_STATE_CONNECTED;

	GetCaps();

	// Run the graph to play the media
	hr = StartGraph();
	if (FAILED(hr))
	{
#ifdef _DEBUG
		TCHAR msg[80];
		CString s;
		AMGetErrorText(hr, msg, 80);
		s.Format("播放媒体失败: %s (%08X)", CString(msg), hr);
	    AfxMessageBox(s);
#endif
		return hr;
	}
	m_eCurPlayState = PLAY_STATE_RUNNING;

	// All done
	return S_OK;
}

////////////////////////////////////////////////////
//
//    Start the streaming of active graph.
//
////////////////////////////////////////////////////
HRESULT CVideoMediaRealtimeCapture::StartGraph()
{
    HRESULT hr;
    IMediaControl* pMediaControl = NULL;

    if ( !m_pGraphBuilder )
        return E_FAIL;
    
	// Query Interface for the media control 
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<PVOID *>(&pMediaControl));
    if (FAILED( hr ))
        return hr;

    if (m_pSampleGrabber)
    {
        // Get sample grabber filter.
	    ISampleGrabber* pGrab = NULL;  
        hr = m_pSampleGrabber->QueryInterface( IID_ISampleGrabber, (void**) &pGrab );

        if ( SUCCEEDED( hr ) )
        {
			if (m_pGrabCB == NULL)
				m_pGrabCB = new CGrabCB(this);
            pGrab->SetCallback( m_pGrabCB, 0 );
            SAFE_RELEASE( pGrab );
        }
    }

    // Start the graph.
    hr = pMediaControl->Run();
    if (FAILED(hr))
    {
        // Stop parts that ran
        pMediaControl->Stop();
        SAFE_RELEASE( pMediaControl );
        return hr;
    }

    SAFE_RELEASE( pMediaControl );
    return S_OK;
}

////////////////////////////////////////////////////
//
//    Stop active Graph
//
////////////////////////////////////////////////////
HRESULT CVideoMediaRealtimeCapture::StopGraph()
{
    HRESULT hr;
    ISampleGrabber* pGrab = NULL;  
    IMediaControl* pMediaControl = NULL;

    if (!m_pGraphBuilder)
        return E_FAIL;

    if (m_pSampleGrabber)
    {
        // Get sample grabber filter.
        hr = m_pSampleGrabber->QueryInterface( IID_ISampleGrabber, (void**) &pGrab );

        if ( SUCCEEDED( hr ) )
        {
            pGrab->SetCallback( NULL, 0 );
            SAFE_RELEASE( pGrab );
        }
    }

    // Stop the current graph
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, reinterpret_cast<PVOID *>(&pMediaControl));
    if (SUCCEEDED(hr))
    {
        hr = pMediaControl->Stop();
        SAFE_RELEASE(pMediaControl);
    }

    return hr;
}

HRESULT CVideoMediaRealtimeCapture::BuildPreviewGraph(CString sDeviceID, HWND hWnd, int fmt)
{
	HRESULT hr;

	if (m_eCurPlayState != PLAY_STATE_NOT_INIT)
		CloseInterfaces();

    if (!MakeBuilder())
    {
        TRACE(TEXT("Cannot instantiate graph builder"));
        return S_OK;
    }

    // make a filtergraph
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                          IID_IGraphBuilder, (void **)&m_pGraphBuilder);
    //
    // give it to the graph builder and put the video
    // capture filter in the graph
    //
    hr = m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraphBuilder);
    if (FAILED(hr))
    {
        TRACE(TEXT("Cannot give graph to builder"));
        SAFE_RELEASE(m_pGraphBuilder);
        SAFE_RELEASE(m_pCaptureGraphBuilder2);
		return S_FALSE;
    }

	// Bind Device Filter.  We know the device because the id was passed in
	if (!BindFilter(sDeviceID, &m_pDownstreamFilter))
		return S_FALSE;

	hr = m_pGraphBuilder->AddFilter(m_pDownstreamFilter, L"Video Capture");
	if (FAILED(hr))
		return hr;

	EnumStreamFormats(fmt);

	// Create the Video Renderer and add it to the graph
    hr = AddVideoRenderer(hWnd);
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("Cannot create Video Renderer"));
#endif
		return S_FALSE;
    }

	CComPtr<IEnumPins> pEnum;
	m_pDownstreamFilter->EnumPins(&pEnum);

	hr = pEnum->Reset();
	hr = pEnum->Next(1, &m_pCamOutPin, NULL); 
		
	// QueryInterface for DirectShow interfaces
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
    hr = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (void **)&m_pMediaEventEx);     

	hr = m_pGraphBuilder->Render(m_pCamOutPin);
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("Cannot render graph"));
#endif
		return S_FALSE;
    }

	hr = InitVideoWindow(hWnd, m_nWidth, m_nHeight);

	if(NULL != m_pFrame)
		delete []m_pFrame;
	m_nFrameLen = m_nWidth * m_nHeight * 3;
	m_pFrame = (BYTE*) new BYTE[m_nFrameLen];
		
	m_eCurPlayState = PLAY_STATE_CONNECTED;

	GetCaps();

	// Run the graph to play the media
	hr = m_pMediaControl->Run();
    if (FAILED(hr))
	{
#ifdef _DEBUG
		AfxMessageBox(_T("Cannot play media"));
#endif
		return S_FALSE;
	}

	m_eCurPlayState = PLAY_STATE_RUNNING;

	return hr;
}

// Make a graph builder object we can use for capture graph building
//
BOOL CVideoMediaRealtimeCapture::MakeBuilder()
{
	HRESULT hr;
    if (m_pCaptureGraphBuilder2)
        return TRUE;   // we have one already

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
			CLSCTX_INPROC, IID_ICaptureGraphBuilder2, 
			(void **)&m_pCaptureGraphBuilder2);

    if ( NULL == m_pCaptureGraphBuilder2 )
	{
        return FALSE;
    }
    
    return TRUE;
}

HRESULT CVideoMediaRealtimeCapture::RepaintVideo(HDC hdc)
{
	ASSERT(m_hWnd);

	HRESULT hr;
	IVMRWindowlessControl9* pWC;

	if (m_pVideoRenderer == NULL)
		return E_FAIL;

	hr = m_pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
    if( SUCCEEDED(hr)) 
    {
		pWC->RepaintVideo(m_hWnd, hdc);
        pWC->Release();
    }

	return hr;
}

// resize the media - use when reflected by the window
BOOL CVideoMediaRealtimeCapture::Resize()
{
	ASSERT(m_hWnd);

	HRESULT hr;
	IVMRWindowlessControl9* pWC;

	if (m_pVideoRenderer == NULL)
		return FALSE;

	hr = m_pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
    if( SUCCEEDED(hr)) 
    {
		// MEDIA SIZE
		RECT mediaRect;
		LONG  Width, Height, ARWidth, ARHeight;
		pWC->GetNativeVideoSize(&Width, &Height, &ARWidth, &ARHeight);
		mediaRect.left = 0;
		mediaRect.top = 0;
		mediaRect.right = Width;
		mediaRect.bottom = Height;

		RECT wndRect;
		GetClientRect(m_hWnd, &wndRect);
		pWC->SetVideoPosition(&mediaRect, &wndRect);
        pWC->Release();
		return TRUE;
    }
	else 
	{
	    IVideoWindow* pVW;
		hr = m_pVideoRenderer->QueryInterface(IID_IVideoWindow, (void**)&pVW);
		if (SUCCEEDED(hr))
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			hr = pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
			pVW->Release();
			return TRUE;
		}
	}
	
	return FALSE;
}

HRESULT CVideoMediaRealtimeCapture::InitVideoRenderer(HWND hWnd)
{
    HRESULT hr;
    IVideoWindow* pVW;

	if (m_pVideoRenderer == NULL)
		return E_FAIL;

	hr = m_pVideoRenderer->QueryInterface(IID_IVideoWindow, (void**)&pVW);

	if (SUCCEEDED(hr))
	{
		RECT rc;
		hr = pVW->put_Owner((OAHWND)hWnd);    // We own the window now
		hr = pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		// give the preview window all our space but where the status bar is
		GetClientRect(hWnd, &rc);
		hr = pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
		hr = pVW->put_Visible(OATRUE);
		pVW->Release();
	}

    return hr;
}

HRESULT CVideoMediaRealtimeCapture::AddSampleGrabber()
{
    HRESULT hr = S_OK;
	AM_MEDIA_TYPE* pMediaType = NULL;
	bool bAddFilter = false;

    CComPtr<IAMStreamConfig> pConfig;
	// Get stream config interface (used for device control)
    hr = m_pCaptureGraphBuilder2->FindInterface(
                &PIN_CATEGORY_CAPTURE,
                0,
                m_pDownstreamFilter, 
                IID_IAMStreamConfig, 
                reinterpret_cast<PVOID*>(&pConfig));

	if  (FAILED( hr ))
	{
		return hr;
	}

	hr = pConfig->GetFormat( &pMediaType );
	if (SUCCEEDED(hr) && pMediaType && (pMediaType->majortype == MEDIATYPE_Video))
    {
		if (pMediaType->subtype == GUID_MEDIASUBTYPE_F7M0)
			bAddFilter = true;
		DeleteMediaType( pMediaType );
    }

	if (bAddFilter && m_pSampleGrabber == NULL)
	{
		// Create Sample Grabber filter
		hr = CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
						IID_IBaseFilter, reinterpret_cast<PVOID *>(&m_pSampleGrabber));
		if (FAILED(hr)) 
		{
#ifdef _DEBUG
			CString msg;
			msg  = _T("无法创建样本过采集滤器。\n");
			msg += _T("请确保已安装DirectX 8.1或更高版本。\n");
			AfxMessageBox(msg, MB_OK | MB_ICONERROR);
#endif
		    return hr;
		}

	    // Add sample grabber filter to graph
		hr = m_pGraphBuilder->AddFilter( m_pSampleGrabber, L"Grabber");           
	}

    return hr;
}

HRESULT CVideoMediaRealtimeCapture::AddVideoRenderer(HWND hWnd)
{
    HRESULT hr;

	// Create the VMR and add it to the filter graph.
    hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
                          CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pVideoRenderer);
    if (SUCCEEDED(hr)) 
    {
        hr = m_pGraphBuilder->AddFilter(m_pVideoRenderer, L"Video Renderer");
        if (SUCCEEDED(hr)) 
        {
            // Set the rendering mode and number of streams.  
            IVMRFilterConfig* pConfig;

            hr = m_pVideoRenderer->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
            if( SUCCEEDED(hr)) 
            {
                pConfig->SetRenderingMode(VMRMode_Windowless);
                pConfig->Release();
            }

			IVMRWindowlessControl9* pWC;
            hr = m_pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
            if( SUCCEEDED(hr)) 
            {
                pWC->SetVideoClippingWindow(hWnd);
				//pWC->SetAspectRatioMode(VMR9ARMode_LetterBox);
                pWC->Release();
            }
        }
    }
	else
	{
	    // Try to create another more basic video renderer and add it to the filter graph.
		hr = CoCreateInstance(CLSID_VideoRenderer, NULL,
		                      CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pVideoRenderer);
        if (SUCCEEDED(hr)) 
		{
	        hr = m_pGraphBuilder->AddFilter(m_pVideoRenderer, L"Video Renderer");
		}
	}

    return hr;
}

bool CVideoMediaRealtimeCapture::BindFilter(CString id, IBaseFilter **pFilter)
{
	if (id.IsEmpty())
		return false;
	
    // enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
	{

		return false;
	}

    CComPtr<IEnumMoniker> pEnumMoniker;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
    if (hr != NOERROR) 
		return false;

    pEnumMoniker->Reset();
    ULONG cFetched;
    IMoniker* pMoniker;
	int index = 0;
    while (hr = pEnumMoniker->Next(1, &pMoniker, &cFetched), hr==S_OK)
    {
		IPropertyBag* pBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"DevicePath", &var, NULL);
			if (hr == NOERROR) 
			{
				CString devid = CString(var.bstrVal);
				SysFreeString(var.bstrVal);

				if (id.CompareNoCase(devid) == 0)
				{
					// Device found
					pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
					pBag->Release();
					pMoniker->Release();
					return true;
				}
			}
			pBag->Release();
		}
		pMoniker->Release();
		index++;
    }

	return true;
}

// 设置抓取尺寸，通常从当前media类型中查询尺寸
HRESULT CVideoMediaRealtimeCapture::InitVideoWindow(HWND hWnd,int width, int height)
{
	HRESULT hr;
	
    AM_MEDIA_TYPE *pMediaType = NULL, *pfnt = NULL;

	hr = InitVideoRenderer(hWnd);

    CComPtr<IAMStreamConfig> pConfig;
	// Get stream config interface (used for device control)
    hr = m_pCaptureGraphBuilder2->FindInterface(
                &PIN_CATEGORY_CAPTURE,
                0,
                m_pDownstreamFilter, 
                IID_IAMStreamConfig, 
                reinterpret_cast<PVOID*>(&pConfig));

    if (SUCCEEDED(hr))
    {
        hr = pConfig->GetFormat( &pfnt );
		if (SUCCEEDED(hr) && pfnt && (pfnt->majortype == MEDIATYPE_Video))
        {
            m_nWidth = ((VIDEOINFOHEADER *)pfnt->pbFormat)->bmiHeader.biWidth;
            m_nHeight = ((VIDEOINFOHEADER *)pfnt->pbFormat)->bmiHeader.biHeight;
            m_nHeight = ABS(m_nHeight);
            DeleteMediaType( pfnt );
        }
    }

	if (m_pVideoRenderer) // Do we have a video window?
	{
		IVMRWindowlessControl9* pWC;
		RECT rcDest;
		::GetClientRect (hWnd, &rcDest);
		hr = m_pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
		if( SUCCEEDED(hr)) 
		{
			pWC->SetVideoPosition(NULL, &rcDest);
			pWC->Release();
	    }
		else 
		{
		    IVideoWindow* pVW;
			hr = m_pVideoRenderer->QueryInterface(IID_IVideoWindow, (void**)&pVW);
			if (SUCCEEDED(hr))
			{
				hr = pVW->SetWindowPosition(0, 0, rcDest.right, rcDest.bottom);
				pVW->Release();
			}
		}
	}

	return hr;
}

void CVideoMediaRealtimeCapture::Stop()
{
    HRESULT hr;

	if((m_eCurPlayState == PLAY_STATE_PAUSED) || (m_eCurPlayState == PLAY_STATE_RUNNING))
    {
        //hr = m_pMediaControl->Stop();
		hr = StopGraph();
        m_eCurPlayState = PLAY_STATE_STOPPED;		
    }
}

void CVideoMediaRealtimeCapture::CloseInterfaces(void)
{
    HRESULT hr;

	// Stop media playback
	if((m_eCurPlayState == PLAY_STATE_PAUSED) || (m_eCurPlayState == PLAY_STATE_RUNNING))
	{
	    if (m_pMediaControl)
		    hr = m_pMediaControl->Stop();	    
	}

	m_eCurPlayState = PLAY_STATE_STOPPED;  

    // destroy the graph downstream of our capture filters
	if (m_pDownstreamFilter)
        NukeDownStream(m_pDownstreamFilter);

	// Release and zero DirectShow interfaces
	if (m_pCamOutPin)
		m_pCamOutPin->Disconnect();

	SAFE_RELEASE(m_pCamOutPin);        
    SAFE_RELEASE(m_pMediaControl);    
	SAFE_RELEASE(m_pMediaEventEx);
	SAFE_RELEASE(m_pBaseFilterMux);
    SAFE_RELEASE(m_pFileSinkFilter);
    SAFE_RELEASE(m_pVideoRenderer);
    SAFE_RELEASE(m_pSampleGrabber);
    SAFE_RELEASE(m_pBaseFilterST);
	SAFE_RELEASE(m_pDownstreamFilter);
	SAFE_RELEASE(m_pDecoder);        
    SAFE_RELEASE(m_pGraphBuilder);    
	SAFE_RELEASE(m_pCaptureGraphBuilder2);

	// 删除分配的内存
	if (m_pFrame != NULL)
	{
		delete []m_pFrame;
		m_pFrame = NULL;
	}
	if(m_pSampleCbFrame != NULL)
	{
		delete []m_pSampleCbFrame;
		m_pSampleCbFrame = NULL;
	}

	m_eCurPlayState = PLAY_STATE_NOT_INIT;
}

void CVideoMediaRealtimeCapture::ImageCapture()
{
	ASSERT(m_hWnd);

	BITMAPINFOHEADER bmpHeader;
	BYTE* pImage;
	DWORD dwSize;
	dwSize = this->GrabFrame(&bmpHeader);
	this->GetFrame(&pImage);

	HDC hdc = ::GetDC(m_hWnd);
	CRect rect;
	::GetClientRect(m_hWnd, (LPRECT)rect);

	// Set mode and draw the image
	int oldMode = ::SetStretchBltMode(hdc, COLORONCOLOR);

	int w = bmpHeader.biWidth;
	int h = bmpHeader.biHeight;

	int ret = ::StretchDIBits(hdc,
		rect.left, rect.top, rect.Width(), rect.Height(),
		0, 0, w, h, pImage, (LPBITMAPINFO)&bmpHeader, DIB_RGB_COLORS, SRCCOPY);

	SetStretchBltMode(hdc, oldMode); // Restore mode
}

//Capture RAW IMAGE BITS 24bits/pixel
DWORD CVideoMediaRealtimeCapture::ImageCapture(LPCTSTR szFile)
{
	BITMAPINFOHEADER bmpHeader;
	BYTE* pImage;
	DWORD dwSize, dwWritten;
	dwSize = this->GrabFrame(&bmpHeader);
	this->GetFrame(&pImage);

	HANDLE hFile = CreateFile(szFile, 
						GENERIC_WRITE,
						FILE_SHARE_READ, 
						NULL,
						CREATE_ALWAYS, 
						FILE_ATTRIBUTE_NORMAL, 
						0);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

    BITMAPFILEHEADER bfh;
    ZeroMemory(&bfh, sizeof(bfh));
    bfh.bfType = 'MB';  // Little-endian for "MB".
    bfh.bfSize = sizeof( bfh ) + sizeof(BITMAPINFOHEADER) + dwSize;
    bfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof(BITMAPINFOHEADER);

    // Write the file header
    WriteFile(hFile, &bfh, sizeof( bfh ), &dwWritten, NULL );
    WriteFile(hFile, &bmpHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL );
    // Write the bitmap
	WriteFile(hFile, (LPCVOID)pImage, m_nFrameLen, &dwWritten, 0);
	// Close the file
	CloseHandle(hFile);

	return dwWritten;
}

void CVideoMediaRealtimeCapture::DeleteMediaType(AM_MEDIA_TYPE* pMediaType)
{
    if (pMediaType == NULL)
        return;

    if (pMediaType->cbFormat != 0)
	{
        CoTaskMemFree((PVOID)pMediaType->pbFormat);

        // Strictly unnecessary but tidier
        pMediaType->cbFormat = 0;
        pMediaType->pbFormat = NULL;
    }
    if (pMediaType->pUnk != NULL)
	{
        pMediaType->pUnk->Release();
        pMediaType->pUnk = NULL;
    }

    CoTaskMemFree((PVOID)pMediaType);
}

DWORD CVideoMediaRealtimeCapture::GrabFrame(BITMAPINFOHEADER* pInfo)
{
	long lOut=-1;
	HRESULT hr;

	if (m_pVideoRenderer == NULL)
		return lOut;

	CComPtr<IVMRWindowlessControl9> pWC;
	hr = m_pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	if (SUCCEEDED(hr)) 
    {
		BYTE* lpCurrImage = NULL;
        // Read the current video frame into a byte buffer.  The information
        // will be returned in a packed Windows DIB and will be allocated
        // by the VMR.
        if (pWC->GetCurrentImage(&lpCurrImage) == S_OK)
        {
			LPBITMAPINFOHEADER  pdib = (LPBITMAPINFOHEADER) lpCurrImage;

			memcpy(pInfo, pdib, sizeof(BITMAPINFOHEADER));
			if (m_pFrame==NULL || (pdib->biHeight * pdib->biWidth * 3) != m_nFrameLen )
			{
				if (m_pFrame != NULL)
					delete []m_pFrame;

				m_nFrameLen = pdib->biHeight * pdib->biWidth * 3;
				m_pFrame = new BYTE [pdib->biHeight * pdib->biWidth * 3] ;
			}			

			if (pdib->biBitCount == 32) 
			{
				DWORD  dwSize=0, dwWritten=0;			

				BYTE* pTemp32;
				pTemp32=lpCurrImage + sizeof(BITMAPINFOHEADER);

				//change from 32 to 24 bit /pixel
				this->Convert24Image(pTemp32, m_pFrame, pdib->biSizeImage);
				pInfo->biBitCount = 24;
				pInfo->biSizeImage = (pdib->biSizeImage * 3)/4;
				//pInfo->biHeight = -pdib->biHeight;
			}

			CoTaskMemFree(lpCurrImage);	//free the image 
		}
		else
			return lOut;

	}
	else
		return lOut;
	
    return lOut=m_nFrameLen;
}

bool CVideoMediaRealtimeCapture::Convert24Image(BYTE* p32Img, BYTE* p24Img,DWORD dwSize32)
{
	if (p32Img != NULL && p24Img != NULL && dwSize32>0)
	{
		DWORD dwSize24;

		dwSize24 = (dwSize32 * 3)/4;
		BYTE *pTemp, *ptr;
		//pTemp = p32Img + sizeof(BITMAPINFOHEADER); ;
		pTemp = p32Img;

		ptr = p24Img; //p24Img + dwSize24-1 ;

		int ival=0;
		for (DWORD index = 0; index < dwSize32/4 ; index++)
		{									
			unsigned char r = *(pTemp++);
			unsigned char g = *(pTemp++);
			unsigned char b = *(pTemp++);
			(pTemp++);//skip alpha
						
			*(ptr++) = r;
			*(ptr++) = g;
			*(ptr++) = b;			
		}	
	}
	else
		return false;

	return true;
}

BOOL CVideoMediaRealtimeCapture::Start()
{
    if (!m_pMediaControl)
        return FALSE;
  
    if (m_eCurPlayState == PLAY_STATE_CONNECTED)
    {
		//HRESULT hr = m_pMediaControl->Run();
		HRESULT hr = StartGraph();
	    if (FAILED(hr))
		{
#ifdef _DEBUG
			AfxMessageBox(_T("无法启动媒体流。"));
#endif
			return FALSE;
		}

		m_eCurPlayState = PLAY_STATE_RUNNING;
	}
	return TRUE;
}

BOOL CVideoMediaRealtimeCapture::Pause()
{	
    if (!m_pMediaControl)
        return FALSE;
  
    if ( (m_eCurPlayState == PLAY_STATE_PAUSED) ||
		 (m_eCurPlayState == PLAY_STATE_STOPPED) )
    {
		this->Stop();
		if (SUCCEEDED(m_pMediaControl->Run()))
		{
            m_eCurPlayState = PLAY_STATE_RUNNING;
		}
    }
    else if (m_eCurPlayState == PLAY_STATE_RUNNING)
    {
        if (SUCCEEDED(m_pMediaControl->Pause()))
            m_eCurPlayState = PLAY_STATE_PAUSED;
    }

	return TRUE;
}

DWORD CVideoMediaRealtimeCapture::GetFrame(BYTE **pFrame)
{
	if(m_pFrame && m_nFrameLen)
		*pFrame = m_pFrame;

	return m_nFrameLen;
}

// 枚举全部视频捕捉设备
int  CVideoMediaRealtimeCapture::EnumDevices(HWND hList)
{
	if (!hList)
		return  -1;

	int id = 0;

	CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			  IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
		return -1;

    CComPtr<IEnumMoniker> pEnumMoniker;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
								&pEnumMoniker, 0);
    if (hr != NOERROR) 
		return -1 ;

    pEnumMoniker->Reset();
    ULONG cFetched;
    IMoniker* pMoniker;
    while(hr = pEnumMoniker->Next(1, &pMoniker, &cFetched), hr==S_OK)
    {
		IPropertyBag* pBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) 
			{
				TCHAR str[2048];		

				id++;
				WideCharToMultiByte(CP_ACP,0,var.bstrVal, -1, str, 2048, NULL, NULL);
				(long)SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)str);
				SysFreeString(var.bstrVal);
			}

			pBag->Release();
		}

		pMoniker->Release();
    }

	return id;
}

////////////////////////////////////////////////////
//
//  Convert HRESULT to string.
//
////////////////////////////////////////////////////
CString CVideoMediaRealtimeCapture::GetError(HRESULT hr)
{
    CString err;
    LPVOID lpMsgBuf;

    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL);
               
    err = CString((LPCTSTR)lpMsgBuf);
    LocalFree( lpMsgBuf ); // Free the buffer.
    return err;
}

// Tear down everything downstream of a given filter
void CVideoMediaRealtimeCapture::NukeDownStream(IBaseFilter *pf)
{
    IPin *pP=0, *pTo=0;
    ULONG u;
    IEnumPins* pins = NULL;
    PIN_INFO pininfo;

    if (!pf)
        return;

    HRESULT hr = pf->EnumPins(&pins);
    if (FAILED( hr ))
        return;

    pins->Reset();

    while (hr == NOERROR)
    {
        hr = pins->Next(1, &pP, &u);
        if (hr == S_OK && pP)
        {
            pP->ConnectedTo(&pTo);
            if (pTo)
            {
                hr = pTo->QueryPinInfo(&pininfo);
                if (hr == NOERROR)
                {
                    if (pininfo.dir == PINDIR_INPUT)
                    {
                        NukeDownStream(pininfo.pFilter);
                        m_pGraphBuilder->Disconnect(pTo);
                        m_pGraphBuilder->Disconnect(pP);
                        m_pGraphBuilder->RemoveFilter(pininfo.pFilter);
                    }

                    pininfo.pFilter->Release();
                }

                pTo->Release();
            }

            pP->Release();
        }
    }

    pins->Release();
}

HRESULT CVideoMediaRealtimeCapture::DoCameraCommand(PBYTE pCmd, ULONG lSize)
{
	HRESULT hr;

    if (!m_pCaptureGraphBuilder2 || !m_pDownstreamFilter) 
		return E_FAIL;

	// 获取FLIR命令接口，以便控制设备
    IKsPropertySet* pPropSet = NULL;
    hr = m_pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, 
                                   &MEDIATYPE_Video,
		 						   m_pDownstreamFilter, IID_IKsPropertySet, (void **)&pPropSet);
    if (FAILED(hr))
		return hr;

	ULONG supp;
	hr = pPropSet->QuerySupported(PROPSETID_FLIRDCAM_EXT1, KSPROPERTY_FLIRDCAM_EXT1_EXT1, &supp);

	if (SUCCEEDED(hr) && (supp & KSPROPERTY_SUPPORT_GET))
	{
		// Command interface supported
		DWORD cbReturned;
		KSPROPERTY_FLIRDCAM_EXT1_S instance;
		KSPROPERTY_FLIRDCAM_EXT1_S prop;
		memcpy(instance.DataBuffer, pCmd, lSize);
		instance.Length = lSize;

		hr = pPropSet->Get(PROPSETID_FLIRDCAM_EXT1,
                           KSPROPERTY_FLIRDCAM_EXT1_EXT1, // Property ID
                           (LPVOID*)&instance,
                           sizeof(KSPROPERTY_FLIRDCAM_EXT1_S),
                           (LPVOID*)&prop,
                           sizeof(KSPROPERTY_FLIRDCAM_EXT1_S),
                           &cbReturned);

        if ( SUCCEEDED(hr) )
			memcpy(pCmd, prop.DataBuffer, prop.Length);

	    pPropSet->Release();
		return hr;
	}
    pPropSet->Release();

	// It may be a FLIR camera in DV mode
    // Get external transport interface (used for device control)
	IAMExtTransport* pExtTrans = NULL;
    hr = m_pCaptureGraphBuilder2->FindInterface(
                &PIN_CATEGORY_CAPTURE, 
                &MEDIATYPE_Video, 
                m_pDownstreamFilter,
                IID_IAMExtTransport, 
                reinterpret_cast<PVOID*>(&pExtTrans));

	if (SUCCEEDED(hr)) 
	{
		long sz = (long)lSize;
        pExtTrans->GetTransportBasicParameters( ED_RAW_EXT_DEV_CMD, &sz, (LPOLESTR*) pCmd );
		pExtTrans->Release();
	}

	return hr;
}

void CVideoMediaRealtimeCapture::SampleCB(IMediaSample* pSample)
{
    HRESULT hr;
    USHORT* pPixel = NULL;

	m_nSampleCbFrameLen = pSample->GetActualDataLength();

	if (NULL != m_pSampleCbFrame)
		delete []m_pSampleCbFrame;
	m_pSampleCbFrame = (BYTE*) new BYTE[m_nSampleCbFrameLen];

    hr = pSample->GetPointer(&m_pSampleCbFrame);
    if (FAILED (hr))
	{
		delete []m_pSampleCbFrame;
		m_pSampleCbFrame = NULL;
		m_nSampleCbFrameLen = 0;
        return;
	}

    // Get pixel value 
    pPixel = (USHORT*)m_pSampleCbFrame;
    
	m_PixelValue = pPixel[m_nWidth * (m_nHeight / 2) + (m_nWidth / 2)];
}
