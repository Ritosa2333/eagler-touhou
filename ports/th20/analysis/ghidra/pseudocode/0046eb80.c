/* Entry: 0x0046eb80; symbol: FUN_0046eb80; body bytes: 46 */

void __thiscall FUN_0046eb80(void *this,char param_1)

{
  Concurrency::details::_Condition_variable::_scalar_deleting_destructor_
            ((_Condition_variable *)this,0);
  if (param_1 != '\0') {
    FUN_0040cfc0(this,0x24);
  }
  return;
}

