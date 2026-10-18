/* Entry: 0x0050f2f0; symbol: FUN_0050f2f0; body bytes: 190 */

void __fastcall FUN_0050f2f0(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  uint *puVar5;
  undefined8 uVar6;
  undefined4 local_2c;
  undefined4 local_28;
  uint local_24;
  uint local_20;
  
  iVar2 = FUN_004640a0(0x5ba568);
  iVar3 = FUN_00474d60(0x5ba568);
  uVar6 = FUN_004b8290(0x5ba568);
  puVar5 = (uint *)(param_1 + 0x76f8 + iVar2 * 0x90 + (iVar3 + -1) * 0x10);
  uVar1 = puVar5[1];
  local_20 = (uint)((ulonglong)uVar6 >> 0x20);
  if ((uVar1 <= local_20) && ((uVar1 < local_20 || (local_24 = (uint)uVar6, *puVar5 < local_24)))) {
    uVar6 = FUN_004b8290(0x5ba568);
    iVar2 = FUN_004640a0(0x5ba568);
    iVar3 = FUN_00474d60(0x5ba568);
    puVar4 = (undefined4 *)(param_1 + 0x76f8 + iVar2 * 0x90 + (iVar3 + -1) * 0x10);
    local_2c = (undefined4)uVar6;
    local_28 = (undefined4)((ulonglong)uVar6 >> 0x20);
    *puVar4 = local_2c;
    puVar4[1] = local_28;
  }
  return;
}

