/* Entry: 0x0055be82; symbol: FID_conflict:__get_dstbias; body bytes: 44 */

/* Library Function - Multiple Matches With Different Base Names
    __get_daylight
    __get_dstbias
    __get_timezone
   
   Library: Visual Studio 2019 Release */

errno_t __cdecl FID_conflict___get_dstbias(long *_Timezone)

{
  __acrt_ptd *p_Var1;
  
  if (_Timezone == (long *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  *_Timezone = DAT_005e58a0;
  return 0;
}

