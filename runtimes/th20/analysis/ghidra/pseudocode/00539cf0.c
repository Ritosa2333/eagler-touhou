/* Entry: 0x00539cf0; symbol: FUN_00539cf0; body bytes: 95 */

void __fastcall FUN_00539cf0(int *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if ((*param_1 != 0) && (*param_1 != 0)) {
    FUN_0041f670((LPVOID)*param_1);
    *param_1 = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

