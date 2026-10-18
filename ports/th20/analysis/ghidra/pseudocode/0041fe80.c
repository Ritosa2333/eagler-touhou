/* Entry: 0x0041fe80; symbol: FUN_0041fe80; body bytes: 762 */

void __fastcall FUN_0041fe80(void *param_1)

{
  undefined4 uVar1;
  uint *puVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  int local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005680cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,0xf);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  for (local_24 = 0; local_24 < 4; local_24 = local_24 + 1) {
    puVar4 = &DAT_005b88b0 + local_24 * 0xb0;
    puVar5 = (undefined4 *)(&DAT_005b93b0 + local_24 * 0x2c0);
    for (iVar3 = 0xb0; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
  }
  for (local_2c = 0; local_2c < *(int *)((int)param_1 + 0x14); local_2c = local_2c + 1) {
    FUN_00421b00((int *)((int)param_1 + local_2c * 0x3d4 + 0x20));
  }
  for (local_20 = 0; local_20 < 4; local_20 = local_20 + 1) {
    if ((&DAT_005b8b6c)[local_20 * 0xb0] == 0) {
      *(uint *)(&DAT_005b88b4 + local_20 * 0x2c0) = (&DAT_005b88b0)[local_20 * 0xb0];
    }
    else {
      *(uint *)(&DAT_005b88b4 + local_20 * 0x2c0) = (&DAT_005b88b0)[local_20 * 0xb0] & 0x3ef0300;
    }
  }
  if (DAT_005b8b6c == 0) {
    puVar2 = (uint *)FUN_00421950(param_1,*(int *)((int)param_1 + 0x2e24));
    DAT_005b88b0 = *puVar2;
  }
  else {
    puVar2 = (uint *)FUN_00421950(param_1,*(int *)((int)param_1 + 0x2e24));
    DAT_005b88b0 = *puVar2 & 0x3ef0300;
  }
  DAT_005b90f0 = 0;
  for (local_28 = 0; local_28 < *(int *)((int)param_1 + 0x14); local_28 = local_28 + 1) {
    if (0 < local_28) {
      puVar2 = (uint *)FUN_00421950(param_1,local_28);
      DAT_005b90f0 = DAT_005b90f0 | *puVar2;
    }
  }
  DAT_005b93a8 = *(undefined4 *)((int)param_1 + 0x18);
  *(undefined4 *)((int)param_1 + 0x2e2c) = *(undefined4 *)((int)param_1 + 0x18);
  puVar2 = (uint *)FUN_00421950(param_1,0);
  DAT_005b8e30 = *puVar2 | DAT_005b90f0;
  puVar2 = (uint *)FUN_00421950(param_1,0);
  DAT_005b88b0 = *puVar2 | DAT_005b90f0;
  for (local_30 = 0; local_30 < 4; local_30 = local_30 + 1) {
    FUN_004228b0(&DAT_005b88b0 + local_30 * 0xb0);
  }
  *(int *)((int)param_1 + 0x10) = *(int *)((int)param_1 + 0x10) + 1;
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

