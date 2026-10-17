/* Entry: 0x004491e0; symbol: FUN_004491e0; body bytes: 78 */

void __fastcall FUN_004491e0(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00542f23(param_1,0x600,0x10000,FUN_004492f0);
  *unaff_FS_OFFSET = local_10;
  return;
}

