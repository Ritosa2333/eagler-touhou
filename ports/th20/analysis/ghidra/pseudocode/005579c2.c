/* Entry: 0x005579c2; symbol: FUN_005579c2; body bytes: 14 */

void FUN_005579c2(void)

{
  code *pcVar1;
  int iVar2;
  BOOL BVar3;
  __acrt_ptd *p_Var4;
  
  p_Var4 = FUN_00557a13();
  if (p_Var4 != (__acrt_ptd *)0x0) {
    return;
  }
  iVar2 = ___acrt_get_sigabrt_handler();
  if (iVar2 != 0) {
    FUN_005573ea(0x16);
  }
  if ((DAT_005b2650 & 2) != 0) {
    BVar3 = IsProcessorFeaturePresent(0x17);
    if (BVar3 != 0) {
      pcVar1 = (code *)swi(0x29);
      (*pcVar1)();
    }
    ___acrt_call_reportfault(3,0x40000015,1);
  }
                    /* WARNING: Subroutine does not return */
  __exit(3);
}

