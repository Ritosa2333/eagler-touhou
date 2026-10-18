/* Entry: 0x00507a80; symbol: FUN_00507a80; body bytes: 68 */

void __fastcall FUN_00507a80(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00411ce0(param_1 + 0x3d868);
  *unaff_FS_OFFSET = local_10;
  return;
}

