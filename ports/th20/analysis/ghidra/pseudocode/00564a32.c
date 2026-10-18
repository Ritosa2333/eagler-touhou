/* Entry: 0x00564a32; symbol: write_double_translated_unicode_nolock; body bytes: 104 */

/* Library Function - Single Match
    struct `anonymous namespace'::write_result __cdecl write_double_translated_unicode_nolock(char
   const * const,unsigned int)
   
   Library: Visual Studio 2019 Release */

char * __cdecl write_double_translated_unicode_nolock(char *param_1,uint param_2)

{
  wchar_t _WCh;
  wchar_t wVar1;
  wint_t wVar2;
  wchar_t *pwVar3;
  DWORD DVar4;
  int in_stack_0000000c;
  
  param_1[0] = '\0';
  param_1[1] = '\0';
  param_1[2] = '\0';
  param_1[3] = '\0';
  param_1[4] = '\0';
  param_1[5] = '\0';
  param_1[6] = '\0';
  param_1[7] = '\0';
  param_1[8] = '\0';
  param_1[9] = '\0';
  param_1[10] = '\0';
  param_1[0xb] = '\0';
  pwVar3 = (wchar_t *)(in_stack_0000000c + param_2);
  if (param_2 < pwVar3) {
    do {
      _WCh = *(wchar_t *)param_2;
      wVar1 = __putwch_nolock(_WCh);
      if (wVar1 != _WCh) {
LAB_00564a8b:
        DVar4 = GetLastError();
        *(DWORD *)param_1 = DVar4;
        return param_1;
      }
      *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 2;
      if (_WCh == L'\n') {
        wVar2 = __putwch_nolock(L'\r');
        if (wVar2 != 0xd) goto LAB_00564a8b;
        *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 1;
        *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
      }
      param_2 = param_2 + 2;
    } while (param_2 < pwVar3);
  }
  return param_1;
}

