/* Entry: 0x004a9c70; symbol: FUN_004a9c70; body bytes: 158 */

void __fastcall
FUN_004a9c70(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  void *this;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  
  this = (void *)FUN_0040f160(param_1);
  p_Var1 = param_1 + 4;
  FUN_0040e5e0();
  if (*(int *)p_Var1 != 0) {
    FUN_004a18a0(*(int *)p_Var1,*(int *)(param_1 + 8),this);
    FUN_004aa8d0(this,*(int *)p_Var1,(*(int *)(param_1 + 0xc) - *(int *)p_Var1) / 0x1c);
    *(int *)p_Var1 = 0;
    *(int *)(param_1 + 8) = 0;
    *(int *)(param_1 + 0xc) = 0;
  }
  return;
}

