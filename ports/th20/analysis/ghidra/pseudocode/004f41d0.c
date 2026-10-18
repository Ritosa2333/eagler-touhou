/* Entry: 0x004f41d0; symbol: FUN_004f41d0; body bytes: 81 */

undefined4 __fastcall FUN_004f41d0(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,0x124,0x100,FUN_004f4cb0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

