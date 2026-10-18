/* Entry: 0x0054a5fd; symbol: common_vsscanf<wchar_t>; body bytes: 153 */

/* Library Function - Single Match
    int __cdecl common_vsscanf<wchar_t>(unsigned __int64,wchar_t const * const,unsigned int,wchar_t
   const * const,struct __crt_locale_pointers * const,char * const)
   
   Library: Visual Studio 2019 Release */

int __cdecl
common_vsscanf<wchar_t>
          (__uint64 param_1,wchar_t *param_2,uint param_3,wchar_t *param_4,
          __crt_locale_pointers *param_5,char *param_6)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  uint uVar3;
  char *unaff_ESI;
  input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> local_80 [76];
  LPVOID local_34;
  int local_20;
  wchar_t local_1c [4];
  char local_14;
  wchar_t *local_10;
  wchar_t *local_c;
  wchar_t *local_8;
  
  if ((param_2 == (wchar_t *)0x0) || (param_4 == (wchar_t *)0x0)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    iVar2 = -1;
  }
  else {
    uVar3 = FUN_00548430(param_2,param_3);
    FUN_0054c44c(&local_20,(int *)param_5);
    local_c = param_2 + uVar3;
    local_10 = param_2;
    local_8 = param_2;
    __crt_stdio_input::
    input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>::
    input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
              (local_80,(string_input_adapter<wchar_t> *)&local_10,CONCAT44(param_4,param_1._4_4_),
               local_1c,(__crt_locale_pointers *)param_6,unaff_ESI);
    iVar2 = FUN_0054efeb(local_80);
    FUN_00557ba0(local_34);
    if (local_14 != '\0') {
      *(uint *)(local_20 + 0x350) = *(uint *)(local_20 + 0x350) & 0xfffffffd;
    }
  }
  return iVar2;
}

