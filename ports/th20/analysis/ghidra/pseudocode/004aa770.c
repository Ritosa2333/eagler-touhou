/* Entry: 0x004aa770; symbol: FUN_004aa770; body bytes: 116 */

void __fastcall
FUN_004aa770(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  undefined4 uVar2;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var3;
  
  p_Var1 = param_1 + 4;
  p_Var3 = param_1 + 8;
  if (*(int *)p_Var1 != *(int *)p_Var3) {
    FUN_0040e5e0();
    uVar2 = FUN_0040f160(param_1);
    FUN_004a18a0(*(int *)p_Var1,*(int *)p_Var3,uVar2);
    *(int *)p_Var3 = *(int *)p_Var1;
  }
  return;
}

