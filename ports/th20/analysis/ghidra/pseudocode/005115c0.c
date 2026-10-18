/* Entry: 0x005115c0; symbol: FUN_005115c0; body bytes: 113 */

undefined4 * __cdecl FUN_005115c0(undefined4 param_1)

{
  undefined4 *this;
  int iVar1;
  
  if (DAT_005c6114 != (undefined4 *)0x0) {
    FUN_004217c0(DAT_005c6114);
  }
  this = FUN_00510930();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_00511270(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

