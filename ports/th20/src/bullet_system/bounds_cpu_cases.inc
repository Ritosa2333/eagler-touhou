{
    std::ofstream bounds_failures(std::filesystem::path(argv[2]).parent_path().parent_path()/"bullet_system/bounds_failures.jsonl");
    s::SpriteData descriptor{};descriptor.extent_48=16;descriptor.extent_4c=12;file.sprites=&descriptor;
    for(unsigned operation=0;operation<2;++operation){reset(0,1);auto& b=owner.pool[0];b.animation->base.fields_10_28[3]=7;b.animation->base.fields_10_28[4]=0;
        for(unsigned test=0;test<8192;++test){diagnostic_case=test;diagnostic_phase=operation?"Bullet offscreen delay geometry":"Bullet screen bounce geometry";
            b.flags=rng();b.field_90=(static_cast<std::uint64_t>(rng())<<32)|rng();b.position={finite()*320,224+finite()*400,finite()};b.velocity={finite()*8,finite()*8,finite()*8};b.angle=finite()*20;b.field_20=finite()*8;b.field_40=test%11?0xffffffffu:std::uint32_t(test%90);
            const float widths[]{0,-1,256,384,512},heights[]{0,-1,256,448,600};owner.vector_4c={widths[test%5],heights[(test/5)%5]};
            auto& command=b.commands[operation?11:4];command.field_10=test%3?finite()*8:-1000;command.vector_18={256+finite()*128,448+finite()*128,0};command.field_3c=test%32;command.field_30=operation?test%2:test%4;command.field_34=1+test%4;const int elapsed=int(test%9)-4;command.timer={elapsed-1,elapsed,float(elapsed)+.25f,rng()};
            if(test%17==0)b.position.x=192;if(test%19==0)b.position.x=-192;if(test%23==0)b.position.y=448;if(test%29==0)b.position.y=0;
            // The old CRT's NaN sqrt error callback cannot be exercised in
            // this isolated image (see unavailable_nan_probe.json). Bounce
            // retains NaN samples because its tested paths do not call sqrt.
            if(operation==0&&test%101==0){const std::uint32_t nan=0x7fc12345;std::memcpy(test%202?&b.position.x:&b.position.y,&nan,4);}
            clock_value=float(test%7)*.25f;anm_clock_pointer=&clock_value;state::clock_scale=clock_value;state::timer_rate=test%31?&clock_value:nullptr;
            *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=clock_value;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=state::timer_rate;sound_reset();
            const auto initial=b;const auto initial_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry),reinterpret_cast<std::uint8_t*>(&pe_state::thread_registry)+0x57e8);
            FloatingEnvironment::prepare();const int expected_return=cpu<int>(operation?0x481200:0x482a60,&b);const auto expected=b;const auto expected_sound=std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830),reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1ba830)+0x57e8);
            b=initial;std::memcpy(&pe_state::thread_registry,initial_sound.data(),initial_sound.size());FloatingEnvironment::prepare();const int actual_return=operation?bu::unrecovered::update_offscreen_delay_00481200(b):bu::unrecovered::update_bounce_00482a60(b);
            if(std::memcmp(&expected,&b,sizeof(b))||expected_return!=actual_return){
                bounds_failures<<"{\"operation\":"<<operation<<",\"case\":"<<test<<",\"expected_return\":"<<expected_return<<",\"actual_return\":"<<actual_return<<",\"bounds_bits\":[";
                std::uint32_t bounds_words[2];std::memcpy(bounds_words,&owner.vector_4c,8);bounds_failures<<bounds_words[0]<<','<<bounds_words[1]<<']';
                auto words=[&](const char* name,const bu::Bullet& value){bounds_failures<<",\""<<name<<"\":[";for(unsigned i=0;i<sizeof(value)/4;++i){std::uint32_t word;std::memcpy(&word,reinterpret_cast<const std::uint8_t*>(&value)+i*4,4);if(i)bounds_failures<<',';bounds_failures<<word;}bounds_failures<<']';};
                words("initial_words",initial);words("expected_words",expected);words("actual_words",b);bounds_failures<<"}\n";
            }
            const auto label=operation?"bullet_offscreen_delay":"bullet_bounce";check(label,&expected,&b,sizeof(b));check(std::string(label)+"_return",&expected_return,&actual_return,4);check(std::string(label)+"_sound",expected_sound.data(),&pe_state::thread_registry,expected_sound.size());
        }
    }
    file.sprites=nullptr;
}
