#include "BackgroundThread.hpp"
namespace th10 {
// 0x44c150. A timeout asks the worker to stop again before the next wait.
void BackgroundThread::stop(BackgroundThreadEnvironment& env){
    if(!handle)return;
    stop_requested=1;running=0;
    auto status=env.wait_thread(handle,200);
    while(status==0x102){stop_requested=1;running=0;env.sleep(1);status=env.wait_thread(handle,200);}
    env.close_thread(handle);handle=0;callback=0;
}
// 0x44c1c0 / 0x44c200. The argument is passed to the platform without storing
// it in the unused member, matching the resource workers in the original.
void BackgroundThread::start(CallbackToken entry,void* parameter,bool suspended,BackgroundThreadEnvironment& env){stop(env);callback=entry;running=1;stop_requested=0;handle=env.begin_thread(entry,parameter,suspended?4:0,id);}
}
