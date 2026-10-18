/* Entry: 0x005468ae; symbol: ___vcrt_initialize_ptd; body bytes: 51 */

/* Library Function - Single Match
    ___vcrt_initialize_ptd
   
   Library: Visual Studio 2019 Release */

uint ___vcrt_initialize_ptd(void)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = ___vcrt_FlsAlloc(FUN_005467ba);
  DAT_005b2630 = uVar1;
  if (uVar1 != 0xffffffff) {
    iVar2 = ___vcrt_FlsSetValue(uVar1,&DAT_005e52a4);
    if (iVar2 != 0) {
      return CONCAT31((int3)((uint)iVar2 >> 8),1);
    }
    uVar1 = FUN_005468e1();
  }
  return uVar1 & 0xffffff00;
}

