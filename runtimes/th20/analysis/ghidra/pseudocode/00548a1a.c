/* Entry: 0x00548a1a; symbol: is_wide_character_specifier<char>; body bytes: 49 */

/* Library Function - Single Match
    bool __cdecl __crt_stdio_output::is_wide_character_specifier<char>(unsigned __int64,char,enum
   __crt_stdio_output::length_modifier)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

bool __cdecl
__crt_stdio_output::is_wide_character_specifier<char>
          (__uint64 param_1,char param_2,length_modifier param_3)

{
  if ((param_3 != 2) &&
     (((param_3 == 3 || (param_3 == 0xc)) ||
      ((param_3 != 0xd && ((param_2 != 'c' && (param_2 != 's')))))))) {
    return true;
  }
  return false;
}

