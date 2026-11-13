#pragma once
#include "SceneCamera.hpp"
#include <cstring>
namespace th11 {
inline void th14_camera_matrix_word(Matrix4& target,const Matrix4& value,u32 index)noexcept{
    u32 bits;std::memcpy(&bits,value.m+index,4);
    __atomic_store_n(reinterpret_cast<u32*>(target.m+index),bits,__ATOMIC_SEQ_CST);
}
inline void th14_publish_camera_view(Matrix4& target,const Matrix4& value)noexcept{
    // D3DX9_43 exported LookAtLH100329c0..10032a5b stores one column at a time.
    constexpr u32 order[]={0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15};
    for(u32 index:order)th14_camera_matrix_word(target,value,index);
}
inline void th14_publish_camera_projection(Matrix4& target,const Matrix4& value)noexcept{
    // PerspectiveFovLH10032c36..10032c86 stores index5 after indices6..9.
    constexpr u32 order[]={0,1,2,3,4,6,7,8,9,5,10,11,12,13,14,15};
    for(u32 index:order)th14_camera_matrix_word(target,value,index);
}
}
