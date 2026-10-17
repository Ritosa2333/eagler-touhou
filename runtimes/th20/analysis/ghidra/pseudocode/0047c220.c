/* Entry: 0x0047c220; symbol: FUN_0047c220; body bytes: 90 */

void __fastcall FUN_0047c220(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (*(int *)(param_1 + 4) != 0) {
    FUN_00485240(*(LPVOID *)(param_1 + 4));
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

