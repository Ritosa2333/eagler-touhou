/* Entry: 0x004aee90; symbol: FUN_004aee90; body bytes: 80 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__fastcall
FUN_004aee90(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  default_delete<class_std::_Facet_base> *this;
  allocator<struct_std::_Container_proxy> *paVar1;
  undefined1 local_6 [2];
  
  _Compressed_pair<><>(param_1);
  this = std::
         _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
         ::_Get_first(param_1);
  FUN_0040f160(param_1);
  paVar1 = (allocator<struct_std::_Container_proxy> *)FUN_0040b120(local_6);
  std::_Container_base12::_Alloc_proxy<class_std::allocator<struct_std::_Container_proxy>_>
            ((_Container_base12 *)this,paVar1);
  return param_1;
}

