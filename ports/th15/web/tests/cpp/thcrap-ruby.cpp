#include "../../cpp/game/ThcrapRuby.hpp"
#include <cassert>
int main(){
 th15::ThcrapRuby ruby;
 assert(th15::parse_thcrap_ruby("|\t\t,\tEarth\t,This place",ruby));
 assert(ruby.begin.empty()&&ruby.base=="Earth"&&ruby.annotation=="This place");
 assert(th15::parse_thcrap_ruby("|\tWell, \t,\t地球\t,Earth, here",ruby));
 assert(ruby.begin=="Well, "&&ruby.base=="地球"&&ruby.annotation=="Earth, here");
 assert(!th15::parse_thcrap_ruby("|2,3,original",ruby));
 assert(!th15::parse_thcrap_ruby("|\tbad",ruby));
 assert(!th15::parse_thcrap_ruby("|\t\t;\tEarth\t,This place",ruby));
 assert(th15::thcrap_ruby_offset(0,60,100)==-15);
 assert(th15::thcrap_ruby_offset(50,61,20)==75);
 assert(th15::thcrap_bubble_x(400,300,false)==321);
 assert(th15::thcrap_bubble_x(200,300,true)==319);
 assert(th15::thcrap_bubble_x(100,900,false)==100);
 assert(th15::thcrap_bubble_x(620,900,true)==620);
}
