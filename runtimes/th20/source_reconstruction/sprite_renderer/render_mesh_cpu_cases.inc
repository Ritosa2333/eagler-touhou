// Included in the hash-verified CPU test main; no game entry point is run.
{
    for(const auto& imported:pe.imports)if(imported.name=="HeapAlloc"||imported.name=="HeapFree"||imported.name=="GetLastError")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+imported.iat_rva)=reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),imported.name.c_str()));
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();
    *reinterpret_cast<int*>(mapped_image_base+0x1b87f8)=640;*reinterpret_cast<int*>(mapped_image_base+0x1b87fc)=480;
    for(unsigned i=0;i<2;++i){*reinterpret_cast<int*>(mapped_image_base+0x1b6790+i*4)=int(i*37)-11;*reinterpret_cast<int*>(mapped_image_base+0x1b6798+i*4)=int(i*53)-17;}
    *reinterpret_cast<int*>(mapped_image_base+0x1c538c)=7;*reinterpret_cast<int*>(mapped_image_base+0x1c5390)=-19;
    for(unsigned test=0;test<120;++test){diagnostic_case=test;diagnostic_phase="mesh setup";init();c.field_6c4=0;
        s::AnmInstruction instruction{2,8,0,0};s::AnmInstruction* scripts[16];s::Animation templates[16];
        for(unsigned i=0;i<16;++i){scripts[i]=&instruction;s::construct_animation(templates[i]);s::reset_animation_state(templates[i]);templates[i].base.fields_10_28[6]=0;templates[i].base.fields_10_28[5]=i;}
        s::AnimationFile file{};file.templates=templates;file.scripts=scripts;c.files[0]=&file;mesh_file=&file;
        *reinterpret_cast<void**>(mapped_image_base+0x1c5880)=&file;mesh_enabled=test%13!=0;*reinterpret_cast<void**>(mapped_image_base+0x1c4edc)=mesh_enabled?&file:nullptr;
        for(unsigned i=0;i<32;++i)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[i].free_link));
        const int columns=1+rng()%8,rows=1+rng()%10,alternate=test%2,index=test%2;const Snapshot before(c);const auto counter=file.fields_5c[3];
        s::RenderMesh mesh{};diagnostic_phase="mesh original ctor";cpu<void>(0x49cf80,&mesh,columns,rows,alternate,index);const Snapshot expected_ctor(c);const auto expected_mesh=mesh;const auto original_counter=file.fields_5c[3];
        std::vector<std::uint32_t> original_geometry;for(unsigned i=0;i<32;++i)if(c.pool[i].animation.fields_550[5])original_geometry.push_back(c.pool[i].animation.fields_550[5]);
        if(mesh_enabled){std::memset(mesh.vertices,0xa8,columns*rows*sizeof(s::Vertex28));std::memset(mesh.positions,0xa8,columns*rows*sizeof(s::Vec3));}
        const float x=float(int(rng()%1000)-500)/3.f,y=float(int(rng()%1000)-500)/7.f,width=float(rng()%1000)/5.f,height=float(rng()%1000)/11.f;
        diagnostic_phase="mesh original grid";FloatingEnvironment::prepare();cpu<void>(test%3?0x49d850:0x49d5b0,&mesh,x,y,width,height);
        std::vector<s::Vertex28> expected_vertices;std::vector<s::Vec3> expected_positions;std::vector<std::vector<s::Vertex28>> expected_strips;
        if(mesh_enabled){expected_vertices.assign(mesh.vertices,mesh.vertices+columns*rows);expected_positions.assign(mesh.positions,mesh.positions+columns*rows);for(int i=0;i<columns-1;++i){auto* vertices=reinterpret_cast<s::Vertex28*>(mesh.strips[i]->fields_550[5]);expected_strips.emplace_back(vertices,vertices+rows*2);}}
        diagnostic_phase="mesh original destructor";cpu<void>(0x471ca0,&mesh);const auto expected_destroy=mesh;const Snapshot expected_retire(c);
        before.restore(c);file.fields_5c[3]=counter;mesh={};diagnostic_phase="mesh source ctor";s::construct_render_mesh(mesh,columns,rows,alternate,index);auto normalized_mesh=expected_mesh;normalized_mesh.strip_handles=mesh.strip_handles;normalized_mesh.strips=mesh.strips;normalized_mesh.vertices=mesh.vertices;normalized_mesh.positions=mesh.positions;check("mesh_ctor_fields",&normalized_mesh,&mesh,sizeof(mesh));check("mesh_ctor_spawn_count",&original_counter,&file.fields_5c[3],4);
        auto normalized_ctor=expected_ctor;for(unsigned i=0;i<32;++i){auto* original=reinterpret_cast<s::PooledAnimation*>(normalized_ctor.pool.data())+i;original->animation.fields_550[5]=c.pool[i].animation.fields_550[5];}compare("mesh_ctor_pool",normalized_ctor,Snapshot(c));
        if(mesh_enabled){std::memset(mesh.vertices,0xa8,columns*rows*sizeof(s::Vertex28));std::memset(mesh.positions,0xa8,columns*rows*sizeof(s::Vec3));}
        diagnostic_phase="mesh source grid";FloatingEnvironment::prepare();if(test%3)s::initialize_render_mesh(mesh,x,y,width,height);else s::initialize_display_render_mesh(mesh,x,y,width,height);
        if(mesh_enabled){check("mesh_grid_vertices",expected_vertices.data(),mesh.vertices,expected_vertices.size()*sizeof(s::Vertex28));check("mesh_grid_positions",expected_positions.data(),mesh.positions,expected_positions.size()*sizeof(s::Vec3));for(int i=0;i<columns-1;++i)check("mesh_strip_upload",expected_strips[i].data(),reinterpret_cast<void*>(mesh.strips[i]->fields_550[5]),expected_strips[i].size()*sizeof(s::Vertex28));}
        diagnostic_phase="mesh source destructor";s::destroy_render_mesh_contents(mesh);check("mesh_destructor_fields",&expected_destroy,&mesh,sizeof(mesh));auto normalized_retire=expected_retire;for(unsigned i=0;i<32;++i){auto* original=reinterpret_cast<s::PooledAnimation*>(normalized_retire.pool.data())+i;original->animation.fields_550[5]=c.pool[i].animation.fields_550[5];}compare("mesh_retired_vm_fields",normalized_retire,Snapshot(c));
        for(auto pointer:original_geometry)HeapFree(GetProcessHeap(),0,reinterpret_cast<void*>(pointer));for(unsigned i=0;i<32;++i){auto& a=c.pool[i].animation;if(a.fields_550[5]){th20::source::runtime::release_bytes(reinterpret_cast<void*>(a.fields_550[5]));a.fields_550[5]=0;}}
    }
}
