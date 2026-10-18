/* Entry: 0x004c2060; symbol: FUN_004c2060; body bytes: 33 */

void __thiscall FUN_004c2060(void *this,undefined4 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)((int)this + 0x34);
  for (iVar1 = 0x12; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_1;
    param_1 = param_1 + 1;
    puVar2 = puVar2 + 1;
  }
  return;
}

