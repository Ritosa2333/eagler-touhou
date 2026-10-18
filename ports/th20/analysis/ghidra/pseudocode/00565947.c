/* Entry: 0x00565947; symbol: __mbsdec_l; body bytes: 141 */

/* Library Function - Single Match
    __mbsdec_l
   
   Library: Visual Studio 2019 Release */

uchar * __cdecl __mbsdec_l(uchar *_Start,uchar *_Pos,_locale_t _Locale)

{
  __acrt_ptd *p_Var1;
  byte *pbVar2;
  int local_14 [2];
  int local_c;
  char local_8;
  
  if (_Start == (uchar *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return (uchar *)0x0;
  }
  if (_Pos == (uchar *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
  }
  else if (_Start < _Pos) {
    FUN_0054c44c(local_14,(int *)_Locale);
    pbVar2 = _Pos + -1;
    if (*(int *)(local_c + 8) != 0) {
      do {
        pbVar2 = pbVar2 + -1;
        if (pbVar2 < _Start) break;
      } while ((*(byte *)(*pbVar2 + 0x19 + local_c) & 4) != 0);
      pbVar2 = _Pos + (-1 - ((int)_Pos - (int)pbVar2 & 1U));
    }
    if (local_8 == '\0') {
      return pbVar2;
    }
    *(uint *)(local_14[0] + 0x350) = *(uint *)(local_14[0] + 0x350) & 0xfffffffd;
    return pbVar2;
  }
  return (uchar *)0x0;
}

