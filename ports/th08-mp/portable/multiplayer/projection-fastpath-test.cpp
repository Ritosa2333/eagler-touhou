#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

#ifndef TH_PROJECTION_FASTPATH_TEST
#error This standalone oracle requires TH_PROJECTION_FASTPATH_TEST.
#endif

using namespace th08;

namespace {
// Frozen pre-fastpath formulas are the strict oracle for finite result lanes.
// Native SIMD NaN payload choices can differ between this separately compiled
// clone and the unchanged original helpers; those lanes use the original
// helpers with this frozen viewport expression, still comparing exact bits.
namespace legacy {
void multiply(Matrix4& output,const Matrix4& first,const Matrix4& second) {
    Matrix4 result;
    for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column){
        if(GraphicsMath::arithmetic()==GraphicsArithmetic::Simd){
            const float a=first.m[row][0]*second.m[0][column],b=first.m[row][1]*second.m[1][column];
            const float c=first.m[row][2]*second.m[2][column],d=first.m[row][3]*second.m[3][column];
            result.m[row][column]=((a+b)+c)+d;
            continue;
        }
        const auto even=number(first.m[row][0])*number(second.m[0][column])+number(first.m[row][2])*number(second.m[2][column]);
        const auto odd=number(first.m[row][1])*number(second.m[1][column])+number(first.m[row][3])*number(second.m[3][column]);
        result.m[row][column]=(even+odd).to_float();
    }
    output=result;
}
void compose(Matrix4& transform,const Matrix4& projection,const Matrix4& view,const Vec3& position) {
    Matrix4 world;world.identity();
    world.m[3][0]=position.x;world.m[3][1]=position.y;world.m[3][2]=position.z;
    transform.identity();bool populated=false;
    const Matrix4* matrices[]{&world,&view,&projection};
    for(const auto* matrix:matrices){
        if(populated)multiply(transform,transform,*matrix);
        else{transform=*matrix;populated=true;}
    }
}
bool almost_one(float value) {
    const auto difference=number(value)-number(1);
    return number(-0x1p-23f)<difference&&difference<number(0x1p-23f);
}
void transform_coordinate(Vec3& output,const Vec3& input,const Matrix4& matrix) {
    const auto v=input;float result[4];
    if(GraphicsMath::arithmetic()==GraphicsArithmetic::Simd){
        for(u32 column=0;column<4;++column)
            result[column]=(v.x*matrix.m[0][column]+v.y*matrix.m[1][column])+(v.z*matrix.m[2][column]+matrix.m[3][column]);
        const float seed=1.0f/result[3],product=seed*result[3];
        const float reciprocal=(seed+seed)-(product*seed);
        output={result[0]*reciprocal,result[1]*reciprocal,result[2]*reciprocal};
        return;
    }
    for(u32 column=0;column<4;++column)
        result[column]=(number(v.x)*number(matrix.m[0][column])+number(v.y)*number(matrix.m[1][column])+number(v.z)*number(matrix.m[2][column])+number(matrix.m[3][column])).to_float();
    if(!almost_one(result[3])){
        const auto reciprocal=number(1)/number(result[3]);
        for(u32 i=0;i<3;++i)result[i]=(reciprocal*number(result[i])).to_float();
    }
    std::memcpy(&output,result,sizeof(output));
}
void viewport_only(Vec3& output,const Viewport* viewport) {
    if(viewport){
        const auto& v=*viewport;
        output.x=((number(output.x)+number(1))*Extended::from_int64(v.width)*number(.5f)+Extended::from_int64(v.x)).to_float();
        output.y=((number(1)-number(output.y))*Extended::from_int64(v.height)*number(.5f)+Extended::from_int64(v.y)).to_float();
        output.z=((number(v.max_z)-number(v.min_z))*number(output.z)+number(v.min_z)).to_float();
    }
}
void project_composed(Vec3& output,const Vec3& input,const Viewport* viewport,const Matrix4& transform) {
    transform_coordinate(output,input,transform);
    viewport_only(output,viewport);
}
void project(Vec3& output,const Vec3& input,const Vec3& position,const Viewport* viewport,const Matrix4& projection,const Matrix4& view) {
    Matrix4 transform;compose(transform,projection,view,position);
    project_composed(output,input,viewport,transform);
}
}

u32 checks=0,raw_cases=0,normal_cases=0,composed_cases=0,matrix_cases=0;
u32 original_nan_references=0;
u32 bits(float value) {u32 result;std::memcpy(&result,&value,sizeof(result));return result;}
float value(u32 raw) {float result;std::memcpy(&result,&raw,sizeof(result));return result;}
[[noreturn]] void fail(const char* label,const char* detail) {
    std::fprintf(stderr,"projection-fastpath case=%u precision=%u rounding=%u graphics=%u %s: %s\n",
        checks,unsigned(arithmetic_precision()),unsigned(arithmetic_rounding()),unsigned(GraphicsMath::arithmetic()),label,detail);
    std::exit(1);
}
void require(bool condition,const char* label,const char* detail) {if(!condition)fail(label,detail);}
void same_vector(const Vec3& expected,const Vec3& actual,const char* label) {
    const std::array<u32,3> want{bits(expected.x),bits(expected.y),bits(expected.z)};
    const std::array<u32,3> got{bits(actual.x),bits(actual.y),bits(actual.z)};
    for(u32 index=0;index<3;++index)if(want[index]!=got[index]){
        std::fprintf(stderr,"vector[%u] expected=%08x actual=%08x\n",index,want[index],got[index]);
        fail(label,"vector bits differ");
    }
}
void same_matrix(const Matrix4& expected,const Matrix4& actual,const char* label) {
    for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column)
        if(bits(expected.m[row][column])!=bits(actual.m[row][column])){
            std::fprintf(stderr,"matrix[%u][%u] expected=%08x actual=%08x\n",
                row,column,bits(expected.m[row][column]),bits(actual.m[row][column]));
            fail(label,"composed matrix bits differ");
        }
}
bool nan(float input) {return (bits(input)&0x7fffffffu)>0x7f800000u;}
bool has_nan(const Vec3& input) {return nan(input.x)||nan(input.y)||nan(input.z);}
Vec3 original_composed_reference(const Vec3& input,const Matrix4& transform,const Viewport* viewport) {
    Vec3 output;
    // These helpers are unchanged production code. Do not use project or
    // project_composed here: their viewport mapping is the code under test.
    GraphicsMath::transform_coordinate(output,input,transform);
    legacy::viewport_only(output,viewport);
    return output;
}
Vec3 original_projection_reference(const Vec3& position,const Vec3& input,const Matrix4& projection,const Matrix4& view,const Viewport* viewport) {
    Matrix4 world;world.identity();world.m[3][0]=position.x;world.m[3][1]=position.y;world.m[3][2]=position.z;
    Matrix4 transform;GraphicsMath::compose_projection(transform,&projection,&view,&world);
    return original_composed_reference(input,transform,viewport);
}
Vec3 resolve_nan_reference(const Vec3& frozen,const Vec3& original,const char* label) {
    ++original_nan_references;
    const std::array<float,3> clone{frozen.x,frozen.y,frozen.z},old{original.x,original.y,original.z};
    for(u32 index=0;index<3;++index){
        // Only a NaN-vs-NaN payload choice is resolved through the actual old
        // helpers. All finite/infinite lanes and every class change stay strict
        // against the frozen formulas; the eventual candidate comparison is
        // bitwise for ALL lanes, including the original helper's NaN payload.
        if(nan(clone[index])&&nan(old[index]))continue;
        require(bits(clone[index])==bits(old[index]),label,"original helper and frozen non-NaN lane disagree");
    }
    return original;
}
enum class Cache {Any,Miss,Hit,Fallback};
void diagnose_projection(const Vec3& position,const Vec3& input,const Matrix4& projection,const Matrix4& view,
                         const Viewport* viewport,const ProjectionFastpathStats& before,const ProjectionFastpathStats& after) {
    const auto vector=[](const char* name,const Vec3& v){
        std::fprintf(stderr,"%s=%08x,%08x,%08x\n",name,bits(v.x),bits(v.y),bits(v.z));
    };
    const auto matrix=[](const char* name,const Matrix4& m){
        std::fprintf(stderr,"%s=",name);
        for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column)
            std::fprintf(stderr,"%s%08x",row||column?",":"",bits(m.m[row][column]));
        std::fprintf(stderr,"\n");
    };
    std::fprintf(stderr,"pathDelta translationHit=%llu computation=%llu fallback=%llu viewportHit=%llu viewportFallback=%llu\n",
        static_cast<unsigned long long>(after.translation_hits-before.translation_hits),
        static_cast<unsigned long long>(after.translation_computations-before.translation_computations),
        static_cast<unsigned long long>(after.translation_fallbacks-before.translation_fallbacks),
        static_cast<unsigned long long>(after.viewport_hits-before.viewport_hits),
        static_cast<unsigned long long>(after.viewport_fallbacks-before.viewport_fallbacks));
    vector("position",position);vector("input",input);matrix("view",view);matrix("projection",projection);
    if(viewport)std::fprintf(stderr,"viewport=%u,%u,%u,%u,%08x,%08x\n",
        viewport->x,viewport->y,viewport->width,viewport->height,bits(viewport->min_z),bits(viewport->max_z));
    Matrix4 world;world.identity();world.m[3][0]=position.x;world.m[3][1]=position.y;world.m[3][2]=position.z;
    Matrix4 frozen_wv,unchanged_wv,frozen_transform,unchanged_transform;
    legacy::multiply(frozen_wv,world,view);GraphicsMath::multiply(unchanged_wv,world,view);
    legacy::compose(frozen_transform,projection,view,position);
    GraphicsMath::compose_projection(unchanged_transform,&projection,&view,&world);
    matrix("frozen_WV",frozen_wv);matrix("unchanged_WV",unchanged_wv);
    matrix("frozen_transform",frozen_transform);matrix("unchanged_transform",unchanged_transform);
    Vec3 frozen_coordinate,unchanged_coordinate,old_entry;
    legacy::transform_coordinate(frozen_coordinate,input,unchanged_transform);
    GraphicsMath::transform_coordinate(unchanged_coordinate,input,unchanged_transform);
    GraphicsMath::project(old_entry,input,viewport,&projection,&view,&world);
    vector("frozen_coordinate_same_matrix",frozen_coordinate);
    vector("unchanged_coordinate_same_matrix",unchanged_coordinate);vector("old_project_entry",old_entry);
    auto unchanged_with_old_viewport=unchanged_coordinate;
    if(viewport){
        const auto& v=*viewport;auto& p=unchanged_with_old_viewport;
        p.x=((number(p.x)+number(1))*Extended::from_int64(v.width)*number(.5f)+Extended::from_int64(v.x)).to_float();
        p.y=((number(1)-number(p.y))*Extended::from_int64(v.height)*number(.5f)+Extended::from_int64(v.y)).to_float();
        p.z=((number(v.max_z)-number(v.min_z))*number(p.z)+number(v.min_z)).to_float();
    }
    vector("unchanged_helpers_with_frozen_viewport",unchanged_with_old_viewport);
}
void check_translation(const Vec3& position,const Vec3& input,const Matrix4& projection,const Matrix4& view,
                       const Viewport* viewport,Cache expected_cache,const char* label) {
    ++checks;
    Vec3 expected,actual{value(0x7fc12345u),value(0xffc54321u),value(0x80000000u)};
    legacy::project(expected,input,position,viewport,projection,view);
    if(has_nan(expected))expected=resolve_nan_reference(expected,original_projection_reference(position,input,projection,view,viewport),label);
    const auto before=GraphicsMath::projection_fastpath_stats();
    GraphicsMath::project_translation(actual,input,position,viewport,projection,view);
    const auto after=GraphicsMath::projection_fastpath_stats();
    if(bits(expected.x)!=bits(actual.x)||bits(expected.y)!=bits(actual.y)||bits(expected.z)!=bits(actual.z))
        diagnose_projection(position,input,projection,view,viewport,before,after);
    same_vector(expected,actual,label);
    if(expected_cache!=Cache::Any){
        require(after.translation_hits-before.translation_hits==(expected_cache==Cache::Hit?1u:0u),label,"unexpected translation hit count");
        require(after.translation_computations-before.translation_computations==(expected_cache==Cache::Miss?1u:0u),label,"unexpected translation computation count");
        require(after.translation_fallbacks-before.translation_fallbacks==(expected_cache==Cache::Fallback?1u:0u),label,"unexpected translation fallback count");
    }
}
void check_matrix(const Vec3& position,const Matrix4& projection,const Matrix4& view,int accepted,const char* label) {
    ++checks;++matrix_cases;
    Matrix4 expected,actual;legacy::compose(expected,projection,view,position);
    for(auto& row:actual.m)for(float& element:row)element=value(0x7fc54321u);
    const Matrix4 before=actual;
    const bool result=GraphicsMath::translation_transform_for_test(actual,position,projection,view);
    if(accepted>=0)require(result==(accepted!=0),label,"unexpected composition acceptance");
    if(result)same_matrix(expected,actual,label);
    else same_matrix(before,actual,label);
}
void check_composed(const Vec3& input,const Matrix4& transform,const Viewport* viewport,int fast,const char* label) {
    ++checks;++composed_cases;
    Vec3 expected,actual;legacy::project_composed(expected,input,viewport,transform);
    if(has_nan(expected))expected=resolve_nan_reference(expected,original_composed_reference(input,transform,viewport),label);
    const auto before=GraphicsMath::projection_fastpath_stats();
    GraphicsMath::project_composed(actual,input,viewport,transform);
    const auto after=GraphicsMath::projection_fastpath_stats();
    same_vector(expected,actual,label);
    if(fast>=0){
        require(after.viewport_hits-before.viewport_hits==(viewport&&fast?1u:0u),label,"unexpected viewport fast-path count");
        require(after.viewport_fallbacks-before.viewport_fallbacks==(viewport&&!fast?1u:0u),label,"unexpected viewport fallback count");
    }
}
Matrix4 base_view() {
    return {{{1,.125f,-.25f,0},{-.0625f,.875f,.5f,0},{.25f,-.125f,1,0},{-37.5f,18.75f,512.25f,1}}};
}
Matrix4 base_projection() {
    return {{{1.75f,0,.03125f,0},{0,2.125f,0,0},{0,0,1.015625f,1},{0,0,-30.5f,0}}};
}
float& matrix_field(Matrix4& projection,Matrix4& view,u32 index) {
    return index<16?view.m[index/4][index%4]:projection.m[(index-16)/4][(index-16)%4];
}
float& vector_field(Vec3& vector,u32 index) {return index==0?vector.x:index==1?vector.y:vector.z;}
u32 random(u32& state) {state^=state<<13;state^=state>>17;state^=state<<5;return state;}
float ordinary(u32& state) {return float(i32(random(state)%8193u)-4096)*.0625f;}

void key_and_fallbacks(Precision precision,Rounding rounding,GraphicsArithmetic graphics) {
    const auto view=base_view(),projection=base_projection();
    const Vec3 position{123.25f,-45.5f,17.125f},input{.25f,-.5f,.125f};
    const Viewport viewport{32,16,384,448,0,1};
    GraphicsMath::reset_projection_fastpath_test();
    check_translation(position,input,projection,view,&viewport,Cache::Miss,"initial translation");
    check_translation(position,input,projection,view,&viewport,Cache::Hit,"repeated translation");
    check_translation({124.25f,-51.5f,19.125f},input,projection,view,&viewport,Cache::Hit,"position is recomputed on a hit");
    for(const Vec3 reused:std::array<Vec3,5>{{{},{1,0,0},{0,-1,0},{.375f,-.625f,.5f},{2,-3,7}}})
        check_translation(position,reused,projection,view,&viewport,Cache::Hit,"beam projection_input is a separate per-call value");
    auto moved=viewport;moved.x=97;moved.y=51;moved.width=385;moved.height=449;moved.min_z=.25f;moved.max_z=.75f;
    check_translation(position,input,projection,view,&moved,Cache::Hit,"viewport is read after matrix reuse");
    check_translation(position,input,projection,view,nullptr,Cache::Hit,"null viewport");
    for(u32 index=0;index<32;++index){
        auto changed_view=view,changed_projection=projection;
        auto& element=matrix_field(changed_projection,changed_view,index);element=value(bits(element)^1u);
        check_translation(position,input,changed_projection,changed_view,&viewport,Cache::Miss,"each matrix lane invalidates");
        check_translation(position,input,changed_projection,changed_view,&viewport,Cache::Hit,"changed matrix repeats");
        check_translation(position,input,projection,view,&viewport,Cache::Miss,"rollback restores prior matrix");
        check_translation(position,input,projection,view,&viewport,Cache::Hit,"restored matrix repeats");

        auto positive_view=view,positive_projection=projection;
        auto& positive=matrix_field(positive_projection,positive_view,index);
        const bool was_zero=bits(positive)==0;positive=value(0);
        check_translation(position,input,positive_projection,positive_view,&viewport,was_zero?Cache::Hit:Cache::Miss,"positive zero matrix lane");
        check_translation(position,input,positive_projection,positive_view,&viewport,Cache::Hit,"positive zero repeats");
        positive=value(0x80000000u);
        check_translation(position,input,positive_projection,positive_view,&viewport,Cache::Miss,"signed zero key is distinct");
        check_translation(position,input,positive_projection,positive_view,&viewport,Cache::Hit,"negative zero repeats");
        check_translation(position,input,projection,view,&viewport,Cache::Miss,"restore after signed zero");
    }
    const auto transition=[&](Precision p,Rounding r,GraphicsArithmetic g){
        arithmetic_mode(p,r);GraphicsMath::arithmetic(g);
        check_translation(position,input,projection,view,&viewport,Cache::Miss,"arithmetic component invalidates independently");
        check_translation(position,input,projection,view,&viewport,Cache::Hit,"arithmetic transition repeats");
    };
    transition(precision==Precision::Single?Precision::Double:Precision::Single,rounding,graphics);
    transition(precision,rounding,graphics);
    transition(precision,rounding==Rounding::NearestEven?Rounding::Down:Rounding::NearestEven,graphics);
    transition(precision,rounding,graphics);
    transition(precision,rounding,graphics==GraphicsArithmetic::Simd?GraphicsArithmetic::Scalar:GraphicsArithmetic::Simd);
    transition(precision,rounding,graphics);

    const u32 exceptional[]{0x7f800000u,0xff800000u,0x7fc12345u,0xffc54321u,0x7f812345u,0xff812345u};
    for(u32 raw:exceptional){
        for(u32 index=0;index<3;++index){
            auto invalid_position=position;vector_field(invalid_position,index)=value(raw);
            check_translation(invalid_position,input,projection,view,&viewport,Cache::Fallback,"nonfinite position bypass");
            check_translation(position,input,projection,view,&viewport,Cache::Hit,"position bypass retains valid entry");
            auto invalid_input=input;vector_field(invalid_input,index)=value(raw);
            check_translation(position,invalid_input,projection,view,&viewport,Cache::Fallback,"nonfinite projection_input bypass");
            check_translation(position,input,projection,view,&viewport,Cache::Hit,"input bypass retains valid entry");
        }
        for(u32 index:{0u,7u,16u,31u}){
            auto invalid_view=view,invalid_projection=projection;
            matrix_field(invalid_projection,invalid_view,index)=value(raw);
            check_translation(position,input,invalid_projection,invalid_view,&viewport,Cache::Fallback,"nonfinite matrix bypass");
            check_translation(position,input,invalid_projection,invalid_view,&viewport,Cache::Fallback,"nonfinite matrix never cached");
            check_translation(position,input,projection,view,&viewport,Cache::Hit,"matrix bypass retains valid entry");
            check_matrix(position,invalid_projection,invalid_view,0,"nonfinite matrix does not publish a partial transform");
        }
        auto invalid=viewport;invalid.min_z=value(raw);
        check_translation(position,input,projection,view,&invalid,Cache::Fallback,"nonfinite viewport minimum");
        invalid=viewport;invalid.max_z=value(raw);
        check_translation(position,input,projection,view,&invalid,Cache::Fallback,"nonfinite viewport maximum");
        check_translation(position,input,projection,view,&viewport,Cache::Hit,"depth bypass retains valid entry");
    }

    Matrix4 overflowing{};
    for(auto& row:overflowing.m)for(float& element:row)element=value(0x7f7fffffu);
    check_matrix(position,overflowing,overflowing,-1,"finite matrices with overflowing composition");
    check_translation(position,input,overflowing,overflowing,&viewport,Cache::Any,"overflowing composition uses old result");
    check_translation(position,input,view,view,&viewport,Cache::Any,"view and projection may alias");
    check_matrix(position,view,view,1,"aliased source matrices");

    Vec3 expected,aliased=input;
    legacy::project(expected,input,position,&viewport,projection,view);
    GraphicsMath::project_translation(aliased,aliased,position,&viewport,projection,view);
    ++checks;same_vector(expected,aliased,"output aliases projection_input");
    aliased=position;
    legacy::project(expected,input,position,&viewport,projection,view);
    GraphicsMath::project_translation(aliased,input,aliased,&viewport,projection,view);
    ++checks;same_vector(expected,aliased,"output aliases position");
    aliased=position;
    legacy::project(expected,position,position,&viewport,projection,view);
    GraphicsMath::project_translation(aliased,aliased,aliased,&viewport,projection,view);
    ++checks;same_vector(expected,aliased,"output aliases both per-call vectors");
}

void viewport_cases() {
    Matrix4 identity;identity.identity();
    const Viewport viewport{32,16,384,448,0,1};
    const Vec3 input{.25f,-.5f,.75f};
    const int normal_fast=single_precision_nearest()?1:0;
    check_composed(input,identity,&viewport,normal_fast,"ordinary viewport compound");
    check_composed(input,identity,nullptr,0,"no viewport performs no mapping");
    for(u32 field=0;field<4;++field)for(u32 size:{0u,1u,16777216u,16777217u,16777218u,0xffffff00u,0xffffffffu}){
        auto changed=viewport;
        switch(field){case 0:changed.x=size;break;case 1:changed.y=size;break;case 2:changed.width=size;break;default:changed.height=size;break;}
        const bool exact=size!=16777217u&&size!=0xffffffffu;
        check_composed(input,identity,&changed,single_precision_nearest()&&exact?1:0,"viewport integer conversion is exact or falls back");
    }
    const Viewport expanding{0,0,2,2,0,1};
    check_composed({value(0x7f7fffffu),value(0x7f7fffffu),.5f},identity,&expanding,0,"overflow then half must recover through full Extended expression");
    const Viewport shrinking{0,0,1,1,0,.5f};
    check_composed({.25f,-.5f,value(0x00800000u)},identity,&shrinking,0,"subnormal intermediate retains extended exponent");
    const Viewport cancellation{0,0,384,448,value(0x807fffffu),value(0x00800000u)};
    check_composed({.25f,-.5f,1},identity,&cancellation,0,"depth subtraction boundary restarts all coordinates");
    const u32 boundaries[]{
        0x00000000u,0x80000000u,0x00000001u,0x80000001u,0x007fffffu,0x807fffffu,
        0x00800000u,0x80800000u,0x00800001u,0x80800001u,0x00ffffffu,0x01000000u,
        0x3f7fffffu,0x3f800000u,0x3f800001u,0xbf7fffffu,0xbf800000u,0xbf800001u,
        0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,0x7fc12345u,0xffc54321u,0x7f812345u,0xff812345u
    };
    for(u32 raw:boundaries){
        for(u32 index=0;index<3;++index){
            auto changed=input;vector_field(changed,index)=value(raw);
            check_composed(changed,identity,&viewport,-1,"raw f32 coordinate boundary");
        }
        auto changed=viewport;changed.min_z=value(raw);
        check_composed(input,identity,&changed,-1,"raw f32 depth minimum");
        changed=viewport;changed.max_z=value(raw);
        check_composed(input,identity,&changed,-1,"raw f32 depth maximum");
    }
    auto alias=input;Vec3 expected;
    legacy::project_composed(expected,input,&viewport,identity);
    GraphicsMath::project_composed(alias,alias,&viewport,identity);
    ++checks;same_vector(expected,alias,"project_composed input/output alias");
}

void random_cases() {
    u32 state=0x91e10da5u;
    const Viewport viewport{32,16,384,448,0,1};
    for(u32 iteration=0;iteration<256;++iteration){
        auto view=base_view(),projection=base_projection();
        view.m[3][0]=ordinary(state);view.m[3][1]=ordinary(state);
        projection.m[0][0]=1.0f+float(random(state)%1024u)*.001953125f;
        const Vec3 position{ordinary(state),ordinary(state),ordinary(state)};
        const Vec3 input=iteration%4?Vec3{ordinary(state)*.00390625f,ordinary(state)*.00390625f,ordinary(state)*.00390625f}:Vec3{};
        check_matrix(position,projection,view,1,"ordinary authored-range matrix composition");
        check_translation(position,input,projection,view,&viewport,Cache::Hit,"ordinary authored-range point");
        check_translation({position.x+.25f,position.y-.5f,position.z+1},input,projection,view,&viewport,Cache::Hit,"same camera next point");
        ++normal_cases;
    }
    // Raw bit patterns cover all f32 exponent classes, not just a distribution
    // made by converting small integers. Exceptional cases compare the complete
    // public fallback against the frozen old formulas.
    state=0xe27d4ebdu;
    for(u32 iteration=0;iteration<128;++iteration){
        Matrix4 view,projection;
        for(auto* matrix:{&view,&projection})for(auto& row:matrix->m)for(float& element:row)element=value(random(state));
        const Vec3 position{value(random(state)),value(random(state)),value(random(state))};
        const Vec3 input{value(random(state)),value(random(state)),value(random(state))};
        auto changed=viewport;
        if(iteration%4==0){changed.x=random(state);changed.y=random(state);changed.width=random(state);changed.height=random(state);}
        if(iteration%8==0){changed.min_z=value(random(state));changed.max_z=value(random(state));}
        if(iteration==63&&arithmetic_precision()==Precision::Single&&arithmetic_rounding()==Rounding::NearestEven&&
           GraphicsMath::arithmetic()==GraphicsArithmetic::Simd){
            // Original failing fixture: the second full matrix multiply sees
            // competing NaNs. The clone selected ffe346ec for every element;
            // the unchanged original multiply selected ffc00000 in [0][0].
            require(bits(position.x)==0x5a1962b7u&&bits(position.y)==0xbfb88b90u&&bits(position.z)==0x77914255u&&
                    bits(input.x)==0xa4a27ab8u&&bits(input.y)==0x9554e702u&&bits(input.z)==0x3f20d8d8u&&
                    bits(view.m[3][3])==0xffe346ecu,"recorded NaN regression","fixture identity changed");
            const auto original=original_projection_reference(position,input,projection,view,&changed);
            same_vector({value(0xffc00000u),value(0xffe346ecu),value(0xffe346ecu)},original,
                        "recorded original NaN payload regression");
            check_translation(position,input,projection,view,&changed,Cache::Fallback,"recorded NaN fallback keeps original payload bits");
        }
        check_matrix(position,projection,view,-1,"raw f32 matrix composition");
        check_translation(position,input,projection,view,&changed,Cache::Any,"raw f32 full projection");
        check_composed(input,view,&changed,-1,"raw f32 composed projection");
        ++raw_cases;
    }
}
}

int main() {
    constexpr Precision precisions[]{Precision::Single,Precision::Double,Precision::Extended};
    constexpr Rounding roundings[]{Rounding::NearestEven,Rounding::TowardZero,Rounding::Down,Rounding::Up};
    constexpr GraphicsArithmetic graphics[]{GraphicsArithmetic::Simd,GraphicsArithmetic::Scalar};
    u32 combinations=0;ProjectionFastpathStats total;
    for(auto precision:precisions)for(auto rounding:roundings)for(auto path:graphics){
        arithmetic_mode(precision,rounding);GraphicsMath::arithmetic(path);
        key_and_fallbacks(precision,rounding,path);
        viewport_cases();random_cases();
        const auto stats=GraphicsMath::projection_fastpath_stats();
        require(stats.translation_hits>0&&stats.translation_computations>0&&stats.translation_fallbacks>0,
                "coverage","missing translation cache, miss, or fallback");
        if(single_precision_nearest())require(stats.viewport_hits>0,"coverage","normal viewport did not use its fast path");
        total.translation_hits+=stats.translation_hits;total.translation_computations+=stats.translation_computations;
        total.translation_fallbacks+=stats.translation_fallbacks;total.viewport_hits+=stats.viewport_hits;total.viewport_fallbacks+=stats.viewport_fallbacks;
        ++combinations;
    }
    require(combinations==24&&raw_cases>0&&normal_cases>0&&matrix_cases>0&&original_nan_references>0,"coverage","missing arithmetic or input classes");
    std::printf("projection-fastpath: PASS combinations=%u checks=%u matrixCases=%u normalCases=%u rawCases=%u composedCases=%u originalNaNReferences=%u translationHits=%llu translationComputations=%llu translationFallbacks=%llu viewportHits=%llu viewportFallbacks=%llu\n",
        combinations,checks,matrix_cases,normal_cases,raw_cases,composed_cases,original_nan_references,
        static_cast<unsigned long long>(total.translation_hits),static_cast<unsigned long long>(total.translation_computations),
        static_cast<unsigned long long>(total.translation_fallbacks),static_cast<unsigned long long>(total.viewport_hits),
        static_cast<unsigned long long>(total.viewport_fallbacks));
    return 0;
}

