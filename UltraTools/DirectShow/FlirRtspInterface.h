//
// RTSP=Real time Transport StreamProtocol，是FLIR基于标准RTP定义的协议
//


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Jan 23 13:49:35 2010
 */
/* Compiler settings for .\fsrtspifc.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __fsrtspifc_h_h__
#define __fsrtspifc_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRTSPConfig_FWD_DEFINED__
#define __IRTSPConfig_FWD_DEFINED__
typedef interface IRTSPConfig IRTSPConfig;
#endif 	/* __IRTSPConfig_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_fsrtspifc_0000 */
/* [local] */ 

// {B7E9019A-6857-4baa-BC3C-1522D14C7281}
DEFINE_GUID(CLSID_FLIRRTSP,
0xb7e9019a, 0x6857, 0x4baa, 0xbc, 0x3c, 0x15, 0x22, 0xd1, 0x4c, 0x72, 0x81);
// {1A50100E-2CDA-4308-B302-CA2E4C98B029}
DEFINE_GUID(CLSID_Proppage,
0x1a50100e, 0x2cda, 0x4308, 0xb3, 0x2, 0xca, 0x2e, 0x4c, 0x98, 0xb0, 0x29);



extern RPC_IF_HANDLE __MIDL_itf_fsrtspifc_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_fsrtspifc_0000_v0_0_s_ifspec;

#ifndef __IRTSPConfig_INTERFACE_DEFINED__
#define __IRTSPConfig_INTERFACE_DEFINED__

/* interface IRTSPConfig */
/* [uuid][object] */ 


EXTERN_C const IID IID_IRTSPConfig;
//extern const IID IID_IRTSPConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D733BAD4-4C64-45d9-B100-65D5E30816BF")
    IRTSPConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
            /* [in] */ ULONG ulMedia) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMediaType( 
            /* [out] */ ULONG *pMedia) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIP( 
            /* [in] */ ULONG ulIP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIP( 
            /* [out] */ ULONG *pIP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFmt( 
            /* [in] */ ULONG ulFmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFmt( 
            /* [out] */ ULONG *pFmt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMulticast( 
            /* [in] */ ULONG ulMulticast) = 0;

		virtual HRESULT STDMETHODCALLTYPE Scan( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsConnected( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRTSPConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRTSPConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRTSPConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRTSPConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetMediaType )( 
            IRTSPConfig * This,
            /* [in] */ ULONG ulMedia);
        
        HRESULT ( STDMETHODCALLTYPE *GetMediaType )( 
            IRTSPConfig * This,
            /* [out] */ ULONG *pMedia);
        
        HRESULT ( STDMETHODCALLTYPE *SetIP )( 
            IRTSPConfig * This,
            /* [in] */ ULONG ulIP);
        
        HRESULT ( STDMETHODCALLTYPE *GetIP )( 
            IRTSPConfig * This,
            /* [out] */ ULONG *pIP);
        
        HRESULT ( STDMETHODCALLTYPE *SetFmt )( 
            IRTSPConfig * This,
            /* [in] */ ULONG ulFmt);
        
        HRESULT ( STDMETHODCALLTYPE *GetFmt )( 
            IRTSPConfig * This,
            /* [out] */ ULONG *pFmt);
        
        HRESULT ( STDMETHODCALLTYPE *Scan )( 
            IRTSPConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsConnected )( 
            IRTSPConfig * This);
        
        END_INTERFACE
    } IRTSPConfigVtbl;

    interface IRTSPConfig
    {
        CONST_VTBL struct IRTSPConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRTSPConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRTSPConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRTSPConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRTSPConfig_SetMediaType(This,ulMedia)	\
    (This)->lpVtbl -> SetMediaType(This,ulMedia)

#define IRTSPConfig_GetMediaType(This,pMedia)	\
    (This)->lpVtbl -> GetMediaType(This,pMedia)

#define IRTSPConfig_SetIP(This,ulIP)	\
    (This)->lpVtbl -> SetIP(This,ulIP)

#define IRTSPConfig_GetIP(This,pIP)	\
    (This)->lpVtbl -> GetIP(This,pIP)

#define IRTSPConfig_SetFmt(This,ulFmt)	\
    (This)->lpVtbl -> SetFmt(This,ulFmt)

#define IRTSPConfig_GetFmt(This,pFmt)	\
    (This)->lpVtbl -> GetFmt(This,pFmt)

#define IRTSPConfig_Scan(This)	\
    (This)->lpVtbl -> Scan(This)

#define IRTSPConfig_IsConnected(This)	\
    (This)->lpVtbl -> IsConnected(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRTSPConfig_SetMediaType_Proxy( 
    IRTSPConfig * This,
    /* [in] */ ULONG ulMedia);


void __RPC_STUB IRTSPConfig_SetMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_GetMediaType_Proxy( 
    IRTSPConfig * This,
    /* [out] */ ULONG *pMedia);


void __RPC_STUB IRTSPConfig_GetMediaType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_SetIP_Proxy( 
    IRTSPConfig * This,
    /* [in] */ ULONG ulIP);


void __RPC_STUB IRTSPConfig_SetIP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_GetIP_Proxy( 
    IRTSPConfig * This,
    /* [out] */ ULONG *pIP);


void __RPC_STUB IRTSPConfig_GetIP_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_SetFmt_Proxy( 
    IRTSPConfig * This,
    /* [in] */ ULONG ulFmt);


void __RPC_STUB IRTSPConfig_SetFmt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_GetFmt_Proxy( 
    IRTSPConfig * This,
    /* [out] */ ULONG *pFmt);


void __RPC_STUB IRTSPConfig_GetFmt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_Scan_Proxy( 
    IRTSPConfig * This);


void __RPC_STUB IRTSPConfig_Scan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTSPConfig_IsConnected_Proxy( 
    IRTSPConfig * This);


void __RPC_STUB IRTSPConfig_IsConnected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRTSPConfig_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_fsrtspifc_0010 */
/* [local] */ 

#define DECLARE_IRTSPCONFIG();\
virtual STDMETHODIMP SetMediaType(ULONG);\
virtual STDMETHODIMP GetMediaType(ULONG *);\
virtual STDMETHODIMP SetIP(ULONG);\
virtual STDMETHODIMP GetIP(ULONG *);\
virtual STDMETHODIMP SetFmt(ULONG);\
virtual STDMETHODIMP GetFmt(ULONG *);\
virtual STDMETHODIMP Scan();\
virtual STDMETHODIMP IsConnected();


extern RPC_IF_HANDLE __MIDL_itf_fsrtspifc_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_fsrtspifc_0010_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


