/* Entry: 0x0050a930; symbol: FUN_0050a930; body bytes: 93 */

undefined4 * __cdecl FUN_0050a930(undefined4 param_1,undefined1 (*param_2) [16])

{
  undefined4 *puVar1;
  undefined4 *this;
  int iVar2;
  
  this = FUN_00507100();
  iVar2 = FUN_00508480(this,param_1,param_2);
  puVar1 = this;
  if (iVar2 != 0) {
    if (this != (undefined4 *)0x0) {
      FUN_0041f7c0(this);
    }
    this = (undefined4 *)0x0;
    puVar1 = DAT_005c60fc;
  }
  DAT_005c60fc = puVar1;
  return this;
}

