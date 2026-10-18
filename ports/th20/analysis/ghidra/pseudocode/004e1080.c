/* Entry: 0x004e1080; symbol: FUN_004e1080; body bytes: 90 */

undefined4 * __cdecl FUN_004e1080(int *param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_004dfc00();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_004e0e80(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

