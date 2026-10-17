/* Entry: 0x00413690; symbol: FUN_00413690; body bytes: 93 */

void __cdecl FUN_00413690(void *param_1,char **param_2)

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
  pvVar1 = (void *)move<>(param_2 + 2);
  FUN_00413ca0(param_1,pvVar1);
  FUN_00413620(param_1,param_2);
  *unaff_FS_OFFSET = local_10;
  return;
}

