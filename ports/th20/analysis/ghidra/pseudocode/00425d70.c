/* Entry: 0x00425d70; symbol: FUN_00425d70; body bytes: 573 */

undefined4 * __fastcall FUN_00425d70(undefined4 *param_1)

{
  void *this;
  int *unaff_FS_OFFSET;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  FUN_00425bb0(param_1 + 7);
  FUN_004141c0(param_1 + 0x61f);
  param_1[0x623] = 0;
  param_1[0x624] = 0;
  _memset(param_1 + 0x625,0,0x100);
  FUN_00425c70(param_1 + 0x665);
  param_1[0x881] = 0;
  _memset(param_1 + 0x882,0,0x11c);
  _memset(param_1 + 0x8c9,0,0x120);
  _memset(param_1 + 0x911,0,0x100);
  FUN_00425c10(param_1 + 0x951);
  _memset(param_1 + 0x11b1,0,0x100);
  _memset(param_1 + 0x11f1,0,0xf00);
  _memset(param_1 + 0x15b1,0,0x100);
  param_1[0x15f1] = 0;
  param_1[0x15f2] = 0;
  param_1[0x15f3] = 0;
  param_1[0x15f4] = 0;
  param_1[0x15f5] = 0;
  param_1[0x15f6] = 0;
  param_1[0x15f7] = 0;
  param_1[0x15f8] = 0;
  param_1[0x15f9] = 0;
  for (local_18 = 0; local_18 < 0x5a; local_18 = local_18 + 1) {
    this = (void *)FUN_004260f0(param_1 + 0x665,local_18);
    FUN_00426390(this,local_18);
  }
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

