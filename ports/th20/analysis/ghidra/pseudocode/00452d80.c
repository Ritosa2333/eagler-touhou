/* Entry: 0x00452d80; symbol: FUN_00452d80; body bytes: 75 */

undefined4 __fastcall FUN_00452d80(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,0x30,0x16,FUN_00452ee0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

