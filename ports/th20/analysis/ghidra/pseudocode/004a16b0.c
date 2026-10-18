/* Entry: 0x004a16b0; symbol: FUN_004a16b0; body bytes: 86 */

void __thiscall FUN_004a16b0(void *this,undefined4 param_1,undefined4 param_2)

{
  void *this_00;
  undefined4 *puVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  undefined1 local_c [4];
  void *local_8;
  
                    /* WARNING: Load size is inaccurate */
  local_8 = this;
  this_00 = (void *)move<>(0x1c,*this);
  puVar1 = (undefined4 *)move<>(param_2);
  FUN_0040b670(local_c,puVar1);
  p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *)move<>(param_1);
  FUN_004a2c50(this_00,p_Var2,local_c);
  return;
}

