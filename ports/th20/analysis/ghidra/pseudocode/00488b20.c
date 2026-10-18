/* Entry: 0x00488b20; symbol: FUN_00488b20; body bytes: 90 */

undefined4 * __cdecl FUN_00488b20(int param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_00486a80();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_00487b10(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

