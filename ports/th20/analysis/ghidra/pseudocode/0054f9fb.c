/* Entry: 0x0054f9fb; symbol: to_integer_length; body bytes: 49 */

/* Library Function - Single Match
    unsigned int __cdecl __crt_stdio_input::to_integer_length(enum
   __crt_stdio_input::length_modifier)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

uint __cdecl __crt_stdio_input::to_integer_length(length_modifier param_1)

{
  uint uStack_8;
  
  switch(param_1) {
  case 0:
  case 3:
  case 6:
  case 7:
  case 9:
    uStack_8 = 4;
    break;
  case 1:
    return 1;
  case 2:
    uStack_8 = 2;
    break;
  case 4:
  case 5:
  case 10:
    uStack_8 = 8;
    break;
  default:
    return 0;
  }
  return uStack_8;
}

