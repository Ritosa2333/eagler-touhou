{
    static unsigned diagnostic_case=0,diagnostic_frame=0;static const char* diagnostic_phase="scalar";
    const auto exception_observer=AddVectoredExceptionHandler(1,[](EXCEPTION_POINTERS* info)->LONG{std::cerr<<"Enemy VM native exception "<<std::hex<<info->ExceptionRecord->ExceptionCode<<" at "<<info->ContextRecord->Eip<<" mapped "<<mapped_image_base<<" ecx "<<info->ContextRecord->Ecx<<" access "<<info->ExceptionRecord->ExceptionInformation[1]<<std::dec<<" case "<<diagnostic_case<<" frame "<<diagnostic_frame<<" phase "<<diagnostic_phase<<'\n';return EXCEPTION_CONTINUE_SEARCH;});
    namespace e=th20::source::ecl;
    using enemy_vm_test::instruction;using enemy_vm_test::append;using enemy_vm_test::put32;
    enemy_vm_test::NoEngine engine;
    const auto record_check=check;
    for(const auto& entry:th20::parse_pe(bytes).imports)if(entry.name=="GetCurrentThreadId"||entry.name=="AcquireSRWLockExclusive"||entry.name=="ReleaseSRWLockExclusive"||entry.name=="HeapAlloc"||entry.name=="HeapFree"||entry.name=="GetLastError"){
        const auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),entry.name.c_str());if(!address)throw std::runtime_error("Missing native VM heap/lock import");
        *reinterpret_cast<std::uintptr_t*>(mapped_image_base+entry.iat_rva)=reinterpret_cast<std::uintptr_t>(address);
    }
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();original<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    const auto vm_check=[&](const std::string& label,e::Program& program,const std::function<void(e::Runtime&)>& prepare,float delta=1.f){
        e::Scheduler prepared(program,engine);auto& input=prepared.main;input.subroutine=input.instruction_offset=0;input.stack.words.resize(1024);prepare(input);
        enemy_vm_test::State cpu(environment,input),source(environment,input);
        *reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=input.clock_rate;
        auto* random=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1ba4a8);
        if(input.random){random[1]=input.random->state;random[4]=input.random->maximum;random[5]=input.random->last;}
        gp::EnemyVmEnvironment vm_environment{input.random,input.clock_rate};
        FloatingEnvironment::prepare();const auto before=original<int>(0x53b5c0,&cpu.manager.main,delta);
        FloatingEnvironment::prepare();const auto after=source.manager.main.tick(delta,vm_environment);
        bool same=before==after&&enemy_vm_test::same_runtime(cpu.manager.main,source.manager.main)&&cpu.code==source.code;
        if(input.random)same=same&&random[1]==input.random->state&&random[5]==input.random->last;
        record_check(("EnemyVM_"+label).c_str(),same);
    };
    const auto test=[&](unsigned op,const std::vector<std::uint32_t>& args,unsigned mask,const std::function<void(e::Runtime&)>& prepare,unsigned drop=0,unsigned rank=255){
        auto code=instruction(op,args,mask,rank,0,drop);append(code,instruction(0,{},0,255,1000000));e::Program program;program.subroutines.push_back({"test",code});vm_check("opcode_"+std::to_string(op),program,prepare);
    };
        for(unsigned op=50;op<=77;++op) for(unsigned i=0;i<250;++i) {
            const bool floating=(op<59?(op%2==1):op<=70?(op%2==0):op==72);
            auto a=rng(),b=rng();if((op==56||op==58)&&(!b||(a==0x80000000&&b==0xffffffff)))b=1;
            // Use all raw float bit patterns; NaNs and signed zero are preserved.
            test(op,{},0,[&](e::Runtime& r){r.stack.push(a,floating?'f':'i');if(op!=71&&op!=72)r.stack.push(b,floating?'f':'i');},i%9);
        }
        for(unsigned i=0;i<300;++i){
            const auto a=rng();const auto f=static_cast<float>(static_cast<int>(rng()%100000)-50000)/32.f;
            test(42,{a},0,[](e::Runtime&){});test(44,{e::float_bits(f)},0,[](e::Runtime&){});
            test(42,{0},1,[&](e::Runtime& r){r.stack.frame_base=20;r.stack.absolute(20)=a;});
            test(44,{0},1,[&](e::Runtime& r){r.stack.frame_base=20;r.stack.absolute(20)=e::float_bits(f);});
            test(42,{0xffffffff},1,[&](e::Runtime& r){r.stack.push(e::float_bits(f),'f');});
            test(44,{e::float_bits(-1.f)},1,[&](e::Runtime& r){r.stack.push(a,'i');});
            test(43,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.push(e::float_bits(f),'f');});
            test(45,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.push(a,'i');});
            test(78,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=a;});
            test(83,{},0,[&](e::Runtime& r){r.stack.push(a,'i');});test(84,{},0,[&](e::Runtime& r){r.stack.push(a,'f');});
            test(23,{i%100},0,[](e::Runtime&){});test(24,{e::float_bits(f)},0,[](e::Runtime&){});
            test(40,{i%40*4},0,[](e::Runtime& r){r.stack.push(0x11223344);});
            test(41,{},0,[&](e::Runtime& r){r.stack.enter_frame(i%40*4);});
            test(46,{0,1,a,0,a^0xdeadbeef,0},1,[&](e::Runtime& r){r.stack.frame_base=64;});
            test(47,{0,1,a,0,e::float_bits(f),0},1,[&](e::Runtime& r){r.stack.frame_base=64;});
            test(85,{0,e::float_bits(f),e::float_bits(f/4)},1,[&](e::Runtime& r){r.stack.frame_base=64;});
        }
        for(unsigned op:{0u,1u,22u,30u,31u})for(unsigned i=0;i<20;++i)
            test(op,{},0,[](e::Runtime& r){r.stack.pointer=32;},i%16,i%2?255:0);
        for(unsigned op:{12u,13u,14u})for(unsigned value:{0u,1u,0xffffffffu}) {
            auto code=instruction(op,{40,17});append(code,instruction(1));append(code,instruction(0,{},0,255,1000000));
            e::Program program;program.subroutines.push_back({"jump",code});
            vm_check("opcode_"+std::to_string(op),program,[&](e::Runtime& r){if(op!=12)r.stack.push(value,'i');});
        }
        // Non-consuming typed stack references use top-relative 8-byte slots.
        for(unsigned i=0;i<200;++i)test(23,{0xfffffffe},1,[&](e::Runtime& r){r.stack.push(i,'i');r.stack.push(e::float_bits(99.f),'f');});
        for(unsigned variant=0;variant<4;++variant)for(unsigned i=0;i<100;++i){
            auto call=instruction(11,{4,static_cast<unsigned>('z'),variant%2?0x6966u:0x6669u,variant%2?e::float_bits(12.75f):i},variant>=2?2:0);
            call[11]=2;
            auto caller=call;append(caller,instruction(0,{},0,255,1000000));
            auto callee=instruction(40,{4});append(callee,instruction(10));
            e::Program program;program.subroutines={{"a",caller},{"z",callee}};
            vm_check("synchronous_call_return",program,[&](e::Runtime& r){r.stack.enter_frame(8);if(variant>=2){
                // Numeric -1 means typed stack top in the source argument domain.
                put32(r.program->subroutines[0].bytes,28,variant%2?e::float_bits(-1.f):0xffffffffu);
                r.stack.push(variant%2?e::float_bits(12.75f):i,variant%2?'f':'i');
            }});
        }
        // A call from an empty stack returns through the -1 sentinel frame.
        {
            auto call=instruction(11,{4,static_cast<unsigned>('z')});call[11]=1;
            e::Program program;program.subroutines={{"a",call},{"z",instruction(40,{0})}};
            append(program.subroutines[1].bytes,instruction(10));
            vm_check("root_call_return",program,[](e::Runtime&){});
        }
        for(unsigned i=0;i<500;++i){
            const auto a=static_cast<float>(static_cast<int>(rng()%100000)-50000)/1000.f;
            const auto b=static_cast<float>(static_cast<int>(rng()%100000)-50000)/1000.f;
            for(unsigned op:{79u,80u,88u})test(op,{},0,[&](e::Runtime& r){r.stack.push(e::float_bits(op==88?std::abs(a):a),'f');});
            test(81,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(82,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=e::float_bits(a);});
            test(86,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(87,{0,e::float_bits(a),e::float_bits(b),e::float_bits(b),e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(89,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(90,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b),e::float_bits(a)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(94,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b),e::float_bits(b),e::float_bits(2)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(95,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(96,{0,e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(97,{0,e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
        }
        for(unsigned mode=0;mode<34;++mode)for(unsigned i=0;i<25;++i){
            const auto begin=static_cast<float>(static_cast<int>(rng()%10000)-5000)/32.f;
            const auto end=static_cast<float>(static_cast<int>(rng()%10000)-5000)/32.f;
            test(91,{0,e::float_bits(64),i+1,mode,e::float_bits(begin),e::float_bits(end)},2,[](e::Runtime& r){r.interpolators.resize(1);});
            test(92,{0,e::float_bits(64),i+1,mode,e::float_bits(begin),e::float_bits(end),e::float_bits(4),e::float_bits(-2)},2,[](e::Runtime& r){r.interpolators.resize(1);});
        }
        for(unsigned op:{17u,18u,19u,20u,21u})for(unsigned id:{0u,777u})for(unsigned i=0;i<30;++i)
            test(op,{id,i},0,[&](e::Runtime& r){r.async_flags=op==19?1:0;});
        const std::uint32_t edge_values[]={0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345};
        for(auto bits:edge_values){
            for(unsigned op:{51u,53u,55u,57u,60u,62u,64u,66u,68u,70u})for(auto second:edge_values)
                test(op,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');r.stack.push(second,'f');});
            for(unsigned op:{82u,96u,97u})test(op,op==82?std::vector<std::uint32_t>{0}:std::vector<std::uint32_t>{0,bits},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=bits;});
            // Avoid domain-error cases that need original CRT thread-local errno.
            if((bits&0x7fffffff)<0x7f800000){for(unsigned op:{79u,80u})test(op,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');});}
            if((bits&0x80000000)==0&&(bits&0x7fffffff)<=0x7f800000)test(88,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');});
            // NaN CRT transcendental calls enter original CRT thread-local error
            // handling, which this isolated (uninitialized CRT) oracle cannot run.
        }
        for(unsigned i=0;i<1000;++i){
            std::uint32_t state=rng(),last=0,maximum=i%3==0?0x7fffffffu:i%3==1?0xffffffffu:0x10000u;
            std::recursive_mutex mutex;e::RandomStream random{state,last,maximum,mutex};
            test(93,{0,e::float_bits(4),e::float_bits(3),e::float_bits(10)},3,[&](e::Runtime& r){r.random=&random;r.stack.frame_base=64;});
        }
    for(unsigned n=0;n<800;++n){
        diagnostic_case=n;diagnostic_phase="direct call setup";
        const unsigned skip=n&1u;const bool missing=n%7==0;
        std::vector<std::uint32_t> args{4,unsigned(missing?'x':'z')};if(skip)args.push_back(7);
        args.push_back(n%2?0x6966:0x6669);args.push_back(n%2?e::float_bits(-1.f):0xffffffffu);
        auto call=instruction(skip?16:15,args,1u<<(skip+1));call[11]=std::uint8_t(skip+2);
        e::Program program;program.subroutines={{"a",call},{"z",instruction(0,{},0,255,100)}};e::Scheduler prepared(program,engine);auto& input=prepared.main;
        input.subroutine=input.instruction_offset=0;input.stack.words.resize(1024);input.stack.enter_frame(8);input.stack.push(n%2?e::float_bits(12.75f):n,n%2?'f':'i');
        enemy_vm_test::State cpu(environment,input),source(environment,input);gp::EnemyRuntime cpu_target,source_target;
        cpu_target.manager=&cpu.manager;source_target.manager=&source.manager;cpu_target.stack.words.resize(1024);source_target.stack.words.resize(1024);
        FloatingEnvironment::prepare();const auto before=original<int>(0x53f3b0,&cpu.manager.main,&cpu_target,int(skip),0);
        FloatingEnvironment::prepare();const auto after=source.manager.main.call_into(source.manager.main.current(),source_target,int(skip));
        record_check("EnemyVM_native_direct_call_argument_conversion_and_missing_lookup",(before==0)==after&&enemy_vm_test::same_runtime(cpu.manager.main,source.manager.main)&&enemy_vm_test::same_runtime(cpu_target,source_target));
        record_check("EnemyVM_native_direct_call_current_restored_only_on_success",(cpu.manager.current==&cpu_target)==(source.manager.current==&source_target)&&(cpu.manager.current==&cpu.manager.main)==(source.manager.current==&source.manager.main));
    }
    for(unsigned n=0;n<400;++n){
        diagnostic_case=n;
        auto main_code=instruction(40,{8});auto first=instruction(15,{4,unsigned('b'),0x6969,n});first[11]=2;
        auto second=instruction(16,{4,unsigned('c'),7,0x6966,n+100});second[11]=3;
        append(main_code,first);append(main_code,second);append(main_code,instruction(23,{1}));
        if(n%4==1)append(main_code,instruction(21));
        if(n%4==2)append(main_code,instruction(17,{7}));
        if(n%4==3){append(main_code,instruction(18,{7}));append(main_code,instruction(20,{7,n}));}
        append(main_code,instruction(0,{},0,255,100));append(main_code,instruction(1));
        auto child=instruction(40,{4});append(child,instruction(42,{n*3}));append(child,instruction(43,{0},1));append(child,instruction(23,{2}));append(child,instruction(10));
        e::Program program;program.subroutines={{"a",main_code},{"b",child},{"c",child}};e::Scheduler prepared(program,engine);auto& input=prepared.main;input.subroutine=input.instruction_offset=0;input.rank_mask=n%3?255:0;
        enemy_vm_test::State cpu(environment,input),source(environment,input);gp::EnemyVmEnvironment vm_environment{nullptr,nullptr};
        *reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=nullptr;
        for(unsigned vm_frame=0;vm_frame<6;++vm_frame){
            diagnostic_frame=vm_frame;diagnostic_phase="CPU tick";
            FloatingEnvironment::prepare();const int before=original<int>(0x53e2b0,&cpu.manager,1.f);
            diagnostic_phase="source tick";
            FloatingEnvironment::prepare();const int after=source.manager.tick(1.f,vm_environment);
            diagnostic_phase="compare";
            bool same=before==after&&cpu.code==source.code;auto* a=&cpu.manager.runtimes;auto* b=&source.manager.runtimes;
            int cpu_current=-1,source_current=-1,index=0;
            while(a&&b){auto* ar=reinterpret_cast<gp::EnemyRuntime*>(a->value);auto* br=reinterpret_cast<gp::EnemyRuntime*>(b->value);
                same=same&&enemy_vm_test::same_runtime(*ar,*br)&&(a->next!=nullptr)==(b->next!=nullptr)&&(a->previous!=nullptr)==(b->previous!=nullptr);
                if(cpu.manager.current==ar)cpu_current=index;if(source.manager.current==br)source_current=index;
                a=a->next;b=b->next;++index;
            }
            record_check("EnemyVM_native_async_full_allocation_traversal_return_and_runtime_storage",same&&!a&&!b&&cpu_current==source_current);
            for(int id:{-1,0,7,999}){
                auto* cpu_link=original<scheduler::Link*>(0x53e920,&cpu.manager,id);auto* source_task=source.manager.find_runtime(id);
                record_check("EnemyVM_native_find_async_including_main",(cpu_link==nullptr)==(source_task==nullptr)&&(!cpu_link||enemy_vm_test::same_runtime(*reinterpret_cast<gp::EnemyRuntime*>(cpu_link->value),*source_task)));
            }
        }
        diagnostic_phase="CPU clear";original<void>(0x4973c0,&cpu.manager);cpu.manager.runtimes.next=nullptr;diagnostic_phase="scope destructors";
    }
    RemoveVectoredExceptionHandler(exception_observer);
}
