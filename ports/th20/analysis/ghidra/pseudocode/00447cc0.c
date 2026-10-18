/* Entry: 0x00447cc0; symbol: FUN_00447cc0; body bytes: 87 */

undefined4 * __fastcall FUN_00447cc0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00446c10(param_1);
  FUN_0044b2b0((int)param_1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

