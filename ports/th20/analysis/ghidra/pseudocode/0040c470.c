/* Entry: 0x0040c470; symbol: FUN_0040c470; body bytes: 63 */

void __fastcall
FUN_0040c470(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int iVar3;
  
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(param_1);
  iVar3 = FUN_00540a7c((uint *)pdVar2);
  if (iVar3 != 0) {
    std::_Throw_Cpp_error(5);
  }
  bVar1 = FUN_0040c050((int)param_1);
  if (!bVar1) {
    std::_Throw_Cpp_error(6);
  }
  return;
}

