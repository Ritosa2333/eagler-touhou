/* Entry: 0x004c1c10; symbol: FUN_004c1c10; body bytes: 54 */

undefined4 * __thiscall FUN_004c1c10(void *this,undefined4 *param_1)

{
  int iVar1;
  
  iVar1 = FUN_004c0e10((undefined4 *)this);
  if (iVar1 != 0) {
    iVar1 = FUN_004c0e10((undefined4 *)this);
    FUN_0047a4a0(iVar1);
  }
                    /* WARNING: Load size is inaccurate */
  *param_1 = *this;
  return param_1;
}

