/* Entry: 0x004a14d0; symbol: FUN_004a14d0; body bytes: 120 */

undefined4 * FUN_004a14d0(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  undefined1 *puVar5;
  undefined1 local_c [4];
  undefined4 *local_8;
  
  puVar1 = (undefined4 *)move<>(param_1);
  puVar1 = (undefined4 *)FUN_0040b2a0(puVar1);
  *local_8 = *puVar1;
  puVar1 = local_8 + 1;
  puVar2 = (undefined4 *)move<>(param_2);
  puVar2 = (undefined4 *)FUN_0040b2a0(puVar2);
  FUN_0040b670(local_c,puVar2);
  puVar5 = local_c;
  iVar3 = move<>(param_2);
  p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)FUN_004a21d0(iVar3);
  FUN_004a2c50(puVar1,p_Var4,puVar5);
  return local_8;
}

