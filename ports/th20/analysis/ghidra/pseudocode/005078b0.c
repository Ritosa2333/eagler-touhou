/* Entry: 0x005078b0; symbol: FUN_005078b0; body bytes: 376 */

void __fastcall FUN_005078b0(undefined4 *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  int local_1c;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005732e4;
  FUN_00413b70(DAT_005b8894,(char **)param_1[6]);
  for (local_1c = 0; local_1c < 8; local_1c = local_1c + 1) {
    FUN_00509fa0(param_1,local_1c);
  }
  FUN_00413b70(DAT_005b8894,(char **)param_1[7]);
  param_1[7] = 0;
  for (local_18 = 0; local_18 < 8; local_18 = local_18 + 1) {
    piVar1 = (int *)FUN_00414580(param_1 + 8,local_18);
    FUN_00506ea0(DAT_005b8894,piVar1);
    puVar2 = (undefined4 *)FUN_00414580(param_1 + 8,local_18);
    *puVar2 = 0;
  }
  if (param_1[2] != 0) {
    FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  }
  if (param_1[0x95] != 0) {
    FUN_004124b0(DAT_005b66d8,(void *)param_1[0x95]);
  }
  if (param_1[3] != 0) {
    FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  }
  if (param_1 == DAT_005c60fc) {
    DAT_005c60fc = (undefined4 *)0x0;
  }
  FUN_00507860(param_1 + 0x3a);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

