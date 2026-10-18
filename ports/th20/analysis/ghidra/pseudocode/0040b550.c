/* Entry: 0x0040b550; symbol: FUN_0040b550; body bytes: 118 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__cdecl FUN_0040b550(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *param_1,undefined4 param_2,undefined4 param_3)

{
  void *this;
  undefined4 uVar1;
  undefined4 uVar2;
  
  this = operator_new(8);
  if (this != (void *)0x0) {
    uVar1 = move<>(param_3);
    uVar2 = move<>(param_2);
    FUN_0040af40(this,uVar2,uVar1);
  }
  FID_conflict__Unique_ptr_base<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_><class_std::_Facet_base*>
            (param_1);
  return param_1;
}

