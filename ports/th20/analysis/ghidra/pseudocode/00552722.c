/* Entry: 0x00552722; symbol: __acrt_mbs_to_wcs_cp<struct___crt_win32_buffer_internal_dynamic_resizing>; body bytes: 29 */

/* Library Function - Single Match
    int __cdecl __acrt_mbs_to_wcs_cp<struct __crt_win32_buffer_internal_dynamic_resizing>(char const
   * const,class __crt_win32_buffer<wchar_t,struct __crt_win32_buffer_internal_dynamic_resizing>
   &,unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

int __cdecl
__acrt_mbs_to_wcs_cp<struct___crt_win32_buffer_internal_dynamic_resizing>
          (char *param_1,
          __crt_win32_buffer<wchar_t,struct___crt_win32_buffer_internal_dynamic_resizing> *param_2,
          uint param_3)

{
  int iVar1;
  <lambda_62f6974d9771e494a5ea317cc32e971c> local_5;
  
  iVar1 = __acrt_convert_wcs_mbs_cp<char,wchar_t,class_<lambda_62f6974d9771e494a5ea317cc32e971c>,struct___crt_win32_buffer_internal_dynamic_resizing>
                    (param_1,param_2,&local_5,param_3);
  return iVar1;
}

