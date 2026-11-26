// 完整60ECB0背景切换与60E900视频控制；资源返回句柄及回调后的状态重读保持原序。
#pragma once
#include "core/shared/BackgroundMesh.hpp"
#include "core/math/Random.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerOptions.hpp"
namespace kaikeidou {
inline void control_stage_movies(std::uint32_t mode,StageResourceContext& c,StageResourceServices& services){
    const auto stage=c.stage;
    if(stage==0||stage==2){if(mode)services.movie_seek(c.handles[0],0);services.movie_play(c.handles[0],1,0);return;}
    if(stage!=1)return;
    if(mode)services.movie_seek(c.handles[2],0);
    switch(c.integers[0]){
    case 0:services.movie_play(c.handles[2],1,0);break;
    case 1:services.movie_play(c.handles[2],1,0);services.movie_play(c.handles[3],1,0);break;
    case 2:services.movie_play(c.handles[3],1,0);break;
    case 3:services.movie_play(c.handles[3],1,0);services.movie_play(c.handles[4],1,0);break;
    case 4:services.movie_play(c.handles[4],1,0);break;
    case 5:services.movie_play(c.handles[4],1,0);services.movie_play(c.handles[5],1,0);break;
    default:break;
    }
}
inline void change_stage_resources(std::uint32_t stage,StageResourceContext& c,BackgroundMeshPool& pool,const ObjectRecord<240>& player,
    Random& random,RandomPrecision precision,GameMath& math,StageResourceServices& services){
    using namespace binary_float;const auto bits=static_cast<unsigned>(precision);const options_motion_detail::Arithmetic m{bits};
    const auto set=[&](unsigned i,double value){c.reals[i]=crt_sse2::bits(value);};
    const auto load=[&](unsigned slot,std::string_view name){const auto handle=services.load_graph(name,0);c.handles[slot]=handle;};
    const auto f=[](double value){return to_binary32(binary64(crt_sse2::bits(value)));};
    const auto mesh=[&](std::uint32_t handle,std::uint32_t mode,double upper,double plane,double lower,double bottom,double x,double y,std::uint32_t rows){
        initialize_background_mesh(pool,{{handle,512,0,0,512,512,f(100.),f(100.),mode,f(upper),f(plane),f(lower),f(bottom),f(x),f(y),rows}},bits);
    };
    const auto sine_scaled=[&](unsigned target,unsigned argument,double scale){const auto value=math.sine(options_detail::stored_bits(c.reals[argument]));c.reals[target]=m.store(m.mul(value,m.n(scale)));};
    const auto orbit=[&](){const auto value=services.platform_random(6283);c.reals[4]=m.store(m.mul(m.i(value),m.n(.001)));
        const auto cosine=math.cosine(options_detail::stored_bits(c.reals[4]));c.reals[2]=m.store(m.sub(m.add(m.mul(cosine,m.n(500.)),m.n(192.)),m.n(1075.)));
        const auto sine=math.sine(options_detail::stored_bits(c.reals[4]));c.reals[3]=m.store(m.sub(m.add(m.mul(sine,m.n(500.)),m.n(224.)),m.n(1075.)));
    };
    const auto character_image=[&](bool extra){double scale;
        if(c.character==0){load(3,"dat/img/back/back710.png");scale=extra?.15:.3;}
        else if(c.character==1){load(3,"dat/img/back/back712.png");scale=extra?.3:.7;}
        else{load(3,"dat/img/back/back711.png");scale=extra?.15:.5;}
        set(7,scale);set(8,scale);set(9,scale);
    };
    for(unsigned i=0;i<8;++i)services.delete_graph(c.handles[i],0);
    switch(stage){
    case 0xffffffffu:load(0,"dat/img/back/back000.jpg");load(1,"dat/img/back/back001.png");load(2,"dat/img/back/back002.png");break;
    case 0:set(0,-200.);set(1,224.);load(0,"dat/img/back/back100.wmv");load(1,"dat/img/back/back110.jpg");load(2,"dat/img/back/back111.png");load(3,"dat/img/back/back112.png");break;
    case 1:load(0,"dat/img/back/back210.jpg");load(1,"dat/img/back/back211.png");load(2,"dat/img/back/back200.wmv");load(3,"dat/img/back/back201.wmv");load(4,"dat/img/back/back201.wmv");break;
    case 2:load(0,"dat/img/back/back300.wmv");load(1,"dat/img/back/back310.jpg");load(2,"dat/img/back/back311.png");break;
    case 3:load(0,"dat/img/back/back400.jpg");load(1,"dat/img/back/back410.jpg");load(2,"dat/img/back/back411.jpg");set(0,0.);set(1,0.);sine_scaled(2,1,192.);break;
    case 4:{pool.count=0;constexpr std::array<std::string_view,3> paths{{"dat/img/back/back500.jpg","dat/img/back/back501.jpg","dat/img/back/back502.jpg"}};
        for(unsigned row=0;row<2;++row)for(unsigned col=0;col<3;++col){const auto handle=services.load_graph(paths[col],0);mesh(handle,2,800.,600.,-500.,-700.,124.+200.*col,row?440.:40.,1);}
        const auto handle=services.load_graph("dat/img/back/back503.jpg",0);
        mesh(handle,1,800.,600.,-500.,-700.,24.,340.,1);mesh(handle,1,800.,600.,-500.,-700.,24.,140.,1);mesh(handle,1,800.,600.,-500.,-700.,624.,340.,1);mesh(handle,1,800.,600.,-500.,-700.,624.,140.,1);
        set(0,0.);set(1,0.);set(2,0.);set(3,0.);sine_scaled(4,2,60.);
        const auto cosine=math.cosine(options_detail::stored_bits(c.reals[0]));c.reals[5]=m.store(m.add(m.mul(cosine,m.q(c.reals[4])),m.n(192.)));
        const auto sine=math.sine(options_detail::stored_bits(c.reals[0]));c.reals[6]=m.store(m.add(m.mul(sine,m.q(c.reals[4])),m.n(224.)));
        set(10,1.);set(11,1.);set(12,1.);load(0,"dat/img/back/back510.jpg");load(1,"dat/img/back/back002.png");load(2,"dat/img/back/back520.bmp");break;}
    case 5:{pool.count=0;auto handle=services.load_graph("dat/img/back/back600.png",0);mesh(handle,2,800.,500.,-500.,-700.,324.,-40.,3);
        handle=services.load_graph("dat/img/back/back601.png",0);for(double x:{-476.,-276.,-76.,124.,524.,724.,924.,1124.})mesh(handle,2,800.,500.,-500.,-700.,x,-40.,3);
        c.reals[0]=m.store(m.sub(m.n(192.),m.mul(m.sub(m.q(player.quad(0x90)),m.n(192.)),m.n(1.2))));c.reals[1]=options_detail::stored_bits(c.reals[0]);set(4,-150.);
        c.reals[2]=m.store(m.sub(m.q(c.reals[4]),m.mul(m.sub(m.q(player.quad(0x98)),m.n(224.)),m.n(.02))));c.reals[3]=options_detail::stored_bits(c.reals[2]);
        load(1,"dat/img/back/back002.png");load(2,"dat/img/back/back520.bmp");load(3,"dat/img/back/back610.jpg");load(4,"dat/img/back/back510.jpg");load(5,"dat/img/back/back410.jpg");load(6,"dat/img/back/back411.jpg");set(1,0.);sine_scaled(2,1,192.);break;}
    case 6:{load(0,"dat/img/back/back700.png");load(5,"dat/img/back/back700.png");set(0,0.);set(1,-768.);
        c.integers[0]=0u-random.integer_bits(300,precision,true);c.integers[1]=0u-random.integer_bits(300,precision,true);c.integers[2]=60;c.integers[3]=random.integer_bits(5,precision,true);
        load(4,"dat/img/back/back610.jpg");character_image(false);orbit();load(1,"dat/img/back/back002.png");load(2,"dat/img/back/back520.bmp");pool.count=0;
        const auto handle=services.load_graph("dat/img/back/back701.bmp",0);mesh(handle,0,200.,0.,-200.,-400.,324.,234.,8);set(5,0.);set(6,0.);break;}
    case 7:switch(c.part){
        case 0:set(0,-200.);set(1,224.);load(1,"dat/img/back/back110.jpg");load(2,"dat/img/back/back111.png");load(3,"dat/img/back/back112.png");break;
        case 1:set(0,0.);load(0,"dat/img/back/back210.jpg");load(1,"dat/img/back/back211.png");break;
        case 2:load(1,"dat/img/back/back310.jpg");break;
        case 3:load(2,"dat/img/back/back311.png");break;
        case 4:load(1,"dat/img/back/back410.jpg");load(2,"dat/img/back/back411.jpg");break;
        case 5:load(0,"dat/img/back/back510.jpg");load(1,"dat/img/back/back002.png");load(2,"dat/img/back/back520.bmp");
            if(c.character==0){set(10,.4);set(11,.1);set(12,.1);}else if(c.character==1){set(10,.1);set(11,.1);set(12,.4);}else{set(10,.1);set(11,.4);set(12,.1);}break;
        case 6:load(3,"dat/img/back/back610.jpg");break;
        case 7:character_image(true);load(1,"dat/img/back/back002.png");load(2,"dat/img/back/back520.bmp");orbit();break;
        default:break;
        }break;
    default:break;
    }
}
}
