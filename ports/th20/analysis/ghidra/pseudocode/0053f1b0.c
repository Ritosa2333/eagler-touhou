/* Entry: 0x0053f1b0; symbol: FUN_0053f1b0; body bytes: 156 */

void __fastcall
FUN_0053f1b0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  p_Var3 = param_1 + 8;
  FUN_00414b60();
  ppcVar1 = (char **)move<>(*(int *)p_Var3 + -0x38);
  uVar2 = FUN_0040f160(param_1);
  FUN_004ae9d0(uVar2,ppcVar1);
  *(int *)p_Var3 = *(int *)p_Var3 + -0x38;
  *unaff_FS_OFFSET = local_10;
  return;
}

