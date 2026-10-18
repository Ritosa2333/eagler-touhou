/* Entry: 0x0052ef70; symbol: FUN_0052ef70; body bytes: 54 */

void * __thiscall FUN_0052ef70(void *this,void *param_1)

{
  undefined4 uVar1;
  _Container_base12 *p_Var2;
  
  uVar1 = *(undefined4 *)((int)this + 8);
  p_Var2 = std::_Iterator_base12::_Getcont((_Iterator_base12 *)this);
  default_error_condition(param_1,uVar1,p_Var2);
  return param_1;
}

