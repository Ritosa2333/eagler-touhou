/* Entry: 0x004f4fd0; symbol: FUN_004f4fd0; body bytes: 74 */

undefined4 * __thiscall FUN_004f4fd0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  
  puVar1 = (undefined4 *)FUN_004ec500();
  puVar3 = (undefined4 *)this;
  for (iVar2 = 0xb; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *puVar1;
    puVar1 = puVar1 + 1;
    puVar3 = puVar3 + 1;
  }
  *(undefined4 *)((int)this + 0x2c) = param_1;
  *(undefined4 *)((int)this + 0x30) = param_2;
  *(undefined4 *)((int)this + 0x34) = 0;
  FUN_004fcf50(this);
  return (undefined4 *)this;
}

