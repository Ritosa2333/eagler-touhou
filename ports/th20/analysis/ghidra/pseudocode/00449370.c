/* Entry: 0x00449370; symbol: FUN_00449370; body bytes: 62 */

void __fastcall FUN_00449370(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0044c240(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

