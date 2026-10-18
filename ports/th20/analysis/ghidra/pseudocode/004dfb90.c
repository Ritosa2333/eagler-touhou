/* Entry: 0x004dfb90; symbol: FUN_004dfb90; body bytes: 98 */

undefined4 * __cdecl FUN_004dfb90(undefined4 param_1)

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_004dea10();
  if (this == (undefined4 *)0x0) {
    this = (undefined4 *)0x0;
  }
  else {
    iVar1 = FUN_004df670((int)this);
    if (iVar1 == 0) {
      FUN_0049c490(this,param_1);
    }
    else {
      if (this != (undefined4 *)0x0) {
        FUN_0041f7c0(this);
      }
      this = (undefined4 *)0x0;
    }
  }
  return this;
}

