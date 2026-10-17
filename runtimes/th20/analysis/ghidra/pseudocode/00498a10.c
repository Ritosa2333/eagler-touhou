/* Entry: 0x00498a10; symbol: FUN_00498a10; body bytes: 64 */

void __fastcall FUN_00498a10(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004292e0((float *)(param_1 + 0x28));
  *unaff_FS_OFFSET = local_10;
  return;
}

