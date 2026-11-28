from pathlib import Path
import re
p=Path('D:/Temp/native-performance-20261128/client-info')
source=(p/'client-flutter-window.cpp').read_text(encoding='utf-8')
marker='namespace {\n'
extra=r'''
struct MessageCounts { unsigned long long count=0; };
std::map<std::pair<UINT,HWND>,MessageCounts> called_messages;
HHOOK message_hook=nullptr;
LRESULT CALLBACK CountThreadMessages(int code,WPARAM wp,LPARAM lp) {
  if(code>=0) {
    auto* event=reinterpret_cast<CWPSTRUCT*>(lp);
    ++called_messages[{event->message,event->hwnd}].count;
  }
  return CallNextHookEx(message_hook,code,wp,lp);
}
void WriteMessages(FILE* file) {
  fprintf(file,",\"threadCalledMessages\":[");
  bool first=true;
  for(const auto& entry:called_messages) {
    wchar_t name[256]={};GetClassNameW(entry.first.second,name,256);
    BSTR class_name=SysAllocString(name);
    auto text=Escape(class_name);SysFreeString(class_name);
    fprintf(file,"%s{\"message\":%u,\"hwnd\":%llu,\"class\":\"%s\",\"count\":%llu}",first?"":",",entry.first.first,reinterpret_cast<unsigned long long>(entry.first.second),text.c_str(),entry.second.count);
    first=false;
  }
  fprintf(file,"]");
}
'''
source=source.replace('#include <commctrl.h>','#include <commctrl.h>\n#include <map>')
where='FILE* OpenLog() {'
source=source.replace(where,extra+'\n'+where)
source=source.replace('fprintf(file,"}\\n");fclose(file);','WriteMessages(file);\n  fprintf(file,"}\\n");fclose(file);')
source=source.replace('  RECT frame = GetClientArea();','  SetWindowLongPtr(GetHandle(),GWL_EXSTYLE,GetWindowLongPtr(GetHandle(),GWL_EXSTYLE)|WS_EX_NOACTIVATE);\n  RECT frame = GetClientArea();')
source=source.replace('    Mark("first_frame");','    Mark("first_frame");\n    message_hook=SetWindowsHookEx(WH_CALLWNDPROC,CountThreadMessages,nullptr,GetCurrentThreadId());')
source=source.replace('  Mark("destroy_enter");','  Mark("destroy_enter");\n  if(message_hook) {UnhookWindowsHookEx(message_hook);message_hook=nullptr;}')
source=source.replace('client-info/host-events.jsonl','client-info/thread-events.jsonl')
(p/'client-thread-window.cpp').write_text(source,encoding='utf-8')
for filename in ('compile.rsp','link.rsp'):
    rsp=(p/filename).read_text().replace('client-flutter-window','client-thread-window').replace('preview/client-info-host.exe','preview/client-thread-host.exe')
    (p/('thread-'+filename)).write_text(rsp)
cmd=(p/'build-host.cmd').read_text().replace('/compile.rsp','/thread-compile.rsp').replace('/link.rsp','/thread-link.rsp').replace('\\compile.rsp','\\thread-compile.rsp').replace('\\link.rsp','\\thread-link.rsp')
(p/'build-thread.cmd').write_text(cmd)
runner=(p/'run-host.py').read_text().replace('client-info-host.exe','client-thread-host.exe').replace('host-events.jsonl','thread-events.jsonl').replace('host-stdout.log','thread-stdout.log').replace('host-result.json','thread-result.json')
(p/'run-thread.py').write_text(runner)
