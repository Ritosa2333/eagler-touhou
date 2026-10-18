/* Entry: 0x00470310; symbol: FUN_00470310; body bytes: 72 */

uint __thiscall FUN_00470310(void *this,uint *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  
  Check_offset(this,param_3);
  uVar1 = std::_String_val<struct_std::_Simple_types<char>_>::_Clamp_suffix_size
                    ((_String_val<struct_std::_Simple_types<char>_> *)this,param_3,param_2);
                    /* WARNING: Load size is inaccurate */
  FUN_0040ffd0(param_1,(uint *)(*this + param_3),uVar1);
  return uVar1;
}

