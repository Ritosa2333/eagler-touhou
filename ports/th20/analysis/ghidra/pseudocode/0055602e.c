/* Entry: 0x0055602e; symbol: __configthreadlocale; body bytes: 98 */

/* Library Function - Single Match
    __configthreadlocale
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl __configthreadlocale(int _Flag)

{
  uint uVar1;
  int iVar2;
  __acrt_ptd *p_Var3;
  uint uVar4;
  
  iVar2 = FUN_005579c2();
  uVar1 = *(uint *)(iVar2 + 0x350);
  if (_Flag == -1) {
    DAT_005b29f8 = 0xffffffff;
  }
  else if (_Flag != 0) {
    if (_Flag == 1) {
      uVar4 = uVar1 | 2;
    }
    else {
      if (_Flag != 2) {
        p_Var3 = FUN_005516c1();
        *(undefined4 *)p_Var3 = 0x16;
        FUN_005480bc();
        return -1;
      }
      uVar4 = uVar1 & 0xfffffffd;
    }
    *(uint *)(iVar2 + 0x350) = uVar4;
  }
  return ((uVar1 & 2) == 0) + 1;
}

