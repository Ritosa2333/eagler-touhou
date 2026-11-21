#include "Geometry526.hpp"
#include <algorithm>
#include <cmath>

namespace th128::geometry526 {
namespace {
bool dimensions(unsigned columns,unsigned rows){return columns>=2&&columns<=17&&rows>=2&&rows<=17;}
bool finite(const Vertex& vertex){
    return std::isfinite(vertex.x)&&std::isfinite(vertex.y)&&std::isfinite(vertex.z)&&std::isfinite(vertex.rhw)
        &&std::isfinite(vertex.u)&&std::isfinite(vertex.v);
}
float normalized(float angle){
    constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
    unsigned adjustments=0;
    while(angle>pi&&adjustments++<34)angle=float(double(angle)-double(tau));
    while(angle< -pi&&adjustments++<34)angle=float(double(angle)+double(tau));
    return angle;
}
std::array<float,3> direction(const std::array<float,3>& vector){
    const float squaredX=float(double(vector[0])*vector[0]),squaredY=float(double(vector[1])*vector[1]),
        squaredZ=float(double(vector[2])*vector[2]);
    const float squared=float(double(float(double(squaredX)+squaredY))+squaredZ);
    if(squared<1.4210854715202004e-14f)return {};
    const float seed=float(1.0/std::sqrt(double(squared)));
    const float correction=float(3.0-double(float(double(float(double(seed)*squared))*seed)));
    const float inverse=float(double(correction)*float(double(seed)*0.5));
    return {float(double(vector[0])*inverse),float(double(vector[1])*inverse),float(double(vector[2])*inverse)};
}
}
std::optional<Grid> initializeGrid(unsigned columns,unsigned rows,float x,float y,float width,float height){
    const auto bounded=[](float value){return std::isfinite(value)&&std::abs(value)<=65536;};
    if(!dimensions(columns,rows)||!bounded(x)||!bounded(y)||!bounded(width)||!bounded(height))return std::nullopt;
    Grid grid;grid.columns=columns;grid.rows=rows;
    grid.vertices.reserve(columns*rows);grid.positions.reserve(columns*rows);
    float positionX=float(double(x)+192.0+128.0);
    const float firstY=float(double(y)+16.0);
    const float stepX=float(double(width)/double(columns-1)),stepY=float(double(height)/double(rows-1));
    for(unsigned column=0;column<columns;++column){
        float positionY=firstY;
        for(unsigned row=0;row<rows;++row){
            Vertex vertex;vertex.x=positionX;vertex.y=positionY;
            vertex.u=std::max(0.0f,float(double(positionX)/640.0));vertex.v=std::max(0.0f,float(double(positionY)/480.0));
            if(!finite(vertex))return std::nullopt;
            grid.vertices.push_back(vertex);grid.positions.push_back({positionX,positionY,0});
            positionY=float(double(positionY)+double(stepY));
        }
        positionX=float(double(positionX)+double(stepX));
    }
    return grid;
}

std::optional<std::vector<Vertex>> rowStrips(const Grid& grid){
    if(!dimensions(grid.columns,grid.rows)||grid.vertices.size()!=grid.columns*grid.rows
        ||grid.positions.size()!=grid.vertices.size()||!std::all_of(grid.vertices.begin(),grid.vertices.end(),finite))return std::nullopt;
    std::vector<Vertex> result;result.reserve((grid.columns-1)*grid.rows*2);
    for(unsigned column=0;column+1<grid.columns;++column)for(unsigned row=0;row<grid.rows;++row){
        result.push_back(grid.vertices[column*grid.rows+row]);result.push_back(grid.vertices[(column+1)*grid.rows+row]);
    }
    return result;
}

std::optional<EffectStep> stepEffect(const EffectState& state,const std::array<float,3>& position,
                                   float delta,unsigned columns,unsigned rows){
    const auto bounded=[](float value){return std::isfinite(value)&&std::abs(value)<=65536;};
    if(!dimensions(columns,rows)||!bounded(state.radius)||!bounded(state.scale)||state.scale<=0||state.scale>4096
        ||!std::isfinite(delta)||delta<0||delta>1
        ||!std::all_of(position.begin(),position.end(),bounded)
        ||!std::all_of(state.angles.begin(),state.angles.end(),[](float angle){return std::isfinite(angle)&&std::abs(angle)<=3.1415927410125732f;}))return std::nullopt;
    const float extent=float(double(state.scale)*2.0+40.0);
    const auto grid=initializeGrid(columns,rows,float(double(position[0])-state.scale-20.0),
        float(double(position[1])-state.scale-20.0),extent,extent);
    if(!grid)return std::nullopt;
    EffectStep result{state,*grid};
    if(state.radius>state.scale)result.next.scale=float(double(state.scale)+double(delta)*2.0);
    const std::array center{float(double(position[0])+320.0),float(double(position[1])+16.0),position[2]};
    const double radiusSquared=double(state.scale)*state.scale;
    auto angles=state.angles;
    for(std::size_t index=0;index<result.grid.vertices.size();++index){
        auto& vertex=result.grid.vertices[index];auto& original=result.grid.positions[index];
        const std::array vector{float(double(original[0])-center[0]),float(double(original[1])-center[1]),
            float(double(original[2])-center[2])};
        const float squared=float(double(vector[0])*vector[0]+double(vector[1])*vector[1]);
        const float remaining=float(radiusSquared-double(squared));
        if(remaining>=0){
            const float ratio=float(double(remaining)/radiusSquared);
            vertex.color=0xff000000u;
            for(const unsigned shift:{0u,8u,16u}){
                const auto channel=(state.color>>shift)&255u;
                const auto interpolated=std::uint32_t(255.0-double(255u-channel)*ratio);
                vertex.color|=(interpolated&255u)<<shift;
            }
            const auto unit=direction(vector);
            const float magnitude=float(double(ratio)*32.0);
            const float offsetX=float(double(float(double(unit[0])*magnitude))+double(float(std::sin(double(angles[0]))))*ratio*8.0);
            const float offsetY=float(double(float(double(unit[1])*magnitude))+double(float(std::sin(double(angles[1]))))*ratio*8.0);
            vertex.x=float(double(vertex.x)+offsetX);vertex.y=float(double(vertex.y)+offsetY);
            vertex.z=0;original[2]=0;
        }else vertex.color&=0x00ffffffu;
        angles[0]=normalized(float(double(angles[0])+0.09817477315664291));
        angles[1]=normalized(float(double(angles[1])-0.04908738657832146));
        if(original[0]<=128){original[0]=129;vertex.x=129;}
        else if(original[0]>=512){original[0]=511;vertex.x=511;}
        if(original[1]<=16){original[1]=17;vertex.y=17;}
        else if(original[1]>=464){original[1]=463;vertex.y=463;}
        vertex.u=std::max(0.0f,float(double(original[0])/640.0));
        vertex.v=std::max(0.0f,float(double(original[1])/480.0));
        if(!finite(vertex))return std::nullopt;
    }
    const float firstDelta=float(double(delta)*0.19634954631328583),secondDelta=float(double(delta)*0.09817477315664291);
    result.next.angles={normalized(float(double(state.angles[0])+firstDelta)),normalized(float(double(state.angles[1])+secondDelta))};
    return result;
}
}
