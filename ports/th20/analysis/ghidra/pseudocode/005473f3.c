/* Entry: 0x005473f3; symbol: FUN_005473f3; body bytes: 60 */

/* WARNING: Function: __EH_prolog3_catch replaced with injection: EH_prolog3 */

void FUN_005473f3(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = ___vcrt_getptd();
  if (*(int *)(iVar1 + 0x1c) == 0) {
    unexpected();
    iVar1 = ___vcrt_getptd();
    *(undefined4 *)(iVar1 + 0x1c) = param_1;
    FUN_00544a7c((int *)0x0,(byte *)0x0);
  }
                    /* WARNING: Subroutine does not return */
  _abort();
}

