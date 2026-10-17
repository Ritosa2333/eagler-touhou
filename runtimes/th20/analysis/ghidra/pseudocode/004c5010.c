/* Entry: 0x004c5010; symbol: FUN_004c5010; body bytes: 90 */

undefined4 * __cdecl FUN_004c5010(int param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_004c2130();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_004c3c00(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

