/* Entry: 0x004c7cb0; symbol: FUN_004c7cb0; body bytes: 90 */

undefined4 * __cdecl FUN_004c7cb0(int *param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_004c5410();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_004c58d0(this,param_1);
    if (iVar1 != 0) {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

