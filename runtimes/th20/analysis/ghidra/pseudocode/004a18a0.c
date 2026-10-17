/* Entry: 0x004a18a0; symbol: FUN_004a18a0; body bytes: 95 */

void __cdecl FUN_004a18a0(int param_1,int param_2,undefined4 param_3)

{
  void *pvVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005676c0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  for (; param_1 != param_2; param_1 = param_1 + 0x1c) {
    pvVar1 = (void *)move<>(param_1);
    FUN_004a2980(param_3,pvVar1);
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

