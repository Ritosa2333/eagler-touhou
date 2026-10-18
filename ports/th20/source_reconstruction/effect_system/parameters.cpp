#include "effect.hpp"
namespace th20::source::effects {
void construct_parameters(Parameters& p) noexcept {p.vector_00=p.vector_0c=p.vector_2c={};p.value_18=p.value_1c=p.value_20=p.value_24=0;p.enabled=1;}
void construct_request(Request& r) noexcept {r.type=-1;r.original_parameters=nullptr;r.animation=nullptr;r.delay=0;construct_parameters(r.parameters);}
}
