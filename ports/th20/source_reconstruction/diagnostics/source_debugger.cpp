// Debug an independently compiled build. This utility never reads, maps or
// launches the original executable and is not linked into the game.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
namespace {
void stack_trace(HANDLE process,HANDLE thread,CONTEXT context,std::ofstream& out){
    STACKFRAME64 frame{};frame.AddrPC.Offset=context.Eip;frame.AddrStack.Offset=context.Esp;frame.AddrFrame.Offset=context.Ebp;
    frame.AddrPC.Mode=frame.AddrStack.Mode=frame.AddrFrame.Mode=AddrModeFlat;
    for(unsigned i=0;i<96;++i){
        if(i&&!StackWalk64(IMAGE_FILE_MACHINE_I386,process,thread,&frame,&context,nullptr,SymFunctionTableAccess64,SymGetModuleBase64,nullptr))break;
        if(!frame.AddrPC.Offset)break;
        out<<"  0x"<<std::hex<<frame.AddrPC.Offset;
        char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME]{};auto* symbol=reinterpret_cast<SYMBOL_INFO*>(buffer);symbol->SizeOfStruct=sizeof(SYMBOL_INFO);symbol->MaxNameLen=MAX_SYM_NAME;DWORD64 displacement=0;
        if(SymFromAddr(process,frame.AddrPC.Offset,&displacement,symbol))out<<" "<<symbol->Name<<"+0x"<<displacement;
        IMAGEHLP_LINE64 line{};line.SizeOfStruct=sizeof(line);DWORD line_displacement=0;
        if(SymGetLineFromAddr64(process,frame.AddrPC.Offset,&line_displacement,&line))out<<" "<<line.FileName<<":"<<std::dec<<line.LineNumber;
        out<<'\n';
    }
}
}
int wmain(int argc,wchar_t** argv){
    if(argc!=4){std::cerr<<"Usage: th20_source_debugger SOURCE_BUILD.exe ISOLATED_APPDATA OUTPUT_DIRECTORY\n";return 2;}
    const auto executable=std::filesystem::absolute(argv[1]);const auto userdata=std::filesystem::absolute(argv[2]);const auto output=std::filesystem::absolute(argv[3]);
    // An explicit guard against confusing the previous hybrid/original package
    // with this diagnostic target.
    if(executable.filename()!=L"th20_source.exe"&&executable.filename()!=L"th20_source_link_probe.exe"){std::cerr<<"Expected the independent source build filename\n";return 2;}
    std::filesystem::create_directories(userdata);std::filesystem::create_directories(output);SetEnvironmentVariableW(L"APPDATA",userdata.c_str());
    std::ofstream log(output/L"debugger.log",std::ios::trunc);STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION child{};
    auto command=L"\""+executable.wstring()+L"\"";
    if(!CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,DEBUG_ONLY_THIS_PROCESS,nullptr,executable.parent_path().c_str(),&startup,&child)){log<<"CreateProcess error="<<GetLastError()<<'\n';return 2;}
    DebugSetProcessKillOnExit(FALSE);SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES|SYMOPT_UNDNAME);SymInitialize(child.hProcess,executable.parent_path().string().c_str(),FALSE);
    std::cout<<"source_pid="<<child.dwProcessId<<std::endl;log<<"source_pid="<<child.dwProcessId<<'\n';bool initial_break=true;unsigned dumps=0;int result=0;
    const bool continue_intercepts=GetEnvironmentVariableW(L"ASAN_WIN_CONTINUE_ON_INTERCEPTION_FAILURE",nullptr,0)!=0;
    bool pending_interception_warning=false;
    for(;;){
        DEBUG_EVENT event{};if(!WaitForDebugEvent(&event,1000)){if(GetLastError()==ERROR_SEM_TIMEOUT)continue;log<<"WaitForDebugEvent error="<<GetLastError()<<'\n';result=2;break;}
        DWORD status=DBG_CONTINUE;
        switch(event.dwDebugEventCode){
        case CREATE_PROCESS_DEBUG_EVENT:{auto& p=event.u.CreateProcessInfo;SymLoadModuleEx(child.hProcess,p.hFile,executable.string().c_str(),nullptr,reinterpret_cast<DWORD64>(p.lpBaseOfImage),0,nullptr,0);if(p.hFile)CloseHandle(p.hFile);break;}
        case LOAD_DLL_DEBUG_EVENT:{auto& p=event.u.LoadDll;SymLoadModuleEx(child.hProcess,p.hFile,nullptr,nullptr,reinterpret_cast<DWORD64>(p.lpBaseOfDll),0,nullptr,0);if(p.hFile)CloseHandle(p.hFile);break;}
        case UNLOAD_DLL_DEBUG_EVENT:SymUnloadModule64(child.hProcess,reinterpret_cast<DWORD64>(event.u.UnloadDll.lpBaseOfDll));break;
        case CREATE_THREAD_DEBUG_EVENT:if(event.u.CreateThread.hThread)CloseHandle(event.u.CreateThread.hThread);break;
        case OUTPUT_DEBUG_STRING_EVENT:{auto& text=event.u.DebugString;std::vector<char> data(text.nDebugStringLength*(text.fUnicode?2:1)+2);SIZE_T read=0;if(ReadProcessMemory(child.hProcess,text.lpDebugStringData,data.data(),data.size()-2,&read)&&!text.fUnicode){log<<"debug: "<<data.data()<<'\n';if(continue_intercepts&&std::string(data.data()).find("interception failed for a targeted function")!=std::string::npos)pending_interception_warning=true;}break;}
        case EXCEPTION_DEBUG_EVENT:{const auto& exception=event.u.Exception;const auto code=exception.ExceptionRecord.ExceptionCode;
            if(code==EXCEPTION_BREAKPOINT&&initial_break){initial_break=false;break;}
            if(code==EXCEPTION_BREAKPOINT&&exception.dwFirstChance&&pending_interception_warning){
                log<<"Explicit ASAN interception compatibility probe: continuing its announced diagnostic breakpoint; this run is not a complete sanitizer validation.\n";
                pending_interception_warning=false;break;
            }
            status=DBG_EXCEPTION_NOT_HANDLED;
            if(!exception.dwFirstChance){
                log<<"unhandled exception=0x"<<std::hex<<code<<" at="<<exception.ExceptionRecord.ExceptionAddress<<" thread="<<std::dec<<event.dwThreadId<<'\n';
                for(unsigned i=0;i<exception.ExceptionRecord.NumberParameters;++i)log<<"  parameter["<<i<<"]=0x"<<std::hex<<exception.ExceptionRecord.ExceptionInformation[i]<<'\n';
                HANDLE thread=OpenThread(THREAD_GET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,event.dwThreadId);CONTEXT context{};context.ContextFlags=CONTEXT_FULL;if(thread&&GetThreadContext(thread,&context))stack_trace(child.hProcess,thread,context,log);if(thread)CloseHandle(thread);
                const auto dump=output/(L"source_crash_"+std::to_wstring(++dumps)+L".dmp");HANDLE file=CreateFileW(dump.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);if(file!=INVALID_HANDLE_VALUE){MiniDumpWriteDump(child.hProcess,child.dwProcessId,file,MiniDumpWithIndirectlyReferencedMemory,nullptr,nullptr,nullptr);CloseHandle(file);}log.flush();
            }break;}
        case EXIT_PROCESS_DEBUG_EVENT:result=static_cast<int>(event.u.ExitProcess.dwExitCode);log<<"exit_code=0x"<<std::hex<<event.u.ExitProcess.dwExitCode<<'\n';ContinueDebugEvent(event.dwProcessId,event.dwThreadId,status);goto finished;
        }
        ContinueDebugEvent(event.dwProcessId,event.dwThreadId,status);log.flush();
    }
finished:SymCleanup(child.hProcess);CloseHandle(child.hThread);CloseHandle(child.hProcess);return result;
}
