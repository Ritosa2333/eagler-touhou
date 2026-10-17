#include "../../th08_web/cpp/game/Arithmetic.hpp"
#include "../../th10_web/cpp/game/Arithmetic.hpp"
#include "ExactFloat.hpp"
#include <cstring>
#include <initializer_list>
using E=th08::Extended;
struct F32 { std::uint32_t value; };struct F64 { std::uint64_t value; };
extern "C" {
void f32_to_extF80M(F32,E*);void f64_to_extF80M(F64,E*);void i64_to_extF80M(std::int64_t,E*);
F32 extF80M_to_f32(const E*);F64 extF80M_to_f64(const E*);
std::int64_t extF80M_to_i64(const E*,std::uint8_t,bool);
void extF80M_add(const E*,const E*,E*);void extF80M_sub(const E*,const E*,E*);void extF80M_mul(const E*,const E*,E*);void extF80M_div(const E*,const E*,E*);void extF80M_sqrt(const E*,E*);
bool extF80M_lt_quiet(const E*,const E*);bool extF80M_eq(const E*,const E*);
}
static std::uint32_t random_state=0x5831027;
static std::uint32_t random_u32(){random_state^=random_state<<13;random_state^=random_state>>17;random_state^=random_state<<5;return random_state;}
static bool same(const E& a,const E& b){return a.exponent==b.exponent&&a.significand==b.significand;}
static E cast(const th10::Extended& a){E b;std::memcpy(&b,&a,sizeof(b));return b;}
static std::uint64_t double_bits(double f){std::uint64_t u;std::memcpy(&u,&f,8);return u;}
static std::uint32_t failure[12]{};static std::uint32_t checks=0;
static bool check(const E& actual,const E& expected,int kind,std::uint32_t a,std::uint32_t b){checks++;if(same(actual,expected))return true;failure[0]=kind;failure[1]=a;failure[2]=b;std::memcpy(failure+3,&actual,16);std::memcpy(failure+7,&expected,16);return false;}
extern "C" __attribute__((export_name("details"))) std::uint32_t* details(){return failure;}
extern "C" __attribute__((export_name("checks"))) std::uint32_t count(){return checks;}
extern "C" __attribute__((export_name("verify"))) int verify(std::uint32_t iterations){
    const std::uint32_t special[]{0,0x80000000,1,0x807fffff,0x00800000,0x00800001,0x00ffffff,0x3f7fffff,0x3f800000,0xbf800000,0x3f800001,0x7f7fffff,0x7f800000,0xff800000,0x7fc00000,0x7f800001};
    for(const auto precision:{th08::Precision::Single,th08::Precision::Double,th08::Precision::Extended})for(int rounding=0;rounding<4;rounding++){
        th08::arithmetic_mode(precision,th08::Rounding(rounding));
        for(std::uint32_t i=0;i<iterations;i++){
            auto ua=i<256?special[i%16]:random_u32(),ub=i<256?special[i/16]:random_u32();
            // Half the cases cover the normal gameplay range, half cover all IEEE exponents.
            if(i>=256&&(i&1)){ua=(ua&0x807fffff)|((118+i%22)<<23);ub=(ub&0x807fffff)|((118+i%19)<<23);}
            const float fa=touhou::numeric::value(ua),fb=touhou::numeric::value(ub);E a,b,reference;
            f32_to_extF80M({ua},&a);f32_to_extF80M({ub},&b);
            const auto aa=E::from_float(fa),bb=E::from_float(fb);const auto ca=th10::Extended::from_float(fa),cb=th10::Extended::from_float(fb);
            if(!check(aa,a,1,ua,ub)||!check(cast(ca),a,2,ua,ub))return 1;
            for(int op=0;op<4;op++){
                E actual;th10::Extended other;
                switch(op){case 0:extF80M_add(&a,&b,&reference);actual=aa+bb;other=ca+cb;break;case 1:extF80M_sub(&a,&b,&reference);actual=aa-bb;other=ca-cb;break;case 2:extF80M_mul(&a,&b,&reference);actual=aa*bb;other=ca*cb;break;default:extF80M_div(&a,&b,&reference);actual=aa/bb;other=ca/cb;break;}
                if(!check(actual,reference,10+op,ua,ub)||!check(cast(other),reference,20+op,ua,ub))return 2;
                float scalar,other_scalar;
                switch(op){case 0:scalar=th08::Scalar::add(fa,fb);other_scalar=th10::Scalar::add(fa,fb);break;case 1:scalar=th08::Scalar::sub(fa,fb);other_scalar=th10::Scalar::sub(fa,fb);break;case 2:scalar=th08::Scalar::mul(fa,fb);other_scalar=th10::Scalar::mul(fa,fb);break;default:scalar=th08::Scalar::div(fa,fb);other_scalar=th10::Scalar::div(fa,fb);break;}
                checks+=2;if(touhou::numeric::bits(scalar)!=extF80M_to_f32(&reference).value||touhou::numeric::bits(other_scalar)!=extF80M_to_f32(&reference).value){failure[0]=80+op;failure[1]=ua;failure[2]=ub;return 8;}
                if(touhou::numeric::bits(actual.to_float())!=extF80M_to_f32(&reference).value||double_bits(actual.to_double())!=extF80M_to_f64(&reference).value){failure[0]=30+op;return 3;}
            }
            extF80M_sqrt(&a,&reference);if(!check(aa.square_root(),reference,40,ua,ub)||!check(cast(ca.square_root()),reference,41,ua,ub))return 4;
            const auto count=static_cast<std::int32_t>(i&1?random_u32()%100000:random_u32());
            E integer,product,total;i64_to_extF80M(count,&integer);extF80M_mul(&integer,&b,&product);
            checks+=2;
            if(touhou::numeric::bits(th08::Scalar::mul_int(count,fb))!=extF80M_to_f32(&product).value||touhou::numeric::bits(th10::Scalar::mul_int(count,fb))!=extF80M_to_f32(&product).value){failure[0]=91;return 10;}
            checks+=2;
            if(std::uint32_t(th08::Scalar::mul_int_truncate(count,fb))!=std::uint32_t(extF80M_to_i64(&product,1,false))||std::uint32_t(th10::Scalar::mul_int_truncate(count,fb))!=std::uint32_t(extF80M_to_i64(&product,1,false))){failure[0]=92;return 11;}
            extF80M_add(&product,&a,&total);checks+=2;
            if(touhou::numeric::bits(th08::Scalar::add_scaled(fa,count,fb))!=extF80M_to_f32(&total).value||touhou::numeric::bits(th10::Scalar::add_scaled(fa,count,fb))!=extF80M_to_f32(&total).value){failure[0]=93;return 12;}
            extF80M_sub(&a,&product,&total);checks+=2;
            if(touhou::numeric::bits(th08::Scalar::sub_scaled(fa,count,fb))!=extF80M_to_f32(&total).value||touhou::numeric::bits(th10::Scalar::sub_scaled(fa,count,fb))!=extF80M_to_f32(&total).value){failure[0]=94;return 13;}
            checks+=2;if(std::uint32_t(th08::Scalar::truncate(fa))!=std::uint32_t(extF80M_to_i64(&a,1,false))||std::uint32_t(th10::Scalar::truncate(fa))!=std::uint32_t(extF80M_to_i64(&a,1,false))){failure[0]=90;failure[1]=ua;return 9;}
            if((aa<bb)!=extF80M_lt_quiet(&a,&b)||(aa==bb)!=extF80M_eq(&a,&b)||(ca<cb)!=extF80M_lt_quiet(&a,&b)||(ca==cb)!=extF80M_eq(&a,&b)){failure[0]=50;return 5;}
            const std::uint64_t u=(std::uint64_t(random_u32())<<32)|random_u32();double d;std::memcpy(&d,&u,8);f64_to_extF80M({u},&reference);
            if(!check(E::from_double(d),reference,60,ua,ub)||!check(cast(th10::Extended::from_double(d)),reference,61,ua,ub))return 6;
            const auto n=std::int64_t(u);i64_to_extF80M(n,&reference);if(!check(E::from_int64(n),reference,70,ua,ub)||!check(cast(th10::Extended::from_int64(n)),reference,71,ua,ub))return 7;
        }
    }return 0;
}
