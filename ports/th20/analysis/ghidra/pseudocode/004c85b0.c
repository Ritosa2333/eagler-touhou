/* Entry: 0x004c85b0; symbol: FUN_004c85b0; body bytes: 522 */

undefined4 * __fastcall FUN_004c85b0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005710bc;
  FUN_00418a10(param_1 + 2);
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  FUN_00422d90(param_1 + 10);
  FUN_00422d90(param_1 + 0xe);
  FUN_00422d90(param_1 + 0x12);
  FUN_00422e10(param_1 + 0x16);
  FUN_00422e10(param_1 + 0x19);
  param_1[0x1c] = 0;
  param_1[0x1d] = 0;
  param_1[0x1e] = 0;
  param_1[0x1f] = 0;
  param_1[0x20] = 0;
  param_1[0x21] = 0;
  param_1[0x22] = 0;
  param_1[0x23] = 0;
  FUN_004c81f0(param_1 + 0x24);
  param_1[0x1a4] = 0;
  param_1[0x1a6] = 0;
  param_1[0x1a7] = 0;
  param_1[0x1a8] = 0;
  param_1[0x1a9] = 0;
  param_1[0x1aa] = 0;
  FUN_00422d90(param_1 + 0x1ab);
  FUN_00422d90(param_1 + 0x1af);
  param_1[0x1b3] = 0;
  param_1[0x1b4] = 0;
  param_1[0x1b5] = 0;
  param_1[0x1b6] = 0;
  param_1[0x1b7] = 0;
  param_1[0x1b8] = 0;
  param_1[0x1b9] = 0;
  param_1[0x1ba] = 0xffffffff;
  param_1[0x1bb] = 0;
  param_1[0x1bc] = 0;
  _memset(param_1,0,0x6f8);
  FUN_00423520(param_1 + 10,0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

