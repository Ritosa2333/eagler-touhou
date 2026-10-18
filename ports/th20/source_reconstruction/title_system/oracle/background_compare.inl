// Original full vertex loop executes unchanged. Grid reset / GPU strip commit
// are paired boundaries, leaving the supplied vertices as its input.
hook(0x49d5b0,&mesh_initialize);hook(0x49ddc0,&mesh_commit);
for(const auto& item:th20::parse_pe(bytes).imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
for(unsigned index:{9,10}){auto* lock=reinterpret_cast<unsigned*>(mapped_image_base+0x1c0240+index*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
th20::source::sprite::RenderMesh mesh{};object.mesh=&mesh;std::array<th20::source::sprite::Vertex28,64> vertices;std::array<th20::source::sprite::Vec3,64> positions;
mesh.vertices=vertices.data();mesh.positions=positions.data();at<int>(0x5b87f8)=640;at<int>(0x5b87fc)=480;
for(unsigned mode:{0u,0x100u,0x200u,0x300u}){
 auto prepare=[&]{FloatingEnvironment::prepare();unsigned x87,sse;__control87_2(mode,_MCW_RC,&x87,&sse);};
 for(unsigned test=0;test<2048;++test){
  const int color=static_cast<int>(random());const float weight=test%7?float(random()%20000)/10000.f:std::bit_cast<float>(random());const float direction=test%9?float(int(random()%3000)-1500)/1000.f:std::bit_cast<float>(random());
  prepare();const auto e=cpu<unsigned>(0x51e100,nullptr,color,weight,direction);prepare();const auto a=ti::shade_component(color,weight,direction);check("shade_component",test,&e,&a,4);
 }
 for(unsigned test=0;test<1024;++test){
  mesh.columns=2+random()%7;mesh.rows=2+random()%7;
  for(auto& v:vertices){v.x=float(int(random()%4000)-2000)/2.f;v.y=float(int(random()%4000)-2000)/2.f;v.z=std::bit_cast<float>(random());v.rhw=1;v.color=random();v.u=float(random()%100)/100.f;v.v=float(random()%100)/100.f;}
  object.wave=float(int(random()%18000)-8000)/2.f;for(auto& c:object.color)c=random()%400;
  auto& rng=at<th20::source::state::Random>(0x5ba4c4);rng.state=random();rng.last=random();rng.modulus=0x7fffffff;
  const auto before_vertices=vertices;const auto before_wave=object.wave;const auto before_rng=rng;trace.clear();prepare();cpu<void>(0x51ed90,&object);
  const auto expected_vertices=vertices;const auto expected_wave=object.wave;const auto expected_rng=rng;const auto expected_trace=trace;
  vertices=before_vertices;object.wave=before_wave;rng=before_rng;trace.clear();prepare();event(30);ti::deform_background(object,rng);event(31);
  check("background_vertices",test,expected_vertices.data(),vertices.data(),sizeof(vertices));check("background_wave",test,&expected_wave,&object.wave,4);check("background_rng",test,&expected_rng,&rng,sizeof(rng));check("background_trace",test,expected_trace.data(),trace.data(),expected_trace.size()*4);
 }
}
