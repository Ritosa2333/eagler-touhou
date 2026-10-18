/* Entry: 0x0050e990; symbol: FUN_0050e990; body bytes: 299 */

void __fastcall FUN_0050e990(int param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0050f660(param_1);
  FUN_0040bcf0((undefined4 *)(param_1 + 0x1242c4));
  if ((*(int *)(param_1 + 4) != 0) && (*(int *)(param_1 + 4) != 0)) {
    FUN_0041f670(*(LPVOID *)(param_1 + 4));
    *(undefined4 *)(param_1 + 4) = 0;
  }
  if ((*(int *)(param_1 + 8) != 0) && (*(int *)(param_1 + 8) != 0)) {
    FUN_0041f670(*(LPVOID *)(param_1 + 8));
    *(undefined4 *)(param_1 + 8) = 0;
  }
  if ((*(int *)(param_1 + 0x92144) != 0) && (*(int *)(param_1 + 0x92144) != 0)) {
    FUN_0041f670(*(LPVOID *)(param_1 + 0x92144));
    *(undefined4 *)(param_1 + 0x92144) = 0;
  }
  if ((*(int *)(param_1 + 0x92148) != 0) && (*(int *)(param_1 + 0x92148) != 0)) {
    FUN_0041f670(*(LPVOID *)(param_1 + 0x92148));
    *(undefined4 *)(param_1 + 0x92148) = 0;
  }
  FUN_0040b980((undefined4 *)(param_1 + 0x1242c4));
  *unaff_FS_OFFSET = local_10;
  return;
}

