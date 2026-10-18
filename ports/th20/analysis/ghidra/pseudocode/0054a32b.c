/* Entry: 0x0054a32b; symbol: unget; body bytes: 46 */

/* Library Function - Single Match
    public: void __thiscall __crt_strtox::c_string_character_source<wchar_t>::unget(wchar_t)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
__crt_strtox::c_string_character_source<wchar_t>::unget
          (c_string_character_source<wchar_t> *this,wchar_t param_1)

{
  int iVar1;
  __acrt_ptd *p_Var2;
  
  iVar1 = *(int *)this;
  *(wchar_t **)this = (wchar_t *)(iVar1 + -2);
  if ((param_1 != L'\0') && (*(wchar_t *)(iVar1 + -2) != param_1)) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
  }
  return;
}

