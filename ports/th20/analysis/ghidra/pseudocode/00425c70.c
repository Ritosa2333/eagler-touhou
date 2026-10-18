/* Entry: 0x00425c70; symbol: FUN_00425c70; body bytes: 75 */

undefined4 __fastcall FUN_00425c70(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,0x18,0x5a,FUN_00425d20);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

