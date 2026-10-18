/* Entry: 0x004715c0; symbol: FUN_004715c0; body bytes: 447 */

undefined4 * __fastcall FUN_004715c0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00422d90(param_1);
  param_1[4] = 0;
  *(undefined1 *)(param_1 + 5) = 0;
  FUN_00422d90(param_1 + 6);
  FUN_00422d90(param_1 + 10);
  FUN_00447b30(param_1 + 0xe);
  FUN_00447b30(param_1 + 0x23);
  FUN_00447b30(param_1 + 0x38);
  FUN_00471410(param_1 + 0x4d);
  FUN_004479e0(param_1 + 0x76);
  FUN_00471790(param_1 + 0x81);
  param_1[0xdc] = 0;
  _eh_vector_constructor_iterator_(param_1 + 0xdd,0x5e4,8,FUN_00448b40,FUN_00449370);
  param_1[0xca5] = 0;
  param_1[0xca6] = 0;
  param_1[0xca7] = 0;
  param_1[0xca8] = 0;
  param_1[0xca9] = 0;
  param_1[0xcaa] = 0;
  param_1[0xcab] = 0;
  param_1[0xcac] = 0;
  param_1[0xcad] = 0;
  param_1[0xcae] = 0;
  param_1[0xcaf] = 0;
  param_1[0xcb0] = 0;
  param_1[0xcb1] = 0;
  param_1[0xcb2] = 0;
  param_1[0xcb3] = 0;
  param_1[0xcb4] = 0;
  param_1[0xcb5] = 0;
  FUN_0040bc20(param_1 + 0xcb6,0x10,2,FUN_00422d90);
  FUN_0040bc20(param_1 + 0xcbe,4,2,FUN_00447de0);
  FUN_0040bc20(param_1 + 0xcc0,4,2,FUN_00447de0);
  param_1[0xcc2] = 0;
  FUN_00425cc0(param_1 + 0xcc3);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

