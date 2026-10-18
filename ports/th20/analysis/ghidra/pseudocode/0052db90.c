/* Entry: 0x0052db90; symbol: FUN_0052db90; body bytes: 110 */

void __fastcall
FUN_0052db90(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  undefined4 *puVar1;
  _Facet_base **pp_Var2;
  _Facet_base *p_Var3;
  _Facet_base **pp_Var4;
  _Facet_base *local_14;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_10;
  allocator<struct_std::_Container_proxy> local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  FUN_0052ee70(param_1);
  FUN_0040c080(local_c,4);
  puVar1 = (undefined4 *)FUN_0040f160(local_10);
  FUN_0040b670(local_c,puVar1);
  local_14 = (_Facet_base *)0x0;
  pp_Var4 = &local_14;
  pp_Var2 = (_Facet_base **)FUN_004fbff0((int)local_10);
  p_Var3 = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>(pp_Var2,pp_Var4);
  std::_Delete_plain_internal<class_std::allocator<struct_std::_Container_proxy>_>
            (local_c,(_Container_proxy *)p_Var3);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

