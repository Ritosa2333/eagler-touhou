/* Entry: 0x00548a4b; symbol: make_c_string_character_source<>; body bytes: 27 */

/* Library Function - Multiple Matches With Same Base Name
    class __crt_strtox::c_string_character_source<char> __cdecl
   __crt_strtox::make_c_string_character_source<char,std::nullptr_t>(char const *
   const,std::nullptr_t)
    class __crt_strtox::c_string_character_source<char> __cdecl
   __crt_strtox::make_c_string_character_source<char,char * *>(char const * const,char * * const)
    class __crt_strtox::c_string_character_source<wchar_t> __cdecl
   __crt_strtox::make_c_string_character_source<wchar_t,std::nullptr_t>(wchar_t const *
   const,std::nullptr_t)
    class __crt_strtox::c_string_character_source<wchar_t> __cdecl
   __crt_strtox::make_c_string_character_source<wchar_t,wchar_t * *>(wchar_t const * const,wchar_t *
   * const)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl
make_c_string_character_source<>(undefined4 *param_1,undefined4 param_2,undefined4 *param_3)

{
  *param_1 = param_2;
  param_1[1] = param_3;
  if (param_3 != (undefined4 *)0x0) {
    *param_3 = param_2;
  }
  return;
}

