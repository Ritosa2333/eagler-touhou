/* Entry: 0x004af9e0; symbol: FUN_004af9e0; body bytes: 100 */

void __fastcall
FUN_004af9e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  default_delete<class_std::_Facet_base> *pdVar1;
  _Facet_base *p_Var2;
  _Facet_base **pp_Var3;
  _Facet_base *local_14;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_10;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  FUN_004b7b80(param_1);
  FUN_0040f160(local_10);
  FUN_0040b120(&local_9);
  local_14 = (_Facet_base *)0x0;
  pp_Var3 = &local_14;
  pdVar1 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(local_10);
  p_Var2 = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>
                     ((_Facet_base **)pdVar1,pp_Var3);
  FUN_004ad810(&local_9,p_Var2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

