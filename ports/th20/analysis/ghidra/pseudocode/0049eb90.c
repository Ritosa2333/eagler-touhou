/* Entry: 0x0049eb90; symbol: FUN_0049eb90; body bytes: 75 */

undefined4 __fastcall FUN_0049eb90(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,4,5,FUN_00425cc0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

