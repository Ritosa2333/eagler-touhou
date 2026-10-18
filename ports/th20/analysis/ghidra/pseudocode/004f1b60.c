/* Entry: 0x004f1b60; symbol: FUN_004f1b60; body bytes: 164 */

undefined4 * __cdecl FUN_004f1b60(undefined4 *param_1,undefined4 param_2,char param_3,char param_4)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var1;
  default_delete<class_std::_Facet_base> *pdVar2;
  undefined1 *puVar3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  char local_c;
  undefined1 local_7;
  undefined1 local_6;
  undefined1 local_5;
  
  if (param_4 == '\0') {
    local_c = param_3;
    if (param_3 == '\x01') {
      local_6 = 0x2b;
      puVar3 = &local_6;
      p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_004f57f0(&param_2,&local_14);
      pdVar2 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(p_Var1);
      FUN_004f5650(pdVar2,puVar3);
    }
    else if (param_3 == '\x03') {
      local_7 = 0x20;
      puVar3 = &local_7;
      p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_004f57f0(&param_2,&local_18);
      pdVar2 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(p_Var1);
      FUN_004f5650(pdVar2,puVar3);
    }
  }
  else {
    local_5 = 0x2d;
    puVar3 = &local_5;
    p_Var1 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_004f57f0(&param_2,&local_10);
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var1);
    FUN_004f5650(pdVar2,puVar3);
  }
  *param_1 = param_2;
  return param_1;
}

