/* Entry: 0x0049e0c0; symbol: FUN_0049e0c0; body bytes: 90 */

undefined4 * __cdecl FUN_0049e0c0(int param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_0049cc90();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_0049d790(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

