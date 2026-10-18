/* Entry: 0x00497f40; symbol: FUN_00497f40; body bytes: 116 */

void __fastcall
FUN_00497f40(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  
  p_Var1 = param_1 + 4;
  p_Var2 = param_1 + 8;
  if (*(int *)p_Var1 != *(int *)p_Var2) {
    FUN_0040e5e0();
    FUN_0040f160(param_1);
    std::_Adl_verify_range<char*,char_const*>(*(char ***)p_Var1,*(char ***)p_Var2);
    *(int *)p_Var2 = *(int *)p_Var1;
  }
  return;
}

