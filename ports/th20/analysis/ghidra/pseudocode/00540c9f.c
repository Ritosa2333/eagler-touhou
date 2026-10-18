/* Entry: 0x00540c9f; symbol: FUN_00540c9f; body bytes: 80 */

/* WARNING: Function: __EH_prolog3_GS replaced with injection: EH_prolog3 */

void __thiscall FUN_00540c9f(void *this,undefined4 param_1,undefined4 param_2,uint *param_3)

{
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_2c [36];
  undefined4 local_8;
  undefined4 uStack_4;
  
  uStack_4 = 0x20;
  local_8 = 0x540cab;
  FUN_0040db70(local_2c,param_3);
  local_8 = 0;
  FUN_0040df50(param_1,param_2,local_2c);
  FUN_0040f9e0(local_2c);
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__0056c4f8;
  FUN_00543601();
  return;
}

