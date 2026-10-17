/* Entry: 0x0048ba20; symbol: FUN_0048ba20; body bytes: 54 */

undefined4 * FUN_0048ba20(undefined4 *param_1)

{
  undefined1 local_14 [8];
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *local_c;
  undefined4 *local_8;
  
  local_c = FUN_0047b850(local_14,param_1);
  swap(local_c,local_8);
  FUN_0047c300((int)local_14);
  return local_8;
}

