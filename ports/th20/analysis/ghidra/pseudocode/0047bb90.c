/* Entry: 0x0047bb90; symbol: FUN_0047bb90; body bytes: 255 */

undefined4 * __fastcall FUN_0047bb90(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  FUN_004141c0(param_1 + 1);
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  *(undefined2 *)(param_1 + 0xe) = 0;
  *(undefined2 *)((int)param_1 + 0x3a) = 0;
  param_1[0xf] = 0x15;
  param_1[0x10] = 0x26;
  param_1[0x11] = 0;
  *(undefined1 *)(param_1 + 0x12) = 0;
  *(undefined1 *)((int)param_1 + 0x49) = 0;
  FUN_004862f0(param_1 + 1,2);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

