#include <windows.h>
#include <oaidl.h>
#include <wrl/client.h>
#include <cstdio>
#ifndef _Out_retval_
#define _Out_retval_ _Out_
#endif
struct IUIAutomationClientConnectionCallback;
MIDL_INTERFACE("B2E8A3F1-4C5D-4E7A-8F6B-3D2E1C9A0B8F")
    IUIAutomationClientInfo : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProcessId( 
            /* [retval][out] */ __RPC__out DWORD *processId) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProcessName( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *processName) = 0;
        
    };
MIDL_INTERFACE("F4B8A2E1-9C3D-4A7E-8F6B-2D5E4C1A9B8F")
    IUIAutomationClientInfoSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterClientConnectionCallback( 
            /* [annotation][in] */ 
            _In_  IUIAutomationClientConnectionCallback *callback,
            /* [annotation][out] */ 
            _Out_  unsigned __int64 *handle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterClientConnectionCallback( 
            /* [annotation][in] */ 
            _In_  unsigned __int64 handle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnectedClients( 
            /* [annotation][retval][out] */ 
            _Out_retval_  SAFEARRAY * *clients) = 0;
        
    };
class DECLSPEC_UUID("A8D4F123-7B2C-4E5F-9A1B-3C8D6E9F0A2B")
CUIAutomationClientInfoSource;
