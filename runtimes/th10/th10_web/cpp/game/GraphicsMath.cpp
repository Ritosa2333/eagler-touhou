#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include <cmath>
#include <initializer_list>
namespace th10 {
namespace {
float reciprocal(float value) noexcept {
    // Preserve the original refinement and the existing browser backend's
    // exact-division seed. Hardware RCP/RSQRT seeds vary between processors.
    const float seed=1.0f/value;
    const float product=seed*value;
    return (seed+seed)-(product*seed);
}
Extended unsigned_number(u32 value) noexcept {
    auto result=Extended::from_int(static_cast<i32>(value));
    if(value&0x80000000u)result=result+number(4294967296.0f);
    return result;
}
Matrix4 combined(const Matrix4* projection,const Matrix4* view,const Matrix4* world) noexcept {
    Matrix4 result;result.identity();bool populated=false;
    for(const auto* matrix:{world,view,projection})if(matrix){
        if(populated)GraphicsMath::multiply(result,result,*matrix);
        else{result=*matrix;populated=true;}
    }
    return result;
}
Vec3 cross(const Vec3& a,const Vec3& b) noexcept {
    return {(number(a.y)*number(b.z)-number(a.z)*number(b.y)).to_float(),
            (number(a.z)*number(b.x)-number(a.x)*number(b.z)).to_float(),
            (number(a.x)*number(b.y)-number(a.y)*number(b.x)).to_float()};
}
float negative_dot(const Vec3& a,const Vec3& b) noexcept {
    return (-((number(a.y)*number(b.y)+number(a.x)*number(b.x))+number(a.z)*number(b.z))).to_float();
}
}
void GraphicsMath::multiply(Matrix4& output,const Matrix4& first,const Matrix4& second) noexcept {
    Matrix4 result;
    for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column){
        const float a=first.elements[row][0]*second.elements[0][column];
        const float b=first.elements[row][1]*second.elements[1][column];
        const float c=first.elements[row][2]*second.elements[2][column];
        const float d=first.elements[row][3]*second.elements[3][column];
        // The original SIMD routine groups rows 0/3 differently from 1/2.
        result.elements[row][column]=(row==0||row==3)?(a+b)+(c+d):((a+b)+c)+d;
    }
    output=result;
}
void GraphicsMath::normalize(Vec3& output,const Vec3& input) noexcept {
    const auto v=input;
    const float square=(v.x*v.x+v.y*v.y)+v.z*v.z;
    if(!(square>=0x1p-46f)){output={0,0,0};return;}
    const float seed=1.0f/std::sqrt(square);
    const float product=(seed*square)*seed;
    const float scale=(3.0f-product)*(seed*0.5f);
    output={v.x*scale,v.y*scale,v.z*scale};
}
void GraphicsMath::rotation(Matrix4& output,u32 axis,float radians) noexcept {
    output.identity();if(axis>=3)return;
    const float s=sine(number(radians)).to_float(),c=cosine(number(radians)).to_float();
    const u32 a=(axis+1)%3,b=(axis+2)%3;
    output.elements[a][a]=c;output.elements[b][b]=c;
    output.elements[a][b]=s;output.elements[b][a]=-s;
}
void GraphicsMath::translation(Matrix4& output,const Vec3& offset) noexcept {
    const auto value=offset;output.identity();output.elements[3][0]=value.x;output.elements[3][1]=value.y;output.elements[3][2]=value.z;
}
void GraphicsMath::look_at(Matrix4& output,const Vec3& eye,const Vec3& target,const Vec3& up) noexcept {
    Vec3 z{Scalar::sub(target.x,eye.x),Scalar::sub(target.y,eye.y),Scalar::sub(target.z,eye.z)};
    normalize(z,z);Vec3 x=cross(up,z);normalize(x,x);const Vec3 y=cross(z,x);
    output={{{x.x,y.x,z.x,0},{x.y,y.y,z.y,0},{x.z,y.z,z.z,0},{negative_dot(x,eye),negative_dot(y,eye),negative_dot(z,eye),1}}};
}
void GraphicsMath::perspective(Matrix4& output,float field_of_view,float aspect,float near_plane,float far_plane) noexcept {
    const float half=Scalar::mul(field_of_view,0.5f);
    const float s=sine(number(half)).to_float(),c=cosine(number(half)).to_float();
    const auto vertical=number(c)/number(s),depth=number(far_plane)/(number(far_plane)-number(near_plane));
    output={};output.elements[0][0]=(vertical/number(aspect)).to_float();output.elements[1][1]=vertical.to_float();
    output.elements[2][2]=depth.to_float();output.elements[2][3]=1;output.elements[3][2]=(-(depth*number(near_plane))).to_float();
}
void GraphicsMath::transform(float* output,const Vec3& input,const Matrix4& matrix) noexcept {
    const auto v=input;float result[4];
    for(u32 column=0;column<4;++column)result[column]=(matrix.elements[0][column]*v.x+matrix.elements[1][column]*v.y)+(matrix.elements[2][column]*v.z+matrix.elements[3][column]);
    std::memcpy(output,result,sizeof(result));
}
void GraphicsMath::transform_normal(Vec3& output,const Vec3& input,const Matrix4& matrix) noexcept {
    const auto v=input;float result[3];
    for(u32 column=0;column<3;++column)result[column]=(matrix.elements[0][column]*v.x+matrix.elements[1][column]*v.y)+matrix.elements[2][column]*v.z;
    std::memcpy(&output,result,sizeof(result));
}
void GraphicsMath::transform_coordinate(Vec3& output,const Vec3& input,const Matrix4& matrix) noexcept {
    float result[4];transform(result,input,matrix);const float scale=reciprocal(result[3]);
    output={result[0]*scale,result[1]*scale,result[2]*scale};
}
void GraphicsMath::project(Vec3& output,const Vec3& input,const CameraViewport* viewport,const Matrix4* projection,const Matrix4* view,const Matrix4* world) noexcept {
    const auto matrix=combined(projection,view,world);transform_coordinate(output,input,matrix);
    if(viewport){const auto& v=*viewport;
        output.x=(((number(output.x)+number(1.0f))*unsigned_number(v.width))*number(0.5f)+unsigned_number(v.x)).to_float();
        output.y=(((number(1.0f)-number(output.y))*unsigned_number(v.height))*number(0.5f)+unsigned_number(v.y)).to_float();
        output.z=((number(v.far_depth)-number(v.near_depth))*number(output.z)+number(v.near_depth)).to_float();
    }
}
void GraphicsMath::project_array(void* output,u32 output_stride,const void* input,u32 input_stride,u32 count,const CameraViewport* viewport,const Matrix4* projection,const Matrix4* view,const Matrix4* world) noexcept {
    auto matrix=combined(projection,view,world);
    if(viewport){const auto& v=*viewport;Matrix4 mapping{};
        const auto half_width=unsigned_number(v.width)*number(0.5f),height=unsigned_number(v.height);
        mapping.elements[0][0]=half_width.to_float();mapping.elements[1][1]=(number(-0.5f)*height).to_float();mapping.elements[2][2]=Scalar::sub(v.far_depth,v.near_depth);
        mapping.elements[3][0]=(unsigned_number(v.x)+half_width).to_float();mapping.elements[3][1]=(unsigned_number(v.y)+height*number(0.5f)).to_float();mapping.elements[3][2]=v.near_depth;mapping.elements[3][3]=1;
        multiply(matrix,matrix,mapping);
    }
    // At eleven vectors the original switches to groups of four, with a
    // different addition order from the one-vector remainder loop.
    const u32 grouped=count>10?count&~3u:0;
    for(u32 index=0;index<count;){
        const u32 group=index<grouped?4:1;Vec3 values[4];
        for(u32 i=0;i<group;++i){Vec3 v;std::memcpy(&v,static_cast<const u8*>(input)+(index+i)*input_stride,12);
            if(group==1)transform_coordinate(values[i],v,matrix);
            else{float t[4];for(u32 c=0;c<4;++c)t[c]=((v.x*matrix.elements[0][c]+v.y*matrix.elements[1][c])+v.z*matrix.elements[2][c])+matrix.elements[3][c];
                const float scale=reciprocal(t[3]);values[i]={t[0]*scale,t[1]*scale,t[2]*scale};}
        }
        for(u32 i=0;i<group;++i)std::memcpy(static_cast<u8*>(output)+(index+i)*output_stride,&values[i],12);
        index+=group;
    }
}
}
