#include "AnmVariables.hpp"
namespace th15 {
i32 AnmVariables::integer(i32 value,Rng& rng)noexcept{
    if(value>=10000&&value<=10003)return integers[value-10000];
    if(value>=10004&&value<=10007)return truncate_int(floats[value-10004]);
    if(value==10008||value==10009)return extra_integers[value-10008];
    if(value==10022)return random_bound?signed_bits(rng.next32()%random_bound):0;
    if(value==10027)return truncate_int(random_scale);if(value==10028)return truncate_int(random_angle);if(value==10029)return signed_bits(random_bound);
    if(value>=10033&&value<=10035)return truncate_int(vector[value-10033]);return value;
}
float AnmVariables::floating(float value,Rng& rng)noexcept{
    const i32 id=truncate_int(value);
    if(id>=10000&&id<=10003)return float(integers[id-10000]);
    if(id>=10004&&id<=10007)return floats[id-10004];
    if(id==10008||id==10009)return float(extra_integers[id-10008]);
    if(id==10010)return float(rng.signed_unit()*random_angle);
    if(id==10011)return float(rng.unit()*random_scale);
    if(id==10012)return float(rng.signed_unit()*random_scale);
    if(id>=10013&&id<=10015){const float p[]={position.x,position.y,position.z};return p[id-10013];}
    if(id==10022)return float(rng.next32());
    if(id>=10023&&id<=10025){const float p[]={rotation.x,rotation.y,rotation.z};return p[id-10023];}
    if(id==10027)return random_scale;if(id==10028)return random_angle;if(id==10029)return float(random_bound);
    if(id>=10033&&id<=10035)return vector[id-10033];return value;
}
i32* AnmVariables::integer_destination(i32* argument)noexcept{const i32 id=*argument;if(id>=10000&&id<=10003)return integers+id-10000;if(id==10008||id==10009)return extra_integers+id-10008;if(id==10029)return reinterpret_cast<i32*>(&random_bound);return argument;}
float* AnmVariables::float_destination(float* argument)noexcept{const i32 id=truncate_int(*argument);if(id>=10004&&id<=10007)return floats+id-10004;if(id==10013)return &position.x;if(id==10014)return &position.y;if(id==10015)return &position.z;if(id==10023)return &rotation.x;if(id==10024)return &rotation.y;if(id==10025)return &rotation.z;if(id==10027)return &random_scale;if(id==10028)return &random_angle;if(id>=10033&&id<=10035)return vector+id-10033;return argument;}
}
