/* Entry: 0x00555a57; symbol: __set_fmode; body bytes: 61 */

/* Library Function - Single Match
    __set_fmode
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

errno_t __cdecl __set_fmode(int _Mode)

{
  __acrt_ptd *p_Var1;
  
  if (((_Mode != 0x4000) && (_Mode != 0x8000)) && (_Mode != 0x10000)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  LOCK();
  DAT_005e5994 = _Mode;
  UNLOCK();
  return 0;
}

