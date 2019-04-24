//////////////////////////////////////////////////////////////////////
//
// 本类利用DirectShow实现视频同步捕捉与显示的基本接口
//
//////////////////////////////////////////////////////////////////////
//
//	References: MS DirectShow Samples
//
// VideoMediaRealtimeCapture.h: interface for the CVideoMediaRealtimeCapture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_VIDEO_MEDIA_REALTIME_CAPTURE_H_)
#define _VIDEO_MEDIA_REALTIME_CAPTURE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <dshow.h>
#include <d3d9.h>
#include <vmr9.h>

#define FCAM_MTX	0
#define FCAM_BTH	1

enum PLAY_STATE
{
	PLAY_STATE_STOPPED,
	PLAY_STATE_PAUSED,
	PLAY_STATE_RUNNING,
	PLAY_STATE_CONNECTED,
	PLAY_STATE_NOT_INIT
};

class CGrabCB;

class CVideoMediaRealtimeCapture  
{
public:

	CVideoMediaRealtimeCapture();

	void PresetCamera(HWND hWnd, CString& sIpAddress);	//预设摄像机参数

	int EnumDevices(HWND hList);
	HRESULT BuildPreviewGraph(CString sDeviceID, HWND hWnd, int fmt = -1);
	HRESULT BuildFlirRtspGraph(bool bMulticast = false, int fmt = -1);
	void GetCaps();
	HRESULT EnumStreamFormats(int fmt);
	HRESULT SetFrameRate(double dFrameRate);
	DWORD GetFrame(BYTE** pFrame);

	BOOL Pause();
	BOOL Start();
	void Stop();
	PLAY_STATE GetPlayState(){return m_eCurPlayState;}

	DWORD ImageCapture(LPCTSTR szFile);					// 将当前帧保存为BMP文件
	DWORD GrabFrame(BITMAPINFOHEADER* pInfo);
	CSize GetFrameSize() { return CSize(m_nWidth, m_nHeight); }
	HRESULT DoCameraCommand(PBYTE pCmd, ULONG lSize);
	void CloseInterfaces(void);
	void ImageCapture();
	CString GetError(HRESULT hr);
	void SampleCB(IMediaSample* pSample);
	
	virtual ~CVideoMediaRealtimeCapture();

	BOOL Resize();
	HRESULT RepaintVideo(HDC hdc);

	int GetCameraType() { return m_nCameraType; }
	int GetCurrentStreamFormat() { return m_nCurrentStreamFormat; }
	double GetFrameRate();
	USHORT getValue() { return m_PixelValue; }

	CStringArray			m_saStreamFormats;			// camera支持的StreamFormats
	CArray<float, float>	m_arrFrameRates;			// camera支持的FrameRate
	
protected:

	// PresetCamera(...)设置，其它函数读取
	// 便于各函数统一针对单一camera进行操作
	HWND					m_hWnd;						// 消息处理窗口句柄
	CString					m_sIpAddress;				// 摄像机IP地址

	int						m_nCameraType;				// 枚举StreamFormat时，标志FLIR是否支持
	int						m_nCurrentStreamFormat;		// index of current format
	GUID					m_CurrentMediaSubType;
	float					m_fMaxFrameRate, m_fMinFrameRate;

	IGraphBuilder*			m_pGraphBuilder;
	IMediaControl*			m_pMediaControl;
	IMediaEventEx*			m_pMediaEventEx;
	ICaptureGraphBuilder2*	m_pCaptureGraphBuilder2;

	IBaseFilter*			m_pBaseFilterMux;
    IFileSinkFilter*		m_pFileSinkFilter;
	IBaseFilter*			m_pBaseFilterST;

    IBaseFilter*			m_pSampleGrabber;
	IBaseFilter*			m_pVideoRenderer;
	IPin*					m_pCamOutPin;
	IBaseFilter*			m_pDownstreamFilter;
	IBaseFilter*			m_pDecoder;
    CGrabCB*                m_pGrabCB;

	PLAY_STATE				m_eCurPlayState;

	int						m_nWidth;
	int						m_nHeight;
	USHORT					m_PixelValue;

	BYTE*					m_pFrame;			// 在窗口中显示的帧
	long					m_nFrameLen;

	// 包含温度信息的帧（在SampleCB()中与像素值同步获取）
	// 视频流设置为[IrOnly 320×240]时才激活SampleCB()
	// 按FPF格式保存帧，但FLIR QuickReport不识别:-(
	// 待FLIR完善后再处理，目前只能基于ocx保存含温度信息的红外图像:-(
	// BFish 2010.08.20
	BYTE*					m_pSampleCbFrame;
	long					m_nSampleCbFrameLen;
	
	bool BindFilter(CString id, IBaseFilter** pFilter);
	HRESULT AddSampleGrabber();
	HRESULT AddVideoRenderer(HWND hWnd);
	HRESULT InitVideoWindow(HWND hWnd, int width, int height);
	HRESULT InitVideoRenderer(HWND hWnd);
	BOOL MakeBuilder();
	HRESULT StartGraph();
	HRESULT StopGraph();
	
	void NukeDownStream(IBaseFilter* pf);
	void DeleteMediaType(AM_MEDIA_TYPE* pMediaType);
	bool Convert24Image(BYTE* p32Img, BYTE* p24Img, DWORD dwSize32);
private:
	
};

#endif // !defined(_VIDEO_MEDIA_REALTIME_CAPTURE_H_)
