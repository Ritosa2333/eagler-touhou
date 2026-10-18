/* Entry: 0x004c6130; symbol: FUN_004c6130; body bytes: 43 */

void __thiscall FUN_004c6130(void *this,int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar2 = &DAT_005b9eb0;
  puVar3 = (undefined4 *)((int)this + param_1 * 0x30 + 0x2e38);
  for (iVar1 = 0xc; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  return;
}

