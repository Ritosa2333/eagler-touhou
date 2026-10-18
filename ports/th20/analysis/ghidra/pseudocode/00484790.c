/* Entry: 0x00484790; symbol: FUN_00484790; body bytes: 158 */

void __fastcall
FUN_00484790(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  void *this;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  
  this = (void *)FUN_0040f160(param_1);
  p_Var1 = param_1 + 4;
  FUN_0040e5e0();
  if (*(int *)p_Var1 != 0) {
    std::_Adl_verify_range<char*,char_const*>(*(char ***)p_Var1,*(char ***)(param_1 + 8));
    FUN_004852b0(this,*(int *)p_Var1,(*(int *)(param_1 + 0xc) - *(int *)p_Var1) / 0x2c);
    *(int *)p_Var1 = 0;
    *(undefined4 *)(param_1 + 8) = 0;
    *(int *)(param_1 + 0xc) = 0;
  }
  return;
}

