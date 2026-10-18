/* Entry: 0x004a38a0; symbol: FUN_004a38a0; body bytes: 79 */

void __fastcall FUN_004a38a0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_004a2800((void *)*param_1);
  *param_1 = 0;
  *unaff_FS_OFFSET = local_10;
  return;
}

