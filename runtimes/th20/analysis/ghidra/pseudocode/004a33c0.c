/* Entry: 0x004a33c0; symbol: FUN_004a33c0; body bytes: 210 */

undefined4 * __fastcall FUN_004a33c0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004a35f0(param_1);
  *param_1 = &PTR_FUN_005703cc;
  param_1[0x1c] = 0;
  FUN_00411970(param_1 + 0x1d,param_1);
  FUN_004a3060(param_1 + 0x22);
  FUN_0047bb30(param_1 + 0xde);
  FUN_00418a50(param_1 + 0xf3);
  FUN_00418a10(param_1 + 0xf9);
  function<>(param_1 + 0xfe);
  param_1[0x108] = 0;
  param_1[0x109] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

