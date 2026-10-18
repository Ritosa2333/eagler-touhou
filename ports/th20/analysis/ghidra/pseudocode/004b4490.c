/* Entry: 0x004b4490; symbol: FUN_004b4490; body bytes: 666 */

void __fastcall FUN_004b4490(int param_1)

{
  bool bVar1;
  undefined4 uVar2;
  uint uVar3;
  uint uVar4;
  undefined8 uVar5;
  uint local_38;
  int local_34;
  uint local_28;
  int local_24;
  undefined8 local_20;
  
  uVar5 = FUN_004b8290(0x5ba568);
  local_24 = (int)((ulonglong)uVar5 >> 0x20);
  if ((local_24 == 0) && (local_28 = (uint)uVar5, local_28 < *(uint *)(param_1 + 0x160))) {
    uVar5 = FUN_004b8290(0x5ba568);
    *(undefined8 *)(param_1 + 0x160) = uVar5;
  }
  uVar5 = FUN_004b8290(0x5ba568);
  local_34 = (int)((ulonglong)uVar5 >> 0x20);
  if ((local_34 != 0) || (local_38 = (uint)uVar5, *(uint *)(param_1 + 0x160) <= local_38)) {
    uVar5 = FUN_004b8290(0x5ba568);
    uVar5 = __aulldiv((uint)uVar5 - *(uint *)(param_1 + 0x160),
                      ((int)((ulonglong)uVar5 >> 0x20) - *(int *)(param_1 + 0x164)) -
                      (uint)((uint)uVar5 < *(uint *)(param_1 + 0x160)),0x20,0);
    local_20._4_4_ = (uint)((ulonglong)uVar5 >> 0x20);
    if ((local_20._4_4_ == 0) && (local_20._0_4_ = (uint)uVar5, (uint)local_20 < 0x8d55e)) {
      bVar1 = (uint)local_20 == 0;
      local_20 = uVar5;
      if (bVar1) {
        local_20 = 1;
      }
    }
    else {
      local_20 = 0x8d55e;
    }
    if ((*(uint *)(param_1 + 0x16c) <= local_20._4_4_) &&
       ((*(uint *)(param_1 + 0x16c) < local_20._4_4_ ||
        (*(uint *)(param_1 + 0x168) < (uint)local_20)))) {
      *(uint *)(param_1 + 0x168) = (uint)local_20;
      *(uint *)(param_1 + 0x16c) = local_20._4_4_;
    }
    uVar5 = FUN_004b8290(0x5ba568);
    uVar4 = (uint)uVar5;
    uVar3 = (((int)uVar4 >> 0x1f) - *(int *)(param_1 + 0x164)) -
            (uint)(uVar4 < *(uint *)(param_1 + 0x160));
    if ((uVar3 <= *(uint *)(param_1 + 0x16c)) &&
       ((uVar3 < *(uint *)(param_1 + 0x16c) ||
        (uVar4 - *(uint *)(param_1 + 0x160) < *(uint *)(param_1 + 0x168))))) {
      uVar5 = FUN_004b8290(0x5ba568);
      uVar4 = (uint)uVar5;
      *(uint *)(param_1 + 0x168) = uVar4 - *(uint *)(param_1 + 0x160);
      *(uint *)(param_1 + 0x16c) =
           (((int)uVar4 >> 0x1f) - *(int *)(param_1 + 0x164)) -
           (uint)(uVar4 < *(uint *)(param_1 + 0x160));
    }
    uVar4 = *(uint *)(param_1 + 0x160);
    *(uint *)(param_1 + 0x160) = uVar4 + *(uint *)(param_1 + 0x168);
    *(uint *)(param_1 + 0x164) =
         *(int *)(param_1 + 0x164) + *(int *)(param_1 + 0x16c) +
         (uint)CARRY4(uVar4,*(uint *)(param_1 + 0x168));
    uVar5 = FUN_004b8290(0x5ba568);
    uVar4 = (int)(uint)uVar5 >> 0x1f;
    if ((uVar4 <= *(uint *)(param_1 + 0x164)) &&
       ((uVar4 < *(uint *)(param_1 + 0x164) || ((uint)uVar5 <= *(uint *)(param_1 + 0x160))))) {
      *(undefined4 *)(param_1 + 0x168) = 0;
      *(undefined4 *)(param_1 + 0x16c) = 0;
    }
  }
  uVar5 = FUN_004b8030(0x5ba568);
  uVar4 = (int)(uint)uVar5 >> 0x1f;
  if ((uVar4 <= *(uint *)(param_1 + 0x164)) &&
     ((uVar4 < *(uint *)(param_1 + 0x164) || ((uint)uVar5 < *(uint *)(param_1 + 0x160))))) {
    FUN_004b8960(&DAT_005ba568,*(undefined4 *)(param_1 + 0x160),*(undefined4 *)(param_1 + 0x164));
    uVar2 = FUN_004b7fa0(0x5ba568);
    FUN_004b8940(&DAT_005ba568,uVar2);
    FUN_004b8980(0x5ba568);
    uVar4 = FUN_004b8410(0x5ba568);
    if (uVar4 == 0) {
      FUN_004b90e0(DAT_005c06a4,3,0);
    }
  }
  return;
}

