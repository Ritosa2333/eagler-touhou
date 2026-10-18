/* Entry: 0x00507860; symbol: FUN_00507860; body bytes: 72 */

void __fastcall FUN_00507860(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00542f23(param_1,0x2c,8,FUN_00507a30);
  *unaff_FS_OFFSET = local_10;
  return;
}

