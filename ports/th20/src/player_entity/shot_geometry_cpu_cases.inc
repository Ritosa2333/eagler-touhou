{
    namespace geom=pl::shot_geometry;namespace sp=th20::source::sprite;
    using Line=bool(__cdecl*)(float*,float*,float,float,float,float);
    using Segment=int(__cdecl*)(float*,float*,float,float,float,float,float,float,float,float);
    using Circle=void(__cdecl*)(float*,float*,const sp::Vec3*,float,const sp::Vec3*,float);
    using Rectangle=int(__cdecl*)(sp::Vec3*,sp::Vec3*,const sp::Vec3*,float,float,float,float,float,float);
    const auto line=reinterpret_cast<Line>(mapped_image_base+0x56170);const auto segment=reinterpret_cast<Segment>(mapped_image_base+0x545d0);
    const auto circle=reinterpret_cast<Circle>(mapped_image_base+0x54480);const auto rectangle=reinterpret_cast<Rectangle>(mapped_image_base+0x547e0);
    for(unsigned n=0;n<9000;++n){
        float ax=finite(),ay=finite(),bx=n%8?finite():ax,by=n%9?finite():ay,cx=n%7?finite():ax,cy=n%11?finite():ay,dx=n%6?finite():cx,dy=n%5?finite():cy;
        if(n%13==0){bx=ax;cx=ax;dx=ax;}if(n%17==0){by=ay;cy=ay;dy=ay;}
        float out0[2]{7,11},out1[2]{7,11};FloatingEnvironment::prepare();const bool native_line=line(&out0[0],&out0[1],ax,ay,bx,by);FloatingEnvironment::prepare();const bool source_line=geom::line_equation(out1[0],out1[1],ax,ay,bx,by);
        check("Shot_geometry_line_equation",native_line==source_line&&std::memcmp(out0,out1,sizeof(out0))==0);
        out0[0]=out1[0]=7;out0[1]=out1[1]=11;FloatingEnvironment::prepare();const int native_segment=segment(&out0[0],&out0[1],ax,ay,bx,by,cx,cy,dx,dy);FloatingEnvironment::prepare();const bool source_segment=geom::segment_intersection(out1[0],out1[1],ax,ay,bx,by,cx,cy,dx,dy);
        check("Shot_geometry_segment_intersection",native_segment==int(source_segment)&&std::memcmp(out0,out1,sizeof(out0))==0);
        sp::Vec3 origin{ax,ay,31},center{cx,cy,41};const float angle=n%6?finite():0,radius=std::fabs(finite())+.5f;
        FloatingEnvironment::prepare();circle(&out0[0],&out0[1],&origin,angle,&center,radius);FloatingEnvironment::prepare();geom::ray_circle(out1[0],out1[1],origin,angle,center,radius);
        check("Shot_geometry_ray_circle",std::memcmp(out0,out1,sizeof(out0))==0);
        sp::Vec3 near0{13,17,23},far0{19,29,31},near1=near0,far1=far0;const float width=finite(),height=finite(),rectangle_angle=finite();
        FloatingEnvironment::prepare();const int native_rectangle=rectangle(&near0,&far0,&origin,angle,cx,cy,width,height,rectangle_angle);FloatingEnvironment::prepare();const bool source_rectangle=geom::ray_rectangle(near1,far1,origin,angle,cx,cy,width,height,rectangle_angle);
        if((std::memcmp(&near0,&near1,sizeof(near0))||std::memcmp(&far0,&far1,sizeof(far0)))&&failed<10)std::cerr<<"Ray rectangle case "<<n<<" original "<<near0.x<<","<<near0.y<<" source "<<near1.x<<","<<near1.y<<" inputs "<<ax<<","<<ay<<","<<angle<<","<<cx<<","<<cy<<","<<width<<","<<height<<","<<rectangle_angle<<'\n';
        check("Shot_geometry_ray_rectangle",native_rectangle==int(source_rectangle)&&std::memcmp(&near0,&near1,sizeof(near0))==0&&std::memcmp(&far0,&far1,sizeof(far0))==0);
    }
    { //Finite collinear inputs produce unordered intersection distances.
        sp::Vec3 near0{13,17,23},far0{19,29,31},near1=near0,far1=far0,origin{};
        FloatingEnvironment::prepare();const auto expected=rectangle(&near0,&far0,&origin,0,0,-1,2,2,1);
        FloatingEnvironment::prepare();const auto actual=geom::ray_rectangle(near1,far1,origin,0,0,-1,2,2,1);
        check("Shot_geometry_collinear_nan_order_regression",expected==int(actual)&&std::memcmp(&near0,&near1,sizeof(near0))==0&&std::memcmp(&far0,&far1,sizeof(far0))==0);
    }
}
