/* Entry: 0x004f52d0; symbol: FUN_004f52d0; body bytes: 65 */

void __fastcall FUN_004f52d0(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0044fcd0((undefined4 *)(param_1 + 0x4c));
  *unaff_FS_OFFSET = local_10;
  return;
}

