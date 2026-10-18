/* Entry: 0x0050a340; symbol: FUN_0050a340; body bytes: 114 */

void __fastcall FUN_0050a340(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  uint local_14;
  
  for (local_14 = 0; local_14 < 0x21; local_14 = local_14 + 1) {
    puVar3 = (undefined4 *)FUN_004ff690(param_1);
    uVar1 = *puVar3;
    uVar2 = puVar3[1];
    puVar3 = (undefined4 *)FUN_0045d670((void *)(param_1 + 0x20fc),local_14);
    *puVar3 = uVar1;
    puVar3[1] = uVar2;
  }
  return;
}

