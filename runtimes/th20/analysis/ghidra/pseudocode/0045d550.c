/* Entry: 0x0045d550; symbol: FUN_0045d550; body bytes: 78 */

undefined4 __fastcall FUN_0045d550(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,0x3c0,1,FUN_0045d610);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

