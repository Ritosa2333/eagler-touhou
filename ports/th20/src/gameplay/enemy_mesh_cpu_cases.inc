{
    namespace sp=th20::source::sprite;
    struct Host final:gp::EnemyMeshServices {
        float rate=1;int width=640,height=480;
        float clock_scale()override{return rate;}
        void initialize(sp::RenderMesh& m,float x,float y,float w,float h)override{original<void>(0x49d850,&m,x,y,w,h);}
        int view_offset(int index,unsigned axis)override{return original<int>(axis?0x49df20:0x49df00,reinterpret_cast<void*>(mapped_image_base+0x1b6758),index);}
        int dimension(unsigned axis)override{return axis?height:width;}
        void uv(const sp::Vec3& p,float& u,float& v)override{u=p.x/float(width);v=p.y/float(height);if(u<0)u=0;if(v<0)v=0;}
        void update(sp::RenderMesh& m)override{original<void>(0x49ddc0,&m);}
    } host;
    auto& scale=*reinterpret_cast<float*>(mapped_image_base+0x1aefe4);const auto old_scale=scale;
    auto* window=reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1b6758);const std::array<unsigned,6> offsets{0x38,0x3c,0x40,0x44,0x20a0,0x20a4};std::array<unsigned,6> saved;for(unsigned i=0;i<6;++i)std::memcpy(&saved[i],window+offsets[i],4);
    const auto finite=[&](){return float(int(rng()%40000)-10000)/32.f;};
    for(unsigned n=0;n<1024;++n){
        host.width=n%2?640:960;host.height=n%2?480:720;host.rate=scale=float(n%9)*.25f;
        for(unsigned i=0;i<4;++i)*reinterpret_cast<int*>(window+offsets[i])=int(n%13)*11-50;*reinterpret_cast<int*>(window+0x20a0)=host.width;*reinterpret_cast<int*>(window+0x20a4)=host.height;
        gp::EnemyState state;sp::RenderMesh mesh{};mesh.columns=2+n%16;mesh.rows=2+(n/16)%16;mesh.view_index=n%2;
        std::vector<sp::Vertex28> vertices(mesh.columns*mesh.rows);std::vector<sp::Vec3> positions(vertices.size());std::vector<std::vector<sp::Vertex28>> strips(mesh.columns-1);std::vector<sp::Animation> animations(mesh.columns-1);std::vector<sp::Animation*> animation_pointers;
        for(unsigned i=0;i<strips.size();++i){strips[i].resize(mesh.rows*2);animations[i].fields_550[5]=reinterpret_cast<unsigned>(strips[i].data());animation_pointers.push_back(&animations[i]);}mesh.vertices=vertices.data();mesh.positions=positions.data();mesh.strips=animation_pointers.data();
        gp::EnemyMeshOwner owner{&mesh,0,finite(),n%17?float(n%220)+.25f:0.f,rng(),finite(),finite()};state.fields_2c8[3]=reinterpret_cast<unsigned>(&owner);state.fields_2c8[8]=n%2;sp::Vec3 center{finite(),finite(),float(n%7)-3};std::memcpy(state.motion_110.words,&center,12);
        const auto before=owner;FloatingEnvironment::prepare();original<void>(0x4a4190,&state);const auto expected=owner;const auto expected_vertices=vertices;const auto expected_positions=positions;const auto expected_strips=strips;
        owner=before;FloatingEnvironment::prepare();gp::update_enemy_mesh(state,host);
        const bool owner_same=std::memcmp(&owner,&expected,sizeof(owner))==0,vertex_same=std::memcmp(vertices.data(),expected_vertices.data(),vertices.size()*sizeof(sp::Vertex28))==0,position_same=std::memcmp(positions.data(),expected_positions.data(),positions.size()*sizeof(sp::Vec3))==0;
        bool strips_same=true;for(unsigned i=0;i<strips.size();++i)strips_same&=std::memcmp(strips[i].data(),expected_strips[i].data(),strips[i].size()*sizeof(sp::Vertex28))==0;
        if((!owner_same||!vertex_same||!position_same)&&failed<20){std::cerr<<"Enemy mesh case "<<n<<" owner "<<owner_same<<" vertices "<<vertex_same<<" positions "<<position_same<<'\n';if(!vertex_same){const auto* a=reinterpret_cast<const unsigned*>(vertices.data());const auto* b=reinterpret_cast<const unsigned*>(expected_vertices.data());for(unsigned i=0;i<vertices.size()*7;++i)if(a[i]!=b[i]){std::cerr<<"Vertex word "<<i<<" native "<<std::hex<<b[i]<<" source "<<a[i]<<std::dec<<'\n';break;}}}
        check("Enemy_mesh_owner_all28bytes",owner_same);check("Enemy_mesh_all_vertex_color_warp_uv_bytes",vertex_same);check("Enemy_mesh_source_positions_and_final_strips",position_same&&strips_same);
        state.fields_2c8[3]=0;
    }
    for(unsigned i=0;i<6;++i)std::memcpy(window+offsets[i],&saved[i],4);scale=old_scale;
}
