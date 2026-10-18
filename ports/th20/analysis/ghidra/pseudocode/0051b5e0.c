/* Entry: 0x0051b5e0; symbol: FUN_0051b5e0; body bytes: 160 */

undefined4 * FUN_0051b5e0(undefined4 *param_1,char *param_2,char *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  undefined1 local_20 [4];
  void *local_1c;
  uint local_18;
  void *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>(&param_2,&param_3);
  iVar1 = move<>(param_2);
  puVar2 = Myptr((undefined4 *)((int)local_14 + 4));
  local_18 = iVar1 - (int)puVar2;
  FUN_0051a6c0(local_14,local_18,(int)param_3 - (int)param_2);
  local_1c = FUN_004fed00(local_14,local_20);
  FUN_00516490(local_1c,param_1,local_18);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

