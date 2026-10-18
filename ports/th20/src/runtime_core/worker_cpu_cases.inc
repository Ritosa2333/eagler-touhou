// Included by the isolated runtime oracle only. Original game entry is never run.
{
    for(unsigned fill=0;fill<256;++fill){
        alignas(rt::Worker) std::array<unsigned char,sizeof(rt::Worker)> native_bytes,source_bytes;
        native_bytes.fill(static_cast<unsigned char>(fill));source_bytes=native_bytes;
        original_call<void>(0xb780,native_bytes.data());
        auto* source_worker=::new(source_bytes.data())rt::Worker{};
        check("worker_constructor_all16bytes_with_tail_padding",native_bytes==source_bytes);
        // Empty std::jthread is a valid cross-toolset object without heap state.
        const unsigned op=fill%3;
        if(op==0){original_call<void>(0xbcf0,native_bytes.data());rt::join_worker(*source_worker);}
        else if(op==1){original_call<void>(0xba8b0,native_bytes.data());rt::detach_worker(*source_worker);}
        else{original_call<void>(0xba8b0,native_bytes.data());original_call<void>(0xbcf0,native_bytes.data());rt::sync_close_worker(*source_worker);}
        check("worker_idle_close_join_detach_all16bytes",native_bytes==source_bytes);
        original_call<void>(0xb980,native_bytes.data());source_worker->~Worker();
        check("worker_idle_destructor_all16bytes_with_tail_padding",native_bytes==source_bytes);
    }
    for(unsigned test=0;test<64;++test){
        rt::Worker native_worker,source_worker;
        std::atomic<bool> native_done{false},source_done{false};
        native_worker.thread=std::jthread([&]{while(!native_worker.close_requested.load(std::memory_order_seq_cst))std::this_thread::yield();native_done.store(true);});
        source_worker.thread=std::jthread([&]{while(!source_worker.close_requested.load(std::memory_order_seq_cst))std::this_thread::yield();source_done.store(true);});
        original_call<void>(0xbcf0,&native_worker);rt::join_worker(source_worker);
        check("worker_live_join_closes_and_waits_for_real_threads",native_worker.close_requested.load()&&source_worker.close_requested.load()&&native_done.load()&&source_done.load());
        check("worker_live_join_clears_native_thread_handles",!native_worker.thread.joinable()&&!source_worker.thread.joinable()&&std::memcmp(&native_worker,&source_worker,8)==0);
    }
    for(unsigned test=0;test<64;++test){
        rt::Worker native_worker,source_worker;
        std::atomic<bool> release{false};
        std::atomic<bool> native_saw_close{false},source_saw_close{false};
        HANDLE native_done=CreateEventW(nullptr,TRUE,FALSE,nullptr),source_done=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        if(!native_done||!source_done)throw std::runtime_error("Worker detach fixture event allocation failed");
        native_worker.thread=std::jthread([&]{while(!release.load())std::this_thread::yield();native_saw_close.store(native_worker.close_requested.load());SetEvent(native_done);});
        source_worker.thread=std::jthread([&]{while(!release.load())std::this_thread::yield();source_saw_close.store(source_worker.close_requested.load());SetEvent(source_done);});
        if(test%2){original_call<void>(0xba8b0,&native_worker);original_call<void>(0xbcf0,&native_worker);rt::sync_close_worker(source_worker);}
        else{original_call<void>(0xba8b0,&native_worker);rt::detach_worker(source_worker);}
        check("worker_live_detach_closes_without_waiting_for_thread_body",native_worker.close_requested.load()&&source_worker.close_requested.load()&&WaitForSingleObject(native_done,0)==WAIT_TIMEOUT&&WaitForSingleObject(source_done,0)==WAIT_TIMEOUT);
        check("worker_live_detach_clears_native_thread_handles",!native_worker.thread.joinable()&&!source_worker.thread.joinable()&&std::memcmp(&native_worker,&source_worker,8)==0);
        release.store(true);
        const HANDLE finished[]{native_done,source_done};
        if(WaitForMultipleObjects(2,finished,TRUE,10000)!=WAIT_OBJECT_0)throw std::runtime_error("Worker detached fixture threads failed to finish");
        check("worker_detached_threads_observe_close_flag",native_saw_close.load()&&source_saw_close.load());
        CloseHandle(native_done);CloseHandle(source_done);
    }
}
