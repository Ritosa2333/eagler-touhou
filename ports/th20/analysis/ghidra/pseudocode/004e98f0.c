/* Entry: 0x004e98f0; symbol: FUN_004e98f0; body bytes: 179 */

void __cdecl FUN_004e98f0(undefined4 *param_1,undefined4 param_2,uint param_3)

{
  char *pcVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  default_delete<class_std::_Facet_base> *pdVar3;
  undefined4 *puVar4;
  uint uVar5;
  uint uVar6;
  undefined1 *puVar7;
  undefined4 local_34;
  undefined4 local_30;
  undefined8 local_2c;
  undefined1 local_22;
  undefined1 local_21;
  char local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar6 = 0x10;
  uVar5 = param_3;
  pcVar1 = (char *)FUN_004f3310((int)local_20);
  local_2c = FUN_004710c0(local_20,pcVar1,uVar5,uVar6);
  local_21 = 0x30;
  puVar7 = &local_21;
  p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_004f57f0(&param_2,&local_30);
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(p_Var2);
  FUN_004f5650(pdVar3,puVar7);
  local_22 = 0x78;
  puVar7 = &local_22;
  p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_004f57f0(&param_2,&local_34);
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first(p_Var2);
  FUN_004f5650(pdVar3,puVar7);
  puVar4 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,(undefined1 *)local_2c,*puVar4);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

