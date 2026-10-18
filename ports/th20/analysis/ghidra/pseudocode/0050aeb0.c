/* Entry: 0x0050aeb0; symbol: FUN_0050aeb0; body bytes: 99 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__cdecl FUN_0050aeb0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     *param_1,undefined4 param_2)

{
  void *this;
  undefined4 uVar1;
  
  this = operator_new(0xc);
  if (this != (void *)0x0) {
    uVar1 = move<>(param_2);
    FUN_0050a9f0(this,uVar1);
  }
  FID_conflict__Unique_ptr_base<class_std::_Facet_base,struct_std::default_delete<class_std::_Facet_base>_><class_std::_Facet_base*>
            (param_1);
  return param_1;
}

