/* Entry: 0x0048b2f0; symbol: FUN_0048b2f0; body bytes: 127 */

void * FUN_0048b2f0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *param_1)

{
  undefined4 local_20;
  uint local_1c;
  void *local_18;
  undefined4 *local_14;
  void *local_10;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_c;
  byte local_6;
  byte local_5;
  
  FUN_0040f160(param_1);
  local_14 = FUN_00470a40(&local_20);
  local_5 = local_6;
  FUN_00489070(local_18,(uint)local_6,local_14);
  local_c = param_1 + 4;
  local_1c = (*(int *)(param_1 + 8) - *(int *)local_c) / 0x2c;
  FUN_00489730(local_10,local_1c,local_c,param_1 + 8);
  return local_10;
}

