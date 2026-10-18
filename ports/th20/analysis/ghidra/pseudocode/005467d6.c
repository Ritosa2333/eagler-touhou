/* Entry: 0x005467d6; symbol: ___vcrt_getptd; body bytes: 14 */

/* Library Function - Single Match
    ___vcrt_getptd
   
   Library: Visual Studio 2019 Release */

void ___vcrt_getptd(void)

{
  code *pcVar1;
  LPVOID pvVar2;
  int iVar3;
  BOOL BVar4;
  
  pvVar2 = ___vcrt_getptd_noexit();
  if (pvVar2 != (LPVOID)0x0) {
    return;
  }
  iVar3 = ___acrt_get_sigabrt_handler();
  if (iVar3 != 0) {
    FUN_005573ea(0x16);
  }
  if ((DAT_005b2650 & 2) != 0) {
    BVar4 = IsProcessorFeaturePresent(0x17);
    if (BVar4 != 0) {
      pcVar1 = (code *)swi(0x29);
      (*pcVar1)();
    }
    ___acrt_call_reportfault(3,0x40000015,1);
  }
                    /* WARNING: Subroutine does not return */
  __exit(3);
}

