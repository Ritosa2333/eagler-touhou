/* Entry: 0x004a0440; symbol: FUN_004a0440; body bytes: 324 */

undefined4 __fastcall FUN_004a0440(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  uint uVar2;
  uint local_10;
  uint local_c;
  
  bVar1 = FUN_004a0590(*(uint **)(param_1 + 0x18));
  if (CONCAT31(extraout_var,bVar1) != 0) {
    if ((*(uint *)(param_1 + 0x20) >> 2 & 1) == 0) {
      *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 8;
      FUN_004a0fb0(&DAT_005c4d40,0x10);
      return 1;
    }
    *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 8;
  }
  *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x24) + 1;
  uVar2 = FUN_004a0ed0(*(int *)(param_1 + 0x18));
  if (((uVar2 == 0) && (uVar2 = FUN_004a0eb0(param_1), uVar2 == 0)) &&
     (uVar2 = FUN_004a0ef0(*(int *)(param_1 + 0x18)), uVar2 != 0)) {
    if (DAT_005b88a4 == (void *)0x0) {
      local_c = 0;
    }
    else {
      local_c = FUN_004495a0(DAT_005b88a4,0x200);
    }
    if (local_c == 0) {
      if (DAT_005b88a4 == (void *)0x0) {
        local_10 = 0;
      }
      else {
        local_10 = FUN_004a0a20(DAT_005b88a4,0);
      }
      if (local_10 < 0x14) {
        return 1;
      }
    }
    if (*(int *)(param_1 + 0x24) % 0xc != 0) {
      return 6;
    }
  }
  return 1;
}

