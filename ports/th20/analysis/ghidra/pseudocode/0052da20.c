/* Entry: 0x0052da20; symbol: FUN_0052da20; body bytes: 80 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__fastcall
FUN_0052da20(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  undefined4 *puVar1;
  undefined1 local_1c [4];
  _Container_base12 *local_18;
  allocator<struct_std::_Container_proxy> *local_14;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_10;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_c;
  
  local_10 = param_1;
  local_c = param_1;
  FUN_0052d040((undefined4 *)param_1);
  local_18 = (_Container_base12 *)FUN_004fbff0((int)local_c);
  puVar1 = (undefined4 *)FUN_0040f160(local_c);
  local_14 = (allocator<struct_std::_Container_proxy> *)FUN_0040b670(local_1c,puVar1);
  std::_Container_base12::_Alloc_proxy<class_std::allocator<struct_std::_Container_proxy>_>
            (local_18,local_14);
  return local_c;
}

