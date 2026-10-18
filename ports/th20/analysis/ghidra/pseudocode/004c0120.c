/* Entry: 0x004c0120; symbol: FUN_004c0120; body bytes: 230 */

void __fastcall FUN_004c0120(undefined4 *param_1)

{
  bool bVar1;
  int *piVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  undefined4 *puVar4;
  int *unaff_FS_OFFSET;
  int local_1c [2];
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005699d0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_00570c88;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown HitCtrlInf\n",local_14);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_0040c080(local_1c,8);
  FUN_00412280(param_1 + 0x3105,local_1c);
  piVar2 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar2);
    if (!bVar1) break;
    p_Var3 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    puVar4 = (undefined4 *)FUN_0040f160(p_Var3);
    FUN_004c1f30((char **)*puVar4);
    FUN_00411c30(local_1c);
  }
  FUN_00411b00(local_1c);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

