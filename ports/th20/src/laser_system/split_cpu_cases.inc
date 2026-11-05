// Included in the live Type0 ANM-file fixture. All surviving intervals are real
// independently allocated Type0 objects on both sides, never a spawn callback.
{
    struct CapturedType0 {
        std::uintptr_t address;
        unsigned kind;
        std::vector<std::uint8_t> bytes;
        la::Segment segment;
        std::vector<bu::Command> commands;
        std::size_t capacity;
    };
    auto capture_children=[&](unsigned kind=0){std::vector<CapturedType0> out;
        for(auto* link=owner.active.sentinel.next;link;link=link->next){
            if(out.size()>=64)unexpected("split fixture child list cycle or excessive output");
            auto* child=reinterpret_cast<la::Laser*>(link->value);
            if(!child->allocated_6d8)unexpected("split fixture missing owned Segment");
            const auto& commands=kind?static_cast<la::Type1Laser*>(child)->parameters.commands:static_cast<la::Type0Laser*>(child)->parameters.commands;const auto size=kind?sizeof(la::Type1Laser):sizeof(la::Type0Laser);const auto* bytes=reinterpret_cast<std::uint8_t*>(child);
            out.push_back({reinterpret_cast<std::uintptr_t>(child),kind,std::vector<std::uint8_t>(bytes,bytes+size),*static_cast<la::Segment*>(child->allocated_6d8),std::vector<bu::Command>(commands.begin(),commands.end()),commands.capacity()});
        }return out;
    };
    auto compare_children=[&](const std::string& label,std::array<std::uint8_t,sizeof(owner)> expected_owner,std::vector<CapturedType0> expected,const std::vector<CapturedType0>& actual){
        const auto expected_count=expected.size(),actual_count=actual.size();check(label+"_child_count",&expected_count,&actual_count,sizeof(actual_count));if(expected_count!=actual_count)return;
        auto relocated=[&](std::uintptr_t pointer){for(std::size_t i=0;i<expected.size();++i){if(pointer==expected[i].address)return actual[i].address;if(pointer==expected[i].address+offsetof(la::Laser,link))return actual[i].address+offsetof(la::Laser,link);}return pointer;};
        auto relocate_link=[&](q::Link& link){link.value=reinterpret_cast<q::Node*>(relocated(reinterpret_cast<std::uintptr_t>(link.value)));link.next=reinterpret_cast<q::Link*>(relocated(reinterpret_cast<std::uintptr_t>(link.next)));link.previous=reinterpret_cast<q::Link*>(relocated(reinterpret_cast<std::uintptr_t>(link.previous)));};
        auto& normalized_owner=*reinterpret_cast<la::Controller*>(expected_owner.data());relocate_link(normalized_owner.active.sentinel);normalized_owner.active.tail=reinterpret_cast<q::Link*>(relocated(reinterpret_cast<std::uintptr_t>(normalized_owner.active.tail)));check(label+"_owner",expected_owner.data(),&owner,sizeof(owner));
        for(std::size_t i=0;i<expected.size();++i){auto& e=expected[i];const auto& a=actual[i];auto& beam=*reinterpret_cast<la::Laser*>(e.bytes.data());const auto& source=*reinterpret_cast<const la::Laser*>(a.bytes.data());
            // Normalize only verified ABI-dependent or independently allocated
            // pointer fields; all scalar fields and list topology remain exact.
            const auto native_vptr=*reinterpret_cast<std::uintptr_t*>(e.bytes.data());const std::uintptr_t required_vptr=mapped_image_base+(e.kind?0x1712e8:0x171148);check(label+"_original_vtable",&required_vptr,&native_vptr,4);if(native_vptr==required_vptr)std::memcpy(e.bytes.data(),a.bytes.data(),4);
            relocate_link(beam.link);beam.allocated_6d8=source.allocated_6d8;
            const auto commands_offset=e.kind?offsetof(la::Type1Laser,parameters)+offsetof(la::Type1Parameters,commands):offsetof(la::Type0Laser,parameters)+offsetof(la::Type0Parameters,commands);std::uint32_t expected_data,actual_data;std::memcpy(&expected_data,e.bytes.data()+commands_offset+4,4);std::memcpy(&actual_data,a.bytes.data()+commands_offset+4,4);const auto delta=actual_data-expected_data;
            for(unsigned offset:{4u,8u,12u}){std::uint32_t pointer;std::memcpy(&pointer,e.bytes.data()+commands_offset+offset,4);pointer+=delta;std::memcpy(e.bytes.data()+commands_offset+offset,&pointer,4);}
            std::vector<unsigned> animation_offsets=e.kind?std::vector<unsigned>{offsetof(la::Type1Laser,animation),offsetof(la::Type1Laser,origin_animation)}:std::vector<unsigned>{offsetof(la::Type0Laser,animation),offsetof(la::Type0Laser,origin_animation),offsetof(la::Type0Laser,tip_animation)};
            for(unsigned offset:animation_offsets){auto& animation=*reinterpret_cast<s::Animation*>(e.bytes.data()+offset);const auto old_animation=e.address+offset,new_animation=a.address+offset;
                for(auto& link:animation.links)if(reinterpret_cast<std::uintptr_t>(link.value)==old_animation)link.value=reinterpret_cast<s::Animation*>(new_animation);
                if(animation.field_5c8==e.address)animation.field_5c8=a.address;if(animation.field_5e0==mapped_image_base+0xcf550)animation.field_5e0=reinterpret_cast<std::uintptr_t>(&la::remap_sprite);
            }
            check(label+"_child",e.bytes.data(),a.bytes.data(),e.bytes.size());check(label+"_segment",&e.segment,&a.segment,sizeof(e.segment));const auto es=e.commands.size(),as=a.commands.size();check(label+"_command_count",&es,&as,sizeof(as));check(label+"_command_capacity",&e.capacity,&a.capacity,sizeof(a.capacity));if(es==as)check(label+"_commands",e.commands.data(),a.commands.data(),as*sizeof(bu::Command));
        }
    };
    la::Type1Laser type1;type1.select_context(0);
    la::Type0Laser type0;type0.select_context(0);type0.parameters.commands.resize(8);
    for(unsigned kind=0;kind<2;++kind)for(unsigned test=0;test<512;++test){diagnostic_case=test;diagnostic_phase="Laser actual split into owned Type0 objects";q::initialize_list(owner.active);owner.count=0;owner.next_handle=test%3?0x10000+test:0x7ffffffc;
        std::uint8_t mask[32]{};const int count=int(test%33);for(int i=0;i<count;++i)mask[i]=(test%8==0?0:test%8==1?1:static_cast<std::uint8_t>(rng()%2));
        auto& parent=kind?static_cast<la::Laser&>(type0):static_cast<la::Laser&>(type1);parent.position={finite()*192,finite()*448,finite()};parent.angle=finite()*24;parent.field_74=float(count*16)+finite()*8;parent.speed=finite()*32;parent.flags=rng();parent.select_context(test%2);
        type1.parameters.type=type0.parameters.type=test%50;type1.parameters.color=type0.parameters.color=test%16;type1.parameters.flags=type0.parameters.flags=rng();type1.parameters.length_limit=type0.parameters.length_limit=finite()*768;type0.parameters.angle=finite()*24;type0.parameters.width=finite()*32;type0.parameters.speed=finite()*8;type0.parameters.length=parent.field_74+finite()*16;type0.parameters.field_14=finite()*512;type0.parameters.position=parent.position;type0.parameters.radial_offset=finite()*4;type0.parameters.field_48=type0.parameters.field_4c=-1;type0.parameters.view_index=test%2;type0.parameters.command_index=rng();for(auto& command:type0.parameters.commands)for(auto& word:command.words)word=rng();
        const auto initial_owner=*reinterpret_cast<std::array<std::uint8_t,sizeof(owner)>*>(&owner);const auto initial_type1=*reinterpret_cast<std::array<std::uint8_t,sizeof(type1)>*>(&type1);const auto initial_type0=*reinterpret_cast<std::array<std::uint8_t,sizeof(type0)>*>(&type0);c.field_b8=test;const auto initial_counter=c.field_b8;
        FloatingEnvironment::prepare();if(kind)cpu<void>(0x4ce510,&type0,mask,count);else cpu<void>(0x4ce220,&type1,mask,count);auto expected_children=capture_children();const auto expected_owner=*reinterpret_cast<std::array<std::uint8_t,sizeof(owner)>*>(&owner);const auto expected_type1=*reinterpret_cast<std::array<std::uint8_t,sizeof(type1)>*>(&type1);const auto expected_type0=*reinterpret_cast<std::array<std::uint8_t,sizeof(type0)>*>(&type0);const auto expected_counter=c.field_b8;cpu<void>(0x4bc480,&owner);
        std::memcpy(&owner,initial_owner.data(),sizeof(owner));std::memcpy(&type1,initial_type1.data(),sizeof(type1));std::memcpy(&type0,initial_type0.data(),sizeof(type0));c.field_b8=initial_counter;FloatingEnvironment::prepare();if(kind)type0.split(mask,count);else type1.split(mask,count);const auto actual_children=capture_children();const std::string label=kind?"laser_type0_split":"laser_type1_split";
        check(label+"_parent",kind?expected_type0.data():expected_type1.data(),&parent,kind?sizeof(type0):sizeof(type1));check(label+"_animation_counter",&expected_counter,&c.field_b8,4);compare_children(label,expected_owner,std::move(expected_children),actual_children);owner.clear();
    }
    #include "../bullet_system/laser_spawn_cpu_cases.inc"
    #include "shape_cancel_cpu_cases.inc"
}
