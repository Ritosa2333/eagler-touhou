#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include "ReciprocalSqrtTable.hpp"
#include <array>
#include "PracticeRsqrtTables.hpp"
#include <cmath>
#include <initializer_list>

namespace th08 {
namespace {
GraphicsArithmetic arithmetic_path=GraphicsArithmetic::Simd;
bool practice_rsqrt_enabled=false;char practice_rsqrt_cpu='i';
float from_bits(u32 bits) { float value; std::memcpy(&value,&bits,4); return value; }
u32 to_bits(float value) { u32 bits; std::memcpy(&bits,&value,4); return bits; }
void rotation_sine_cosine(float angle, float& sin, float& cos) {
    // Original D3DX 8 SIMD rotations use their own paired polynomial. The
    // sine and cosine lanes reduce by the four parts of pi/2 in different
    // orders; preserve those rounding boundaries instead of calling libm.
    const u32 sign=to_bits(angle)&0x80000000u;
    const float magnitude=from_bits(to_bits(angle)&0x7fffffffu);
    const float scaled=magnitude*from_bits(0x3f22f983);
    const float biased=scaled+8388608.0f;
    const float quadrant=biased-8388608.0f;
    const u32 integer=to_bits(biased)&0x7fffff, odd=integer&1;
    const float pi_parts[]{from_bits(0x3fc90000),from_bits(0x39fda000),from_bits(0x33a22000),from_bits(0x2c34611a)};
    float reduced[2]{magnitude,magnitude};
    for(u32 lane=0;lane<2;++lane) for(u32 part=0;part<4;++part) {
        const float contribution=pi_parts[(part+lane)%4]*quadrant;
        reduced[lane]=reduced[lane]-contribution;
    }
    const float square_s=reduced[0]*reduced[0], square_c=reduced[1]*reduced[1];
    float ps=from_bits(0xb94ca1f0)*square_s;
    ps=ps+from_bits(0x3c08839d); ps=ps*square_s;
    ps=ps+from_bits(0xbe2aaaa3); ps=ps*square_s;
    ps=ps+1.0f; ps=ps*reduced[0]; ps=ps+0.0f;
    float pc=from_bits(0x37ccf5ce)*square_c;
    pc=pc+from_bits(0xbab6061a); pc=pc*square_c;
    pc=pc+from_bits(0x3d2aaaa5); pc=pc*square_c;
    pc=pc-.5f; pc=pc*square_s; pc=pc+1.0f;
    const u32 sine_sign=(((integer-odd)&2)<<30)^sign;
    const u32 cosine_sign=((integer+odd)&2)<<30;
    sin=from_bits(to_bits(odd?pc:ps)^sine_sign);
    cos=from_bits(to_bits(odd?ps:pc)^cosine_sign);
}
bool almost_one(float value) {
    const auto difference = number(value) - number(1);
    return number(-0x1p-23f) < difference && difference < number(0x1p-23f);
}

Vec3 cross(const Vec3& a, const Vec3& b) {
    return {(number(a.y)*number(b.z)-number(a.z)*number(b.y)).to_float(),
            (number(a.z)*number(b.x)-number(a.x)*number(b.z)).to_float(),
            (number(a.x)*number(b.y)-number(a.y)*number(b.x)).to_float()};
}
Extended negative_dot(const Vec3& a, const Vec3& b) {
    return -(number(a.y)*number(b.y)+number(a.x)*number(b.x)+number(a.z)*number(b.z));
}

bool finite_bits(u32 bits) noexcept {return (bits&0x7f800000u)!=0x7f800000u;}
bool finite_vector(const Vec3& value) noexcept {
    return finite_bits(to_bits(value.x))&&finite_bits(to_bits(value.y))&&finite_bits(to_bits(value.z));
}
bool finite_row(const float* row) noexcept {
    for(u32 column=0;column<4;++column)if(!finite_bits(to_bits(row[column])))return false;
    return true;
}
struct TranslationKey {
    std::array<u32,32> matrices{};
    Precision precision{};Rounding rounding{};GraphicsArithmetic graphics{};
    bool operator==(const TranslationKey& other)const noexcept {
        return precision==other.precision&&rounding==other.rounding&&graphics==other.graphics&&matrices==other.matrices;
    }
};
bool translation_key(TranslationKey& key,const Matrix4& projection,const Matrix4& view) noexcept {
    key.precision=arithmetic_precision();key.rounding=arithmetic_rounding();key.graphics=arithmetic_path;
    u32 index=0;
    for(const auto* matrix:{&view,&projection})for(const auto& row:matrix->m)for(float element:row){
        const u32 bits=to_bits(element);if(!finite_bits(bits))return false;
        key.matrices[index++]=bits;
    }
    return true;
}
// Only these derived rows are cached. Actual matrix bits and arithmetic modes
// are checked on every call, including camera switches within a background draw
// and matrices restored by rollback. No game-owned state or pointer is retained.
struct TranslationCache {TranslationKey key;float rows[3][4]{};bool valid=false;};
static_assert(sizeof(TranslationCache)<=256,"Translation projection cache must remain bounded value storage.");
TranslationCache translation_cache;
#ifdef TH_PROJECTION_FASTPATH_TEST
ProjectionFastpathStats test_projection_stats;
#endif
void multiply_row(float* output,const float* first,const Matrix4& second) {
    // Keep multiply's dispatch-specific grouping and float store boundary.
    // In particular, this is not a dot product with (view * projection).
    for(u32 column=0;column<4;++column){
        if(arithmetic_path==GraphicsArithmetic::Simd){
            const float a=first[0]*second.m[0][column],b=first[1]*second.m[1][column];
            const float c=first[2]*second.m[2][column],d=first[3]*second.m[3][column];
            output[column]=((a+b)+c)+d;
            continue;
        }
        const auto even=number(first[0])*number(second.m[0][column])+number(first[2])*number(second.m[2][column]);
        const auto odd=number(first[1])*number(second.m[1][column])+number(first[3])*number(second.m[3][column]);
        output[column]=(even+odd).to_float();
    }
}
bool compose_translation(Matrix4& output,const Vec3& position,const Matrix4& projection,const Matrix4& view) {
    TranslationKey key;
    if(!finite_vector(position)||!translation_key(key,projection,view))return false;
    Matrix4 transform{};
    const bool hit=translation_cache.valid&&translation_cache.key==key;
    if(hit)std::memcpy(transform.m,translation_cache.rows,sizeof(translation_cache.rows));
    else{
        float intermediate[3][4];
        for(u32 row=0;row<3;++row){
            float identity_row[4]{};identity_row[row]=1;
            // Even identity's zero products and signed-zero sums are retained.
            multiply_row(intermediate[row],identity_row,view);
            if(!finite_row(intermediate[row]))return false;
        }
        for(u32 row=0;row<3;++row){
            multiply_row(transform.m[row],intermediate[row],projection);
            if(!finite_row(transform.m[row]))return false;
        }
    }
    const float translation_row[4]{position.x,position.y,position.z,1};
    float intermediate[4];
    multiply_row(intermediate,translation_row,view);
    if(!finite_row(intermediate))return false;
    multiply_row(transform.m[3],intermediate,projection);
    if(!finite_row(transform.m[3]))return false;
    if(!hit){
        translation_cache.key=key;
        std::memcpy(translation_cache.rows,transform.m,sizeof(translation_cache.rows));
        translation_cache.valid=true;
    }
    output=transform;
#ifdef TH_PROJECTION_FASTPATH_TEST
    if(hit)++test_projection_stats.translation_hits;else ++test_projection_stats.translation_computations;
#endif
    return true;
}
bool exact_viewport_integer(u32 value,float& output) noexcept {
    output=static_cast<float>(value);
    // Every u32 is exact in double. Avoid an out-of-range float-to-u32 cast
    // when UINT32_MAX rounds to 2^32, while accepting exact larger multiples.
    return value<=0x01000000u||static_cast<double>(output)==static_cast<double>(value);
}
bool viewport_compound(Vec3& output,const Vec3& input,const Viewport& viewport) noexcept {
    if(!single_precision_nearest())return false;
    float width,height,origin_x,origin_y;
    if(!exact_viewport_integer(viewport.width,width)||!exact_viewport_integer(viewport.height,height)||
       !exact_viewport_integer(viewport.x,origin_x)||!exact_viewport_integer(viewport.y,origin_y))return false;
    using touhou::numeric::scalar_binary;
    float x_shift,x_scale,x_half,y_shift,y_scale,y_half,depth,depth_scale;
    Vec3 result;
    // Accept the compound expression only if EVERY original operation stays
    // in the exact f32 fast range. A failed step restarts the whole viewport
    // expression below, retaining extended exponents and its original grouping.
    if(!scalar_binary(input.x,1,x_shift,0)||!scalar_binary(x_shift,width,x_scale,2)||
       !scalar_binary(x_scale,.5f,x_half,2)||!scalar_binary(x_half,origin_x,result.x,0)||
       !scalar_binary(1,input.y,y_shift,1)||!scalar_binary(y_shift,height,y_scale,2)||
       !scalar_binary(y_scale,.5f,y_half,2)||!scalar_binary(y_half,origin_y,result.y,0)||
       !scalar_binary(viewport.max_z,viewport.min_z,depth,1)||!scalar_binary(depth,input.z,depth_scale,2)||
       !scalar_binary(depth_scale,viewport.min_z,result.z,0))return false;
    output=result;
    return true;
}
}

#ifdef TH_PROJECTION_FASTPATH_TEST
ProjectionFastpathStats GraphicsMath::projection_fastpath_stats() noexcept {return test_projection_stats;}
void GraphicsMath::reset_projection_fastpath_test() noexcept {translation_cache={};test_projection_stats={};}
bool GraphicsMath::translation_transform_for_test(Matrix4& output,const Vec3& position,const Matrix4& projection,const Matrix4& view) {
    return compose_translation(output,position,projection,view);
}
#endif

void GraphicsMath::arithmetic(GraphicsArithmetic path) { arithmetic_path=path; }
GraphicsArithmetic GraphicsMath::arithmetic() noexcept { return arithmetic_path; }
void GraphicsMath::practice_rsqrt(bool enabled,char cpu){practice_rsqrt_enabled=enabled;practice_rsqrt_cpu=cpu;if(enabled)PracticeRsqrtTables::initialize();}

void GraphicsMath::multiply(Matrix4& output, const Matrix4& first, const Matrix4& second) {
    // D3DX's initialized scalar dispatch (0x486fd6) sums even and odd
    // products separately. Its initial, unused bootstrap routine differs.
    Matrix4 result;
    for (u32 row=0; row<4; ++row) for (u32 column=0; column<4; ++column) {
        if (arithmetic_path==GraphicsArithmetic::Simd) {
            const float a=first.m[row][0]*second.m[0][column], b=first.m[row][1]*second.m[1][column];
            const float c=first.m[row][2]*second.m[2][column], d=first.m[row][3]*second.m[3][column];
            result.m[row][column]=((a+b)+c)+d;
            continue;
        }
        const auto even=number(first.m[row][0])*number(second.m[0][column])+number(first.m[row][2])*number(second.m[2][column]);
        const auto odd=number(first.m[row][1])*number(second.m[1][column])+number(first.m[row][3])*number(second.m[3][column]);
        result.m[row][column]=(even+odd).to_float();
    }
    output=result;
}

void GraphicsMath::rotation(Matrix4& output, u32 axis, float radians) {
    output.identity();
    if (axis>=3) return;
    float sin,cos;
    if (arithmetic_path==GraphicsArithmetic::Simd && axis<2) rotation_sine_cosine(radians,sin,cos);
    else { sin=sine(radians).to_float(); cos=cosine(radians).to_float(); }
    const u32 a=(axis+1)%3, b=(axis+2)%3;
    output.m[a][a]=cos; output.m[b][b]=cos;
    output.m[a][b]=sin; output.m[b][a]=-sin;
}

void GraphicsMath::normalize(Vec3& output, const Vec3& input) {
    const auto v=input;
    if (arithmetic_path==GraphicsArithmetic::Simd) {
        const float square=(v.x*v.x+v.y*v.y)+v.z*v.z;
        if (!(square>=0x1p-46f)) { output={}; return; }
        // RCP/RSQRT estimate seeds vary by processor. Preserve the reference
        // browser runtime's seed and the original single-precision refinement.
        // Purple 0048D452 / 0048F032 intercept only the SIMD RSQRT seed;
        // retain retail guards, refinement order and scalar dispatch unchanged.
        const float seed=practice_rsqrt_enabled?PracticeRsqrtTables::lookup(square,practice_rsqrt_cpu):1.0f/std::sqrt(square);
        const float product=(square*seed)*seed;
        const float scale=(.5f*seed)*(3.0f-product);
        output={scale*v.x,scale*v.y,scale*v.z};
        return;
    }
    const auto extended_square=number(v.x)*number(v.x)+number(v.y)*number(v.y)+number(v.z)*number(v.z);
    const u32 square_bits=to_bits(extended_square.to_float());
    if (!square_bits) { output={}; return; }
    if ((to_bits((extended_square-number(1)).to_float())&0x7fffffffu)<=0x3727c5ac) { output=v; return; }
    const u32 index=(square_bits>>12&0xff8)/4;
    const auto mantissa=number(from_bits((square_bits&0xffffff)|0x3f000000));
    const auto coefficient=number(from_bits(reciprocal_sqrt_coefficients[index]));
    const auto intercept=number(from_bits(reciprocal_sqrt_coefficients[index+1]));
    const auto exponent=number(from_bits(((0xbeffffffu-square_bits)>>1)&0xff800000));
    const auto scale=(mantissa*coefficient+intercept)*exponent;
    output={(scale*number(v.x)).to_float(),(scale*number(v.y)).to_float(),(scale*number(v.z)).to_float()};
}

void GraphicsMath::quaternion(Matrix4& output,const float* q) {
    // D3DX 8 4781cd: both dispatch modes use this implementation. Its
    // mixed stored/extended products also apply to non-unit quaternions.
    const auto x=number(q[0]),y=number(q[1]),z=number(q[2]),w=number(q[3]);
    const auto x2=number((number(2)*x).to_float()),y2=number((number(2)*y).to_float()),z2=number(2)*z;
    const auto xx=number((x2*x).to_float()),xy=number((y2*x).to_float()),xz=number((z2*x).to_float());
    const auto yy=number((y2*y).to_float()),yz=number((z2*y).to_float()),zz=z2*z;
    const auto xw=x2*w,yw=y2*w,zw=z2*w,one_minus_xx=number(1)-xx;
    output={{{(number(1)-yy-zz).to_float(),(xy+zw).to_float(),(xz-yw).to_float(),0},
             {(xy-zw).to_float(),(one_minus_xx-zz).to_float(),(yz+xw).to_float(),0},
             {(xz+yw).to_float(),(yz-xw).to_float(),(number(one_minus_xx.to_float())-yy).to_float(),0},
             {0,0,0,1}}};
}

void GraphicsMath::look_at(Matrix4& output, const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 z{Scalar::sub(target.x,eye.x),Scalar::sub(target.y,eye.y),Scalar::sub(target.z,eye.z)};
    normalize(z,z);
    Vec3 x=cross(up,z); normalize(x,x);
    // D3DX stores Y in the matrix while retaining extended values for its
    // translation dot product, so do not reload those three stored floats.
    const auto yx=number(z.y)*number(x.z)-number(z.z)*number(x.y);
    const auto yy=number(z.z)*number(x.x)-number(z.x)*number(x.z);
    const auto yz=number(z.x)*number(x.y)-number(z.y)*number(x.x);
    output={{{x.x,yx.to_float(),z.x,0},{x.y,yy.to_float(),z.y,0},{x.z,yz.to_float(),z.z,0},
             {negative_dot(x,eye).to_float(),(-(yy*number(eye.y)+yx*number(eye.x)+yz*number(eye.z))).to_float(),negative_dot(z,eye).to_float(),1}}};
}

void GraphicsMath::perspective(Matrix4& output, float fov, float aspect, float near_plane, float far_plane) {
    const float half=Scalar::mul(fov,.5f);
    const auto vertical=number(cosine(half).to_float())/number(sine(half).to_float());
    const auto depth=number(far_plane)/(number(far_plane)-number(near_plane));
    output={}; output.m[0][0]=(vertical/number(aspect)).to_float(); output.m[1][1]=vertical.to_float();
    output.m[2][2]=depth.to_float(); output.m[2][3]=1;
    output.m[3][2]=(-(depth*number(near_plane))).to_float();
}

void GraphicsMath::transform_coordinate(Vec3& output, const Vec3& input, const Matrix4& matrix) {
    const auto v=input;
    float result[4];
    if (arithmetic_path==GraphicsArithmetic::Simd) {
        for (u32 column=0; column<4; ++column) result[column]=(v.x*matrix.m[0][column]+v.y*matrix.m[1][column])+(v.z*matrix.m[2][column]+matrix.m[3][column]);
        const float seed=1.0f/result[3], product=seed*result[3];
        const float reciprocal=(seed+seed)-(product*seed);
        output={result[0]*reciprocal,result[1]*reciprocal,result[2]*reciprocal};
        return;
    }
    for (u32 column=0; column<4; ++column)
        result[column]=(number(v.x)*number(matrix.m[0][column])+number(v.y)*number(matrix.m[1][column])+number(v.z)*number(matrix.m[2][column])+number(matrix.m[3][column])).to_float();
    if (!almost_one(result[3])) {
        const auto reciprocal=number(1)/number(result[3]);
        for (u32 i=0; i<3; ++i) result[i]=(reciprocal*number(result[i])).to_float();
    }
    std::memcpy(&output,result,sizeof(output));
}

void GraphicsMath::compose_projection(Matrix4& transform,const Matrix4* projection,const Matrix4* view,const Matrix4* world){
    transform.identity(); bool populated=false;
    for (const auto* matrix:{world,view,projection}) if (matrix) {
        if (populated) multiply(transform,transform,*matrix);
        else { transform=*matrix; populated=true; }
    }
}
void GraphicsMath::project_composed(Vec3& output,const Vec3& input,const Viewport* viewport,const Matrix4& transform){
    transform_coordinate(output,input,transform);
    if (viewport) {
        const auto& v=*viewport;
        Vec3 mapped;
        if(viewport_compound(mapped,output,v)){
            output=mapped;
#ifdef TH_PROJECTION_FASTPATH_TEST
            ++test_projection_stats.viewport_hits;
#endif
            return;
        }
#ifdef TH_PROJECTION_FASTPATH_TEST
        ++test_projection_stats.viewport_fallbacks;
#endif
        output.x=((number(output.x)+number(1))*Extended::from_int64(v.width)*number(.5f)+Extended::from_int64(v.x)).to_float();
        output.y=((number(1)-number(output.y))*Extended::from_int64(v.height)*number(.5f)+Extended::from_int64(v.y)).to_float();
        output.z=((number(v.max_z)-number(v.min_z))*number(output.z)+number(v.min_z)).to_float();
    }
}
void GraphicsMath::project(Vec3& output, const Vec3& input, const Viewport* viewport,
                           const Matrix4* projection, const Matrix4* view, const Matrix4* world) {
    Matrix4 transform;compose_projection(transform,projection,view,world);
    project_composed(output,input,viewport,transform);
}
void GraphicsMath::project_translation(Vec3& output,const Vec3& input,const Vec3& position,
                                       const Viewport* viewport,const Matrix4& projection,const Matrix4& view) {
    Matrix4 transform;
    const bool finite_depth=!viewport||(finite_bits(to_bits(viewport->min_z))&&finite_bits(to_bits(viewport->max_z)));
    if(finite_vector(input)&&finite_depth&&compose_translation(transform,position,projection,view)){
        project_composed(output,input,viewport,transform);
        return;
    }
#ifdef TH_PROJECTION_FASTPATH_TEST
    ++test_projection_stats.translation_fallbacks;
#endif
    Matrix4 world;world.identity();
    world.m[3][0]=position.x;world.m[3][1]=position.y;world.m[3][2]=position.z;
    project(output,input,viewport,&projection,&view,&world);
}
}
