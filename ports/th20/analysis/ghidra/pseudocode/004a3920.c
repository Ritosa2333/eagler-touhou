/* Entry: 0x004a3920; symbol: FUN_004a3920; body bytes: 399 */

void __fastcall FUN_004a3920(undefined4 *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  uint local_44;
  LPVOID local_3c [8];
  int local_1c;
  int local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00569710;
  local_10 = *unaff_FS_OFFSET;
  local_14 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005703f8;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown EnemyCtrlInf\n",local_14);
  FUN_004a80f0();
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0040c080(&local_18,4);
  FUN_004a9d10(&DAT_005c49f4,&local_18);
  FUN_0040c080(&local_1c,4);
  FUN_004a9d40(&DAT_005c49f4,&local_1c);
  while( true ) {
    bVar1 = FUN_00449400(&local_18,&local_1c);
    if (bVar1) break;
    puVar2 = (undefined4 *)FUN_004a3ec0(&local_18);
    FUN_0046a620(local_3c,puVar2);
    if (local_3c[0] != (LPVOID)0x0) {
      FUN_0041f670(local_3c[0]);
      local_3c[0] = (LPVOID)0x0;
    }
    FUN_0046ade0((int)local_3c);
    FUN_0048be00(&local_18);
  }
  FUN_004aa700((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&DAT_005c49f4);
  FUN_004a2790((int *)param_1[0x41]);
  param_1[0x41] = 0;
  param_1[0x41] = 0;
  for (local_44 = 0; local_44 < 8; local_44 = local_44 + 1) {
    FUN_0044c430(DAT_005c0028,local_44 + 0x19);
  }
  FUN_004a3880((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x2d));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

