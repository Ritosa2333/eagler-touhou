/* Entry: 0x004aba70; symbol: FUN_004aba70; body bytes: 94 */

undefined4 * __cdecl FUN_004aba70(int param_1,undefined4 param_2)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_004a28c0();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_004a7080(this,param_1,param_2);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

