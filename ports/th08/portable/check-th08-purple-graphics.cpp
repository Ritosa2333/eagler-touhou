#include "../th08_web/cpp/game/GraphicsMath.hpp"
#include "../th08_web/cpp/game/PracticeRsqrtTables.hpp"
#include <cassert>
#include <cmath>
#include <cstring>
using namespace th08;
static bool same(const Vec3& a,const Vec3& b){return std::memcmp(&a,&b,sizeof(Vec3))==0;}
int main(){
 GraphicsMath::arithmetic(GraphicsArithmetic::Simd);
 for(Vec3 input: {Vec3{3,4,7},Vec3{.125f,-9,2},Vec3{1,0,0}}){
  Vec3 original;GraphicsMath::practice_rsqrt(false,'i');GraphicsMath::normalize(original,input);
  const float square=(input.x*input.x+input.y*input.y)+input.z*input.z;
  for(char cpu:{'i','a','c'}){
   GraphicsMath::practice_rsqrt(true,cpu);Vec3 output;GraphicsMath::normalize(output,input);
   const float seed=PracticeRsqrtTables::lookup(square,cpu);
   const float product=(square*seed)*seed;const float scale=(.5f*seed)*(3.f-product);
   const Vec3 expected{scale*input.x,scale*input.y,scale*input.z};assert(same(output,expected));
  }
  GraphicsMath::practice_rsqrt(false,'i');Vec3 restored;GraphicsMath::normalize(restored,input);assert(same(restored,original));
 }
 GraphicsMath::arithmetic(GraphicsArithmetic::Scalar);
 Vec3 a,b;GraphicsMath::practice_rsqrt(false,'i');GraphicsMath::normalize(a,{2,3,4});
 GraphicsMath::practice_rsqrt(true,'a');GraphicsMath::normalize(b,{2,3,4});assert(same(a,b));
 GraphicsMath::arithmetic(GraphicsArithmetic::Simd);GraphicsMath::normalize(b,{});assert(same(b,Vec3{}));
 GraphicsMath::practice_rsqrt(false,'i');
}
