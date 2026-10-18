/* Entry: 0x0054f9d8; symbol: to_floating_point_length; body bytes: 35 */

/* Library Function - Single Match
    unsigned int __cdecl __crt_stdio_input::to_floating_point_length(enum
   __crt_stdio_input::length_modifier)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

uint __cdecl __crt_stdio_input::to_floating_point_length(length_modifier param_1)

{
  undefined4 uStack_8;
  
  if (param_1 == 0) {
    uStack_8 = 4;
  }
  else {
    if ((param_1 != 3) && (param_1 != 8)) {
      return 0;
    }
    uStack_8 = 8;
  }
  return uStack_8;
}

