// These tests call the complete original47d240 and505e40 bodies. Only their
// externally owned final Bullet cancellation/Shot initialization are observed.
std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x7d240),saved_filtered,5);std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x105e40),saved_firing,5);FlushInstructionCache(GetCurrentProcess(),nullptr,0);
hook(0x47c8f0,reinterpret_cast<void*>(&cancel_one_boundary));hook(0x505d90,reinterpret_cast<void*>(&create_shot_boundary));hook(0x5060a0,reinterpret_cast<void*>(&option_boundary));
{
 std::vector<unsigned char> owner_memory(sizeof(bs::Controller));auto& owner=*reinterpret_cast<bs::Controller*>(owner_memory.data());std::array<bs::Bullet,16> bullet_memory;
 for(unsigned test=0;test<2048;++test){
  th20::source::scheduler::initialize_list(owner.active);owner.cancel_counter=random();
  for(unsigned index=0;index<bullet_memory.size();++index){auto& b=bullet_memory[index];fill(b);std::memset(&b.metadata,0,sizeof(b.metadata));b.index=index;b.state=random()%6;b.field_18=test%2?(random()%2):0;b.position={float(int(random()%128)-64),float(int(random()%128)-64),float(index)};b.size.x=float(random()%60);th20::source::scheduler::initialize_link(b.link,reinterpret_cast<th20::source::scheduler::Node*>(&b));th20::source::scheduler::append(owner.active,b.link);}
  const auto before_bullets=bullet_memory;const unsigned before_counter=owner.cancel_counter;const th20::source::sprite::Vec3 center{float(int(test%50)-25),float(int(test%80)-40),0};const float radius=float(test%100);
  auto predicate=[&](const th20::source::sprite::Vec3& p){extra_trace.insert(extra_trace.end(),{0xff,fbits(p.x),fbits(p.y),fbits(p.z)});return int(p.x>=0||test%3==0);};
  std::function<int(const th20::source::sprite::Vec3&)> accept=predicate;extra_trace.clear();const int er=cpu<int>(0x47d240,&owner,&center,radius,accept);const auto expected_bullets=bullet_memory;const auto expected_counter=owner.cancel_counter;const auto expected_trace=extra_trace;bullet_memory=before_bullets;owner.cancel_counter=before_counter;extra_trace.clear();const int ar=bs::cancel_filtered_circle(owner,center,radius,predicate);check("filtered_circle_return",test,&er,&ar,4);check("filtered_circle_counter",test,&expected_counter,&owner.cancel_counter,4);check("filtered_circle_entities",test,expected_bullets.data(),bullet_memory.data(),sizeof(bullet_memory));auto ec=expected_trace.size(),ac=extra_trace.size();check("filtered_circle_trace_size",test,&ec,&ac,4);if(ec==ac)check("filtered_circle_trace",test,expected_trace.data(),extra_trace.data(),ec*4);
 }
}
{
 FiringBoundaryHost host;std::array<unsigned char,0x800> header{};std::array<pe::ShotRecord,20> records;fixture_player.shots.field_12558=reinterpret_cast<unsigned>(header.data());fixture_player.shots.context=&fixture_context;
 for(unsigned test=0;test<4096;++test){
  const int pattern=int(test%80),first=int(random()%2000)-1000,second=int(random()%2000)-1000;const unsigned count=test%20;for(unsigned index=0;index<count;++index){auto& row=records[index];fill(row);row.period=std::int8_t(random()%11);row.phase=std::int8_t(random()%10);row.secondary_period=std::int8_t(int(random()%23)-11);row.secondary_phase=std::int8_t(int(random()%11)-5);row.source=std::int8_t(random()%5);}records[count].period=-1;auto* table=records.data();std::memcpy(header.data()+0x5d4+pattern*4,&table,4);const th20::source::sprite::Vec3 p{float(first)/7,float(second)/3,float(test)};
  extra_trace.clear();const int er=cpu<int>(0x505e40,&fixture_player.shots,pattern,first,second,&p);const auto expected_trace=extra_trace;extra_trace.clear();const int ar=pe::fire_shots_at_position(fixture_player.shots,pattern,first,second,p,host);check("shots_at_return",test,&er,&ar,4);auto ec=expected_trace.size(),ac=extra_trace.size();check("shots_at_trace_size",test,&ec,&ac,4);if(ec==ac)check("shots_at_trace",test,expected_trace.data(),extra_trace.data(),ec*4);
 }
}
