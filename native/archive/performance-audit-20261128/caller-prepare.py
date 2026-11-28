from pathlib import Path
import json,re

p=Path(__file__).resolve().parent
source=(p/'client-thread-window.cpp').read_text()
assert source.count('thread-events.jsonl')==1
source=source.replace('thread-events.jsonl','caller-events.jsonl')
needle='void Snapshot() {'
helper=r'''
class CallerFilter final : public IMessageFilter {
 public:
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void** result) override {
    if(!result) return E_POINTER;
    *result=nullptr;
    if(iid==IID_IUnknown||iid==IID_IMessageFilter) {
      *result=static_cast<IMessageFilter*>(this);AddRef();return S_OK;
    }
    return E_NOINTERFACE;
  }
  ULONG STDMETHODCALLTYPE AddRef() override {return InterlockedIncrement(&references_);}
  ULONG STDMETHODCALLTYPE Release() override {
    ULONG count=InterlockedDecrement(&references_);if(!count) delete this;return count;
  }
  DWORD STDMETHODCALLTYPE HandleInComingCall(DWORD type,HTASK caller,DWORD tick,LPINTERFACEINFO info) override {
    ++incoming_;
    DWORD tid=static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(caller));
    GUID iid=info?info->iid:GUID_NULL;
    wchar_t text[40]={};StringFromGUID2(iid,text,40);
    std::wstring key=std::to_wstring(tid)+L"/"+text+L"/"+std::to_wstring(info?info->wMethod:0);
    auto found=calls_.find(key);
    if(found!=calls_.end()) ++found->second.count;
    else if(calls_.size()<256) {
      Call call;call.tid=tid;call.iid=iid;call.method=info?info->wMethod:0;call.count=1;
      call.thread=OpenThread(THREAD_QUERY_LIMITED_INFORMATION,FALSE,tid);
      if(call.thread) call.pid=GetProcessIdOfThread(call.thread);
      call.error=call.pid?0:GetLastError();call.first_tick=GetTickCount64();
      calls_.emplace(key,call);
    } else ++overflow_;
    return previous_?previous_->HandleInComingCall(type,caller,tick,info):SERVERCALL_ISHANDLED;
  }
  DWORD STDMETHODCALLTYPE RetryRejectedCall(HTASK caller,DWORD tick,DWORD reject) override {
    return previous_?previous_->RetryRejectedCall(caller,tick,reject):static_cast<DWORD>(-1);
  }
  DWORD STDMETHODCALLTYPE MessagePending(HTASK caller,DWORD tick,DWORD pending) override {
    return previous_?previous_->MessagePending(caller,tick,pending):PENDINGMSG_WAITDEFPROCESS;
  }
  void Register() {
    IMessageFilter* old=nullptr;HRESULT hr=CoRegisterMessageFilter(this,&old);
    previous_.Attach(old);registered_=hr==S_OK;
    if(FILE* file=OpenLog()) {
      fprintf(file,"{\"event\":\"filter_registered\",\"hr\":%ld,\"previousFilter\":%s,\"thread\":%lu}\n",hr,old?"true":"false",GetCurrentThreadId());fclose(file);
    }
  }
  void Restore() {
    if(!registered_) return;
    IMessageFilter* removed=nullptr;HRESULT hr=CoRegisterMessageFilter(previous_.Get(),&removed);
    if(removed) removed->Release();registered_=false;
    if(FILE* file=OpenLog()) {fprintf(file,"{\"event\":\"filter_restored\",\"hr\":%ld}\n",hr);fclose(file);}
  }
  void Write(FILE* file) {
    fprintf(file,",\"incomingComCalls\":%llu,\"filterOverflow\":%llu,\"callers\":[",incoming_,overflow_);
    bool first=true;
    for(const auto& item:calls_) {
      const auto& call=item.second;wchar_t guid[40]={};StringFromGUID2(call.iid,guid,40);
      BSTR value=SysAllocString(guid);auto text=Escape(value);SysFreeString(value);
      fprintf(file,"%s{\"thread\":%lu,\"pid\":%lu,\"pidError\":%lu,\"iid\":\"%s\",\"method\":%u,\"count\":%llu,\"firstTick\":%llu}",first?"":",",call.tid,call.pid,call.error,text.c_str(),call.method,call.count,call.first_tick);first=false;
    }
    fprintf(file,"]");
  }
 private:
  ~CallerFilter() {for(auto& item:calls_) if(item.second.thread) CloseHandle(item.second.thread);}
  struct Call {DWORD tid=0,pid=0,error=0;GUID iid={};WORD method=0;unsigned long long count=0,first_tick=0;HANDLE thread=nullptr;};
  LONG references_=1;
  bool registered_=false;
  unsigned long long incoming_=0,overflow_=0;
  Microsoft::WRL::ComPtr<IMessageFilter> previous_;
  std::map<std::wstring,Call> calls_;
};
CallerFilter* caller_filter=nullptr;
'''
assert source.count(needle)==1
source=source.replace(needle,helper+'\n'+needle)
source=source.replace('WriteMessages(file);','WriteMessages(file);\n  if(caller_filter) caller_filter->Write(file);')
source=source.replace('Mark("first_frame");','Mark("first_frame");\n    caller_filter=new CallerFilter();caller_filter->Register();')
source=source.replace('Mark("destroy_enter");','Mark("destroy_enter");\n  if(caller_filter) {caller_filter->Restore();caller_filter->Release();caller_filter=nullptr;}')
(p/'caller-window.cpp').write_text(source,encoding='utf-8')
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
rc=(stage/'windows/runner/Runner.rc').read_text()
rc=rc.replace('"com.touhouonline" "\\0"','"com.touhouonline.diagnostics" "\\0"').replace('"touhouonline" "\\0"','"caller-20261128" "\\0"')
# Keep resources resolved against the original runner source, not the current directory.
rc=rc.replace('"resources\\\\app_icon.ico"','"'+str(stage/'windows/runner/resources/app_icon.ico').replace('\\','/')+'"')
(p/'caller.rc').write_text(rc,encoding='utf-8')
compile=(p/'thread-compile.rsp').read_text().replace('client-thread-window','caller-window')
(p/'caller-compile.rsp').write_text(compile)
link=(p/'thread-link.rsp').read_text().replace('client-thread-host.exe','caller-host.exe').replace('client-thread-window','caller-window').replace(r'TOUHOUONLINE.DIR\RELEASE\RUNNER.RES','"'+str(p/'caller.res')+'"')
(p/'caller-link.rsp').write_text(link)
cmd=(p/'build-thread.cmd').read_text().replace('cl @','rc /nologo /i "'+str(stage/'windows/runner')+'" /fo "'+str(p/'caller.res')+'" "'+str(p/'caller.rc')+'"\nif errorlevel 1 exit /b 1\ncl @').replace('thread-compile.rsp','caller-compile.rsp').replace('thread-link.rsp','caller-link.rsp')
(p/'build-caller.cmd').write_text(cmd)
run=(p/'run-thread.py').read_text().replace('thread-events','caller-events').replace('thread-stdout','caller-stdout').replace('thread-result','caller-result').replace('client-thread-host.exe','caller-host.exe')
(p/'run-caller.py').write_text(run)
print(json.dumps({'source':str(p/'caller-window.cpp'),'isolatedCompany':'com.touhouonline.diagnostics','isolatedProduct':'caller-20261128','aotModified':False,'productionSourceModified':False}))
