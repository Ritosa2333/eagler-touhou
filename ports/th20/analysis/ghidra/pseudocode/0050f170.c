/* Entry: 0x0050f170; symbol: FUN_0050f170; body bytes: 379 */

int FUN_0050f170(void)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  bool bVar6;
  float10 fVar7;
  undefined8 uVar8;
  undefined4 local_58 [10];
  undefined8 local_30;
  uint *local_24;
  int local_20;
  int local_1c;
  int local_18;
  uint *local_14;
  
  iVar2 = FUN_004640a0(0x5ba568);
  local_14 = (uint *)(local_20 + 0x18 + iVar2 * 400);
  for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
    uVar8 = FUN_004b8290(0x5ba568);
    local_24 = local_14;
    local_30._4_4_ = (uint)((ulonglong)uVar8 >> 0x20);
    bVar6 = local_14[1] < local_30._4_4_;
    local_30 = uVar8;
    if ((local_14[1] <= local_30._4_4_) &&
       ((bVar6 || (local_30._0_4_ = (uint)uVar8, bVar6 = *local_14 <= (uint)local_30, bVar6))))
    break;
    local_14 = local_14 + 10;
  }
  if (local_18 < 10) {
    for (local_1c = 9; local_18 < local_1c; local_1c = local_1c + -1) {
      iVar2 = FUN_004640a0(0x5ba568);
      puVar4 = (undefined4 *)(local_20 + 0x18 + iVar2 * 400 + (local_1c + -1) * 0x28);
      puVar5 = local_58;
      for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
      iVar2 = FUN_004640a0(0x5ba568);
      puVar4 = local_58;
      puVar5 = (undefined4 *)(local_20 + 0x18 + iVar2 * 400 + local_1c * 0x28);
      for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar5 = *puVar4;
        puVar4 = puVar4 + 1;
        puVar5 = puVar5 + 1;
      }
    }
    uVar8 = FUN_004b8290(0x5ba568);
    *(undefined8 *)local_14 = uVar8;
    uVar1 = FUN_004b7fa0(0x5ba568);
    *(undefined1 *)((int)local_14 + 9) = uVar1;
    uVar1 = FUN_00474d60(0x5ba568);
    *(undefined1 *)(local_14 + 2) = uVar1;
    FUN_0050a990((longlong *)(local_14 + 6));
    FUN_00548610((char *)((int)local_14 + 10),10,0x5724ac);
    fVar7 = FUN_0050a1e0(DAT_005c4a00);
    local_14[8] = (uint)(float)fVar7;
  }
  else {
    local_18 = -1;
  }
  return local_18;
}

