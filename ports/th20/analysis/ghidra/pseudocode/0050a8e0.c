/* Entry: 0x0050a8e0; symbol: FUN_0050a8e0; body bytes: 74 */

undefined4 * __cdecl FUN_0050a8e0(undefined1 (*param_1) [16])

{
  undefined4 *this;
  int iVar1;
  
  this = FUN_00507100();
  iVar1 = FUN_00508480(this,2,param_1);
  if (iVar1 != 0) {
    FUN_0041f7c0(this);
    this = (undefined4 *)0x0;
  }
  return this;
}

