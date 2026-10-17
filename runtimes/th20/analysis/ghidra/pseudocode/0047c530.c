/* Entry: 0x0047c530; symbol: FUN_0047c530; body bytes: 63 */

undefined4 * FUN_0047c530(undefined4 param_1)

{
  shared_ptr<struct__EXCEPTION_RECORD_const_> *psVar1;
  shared_ptr<struct__EXCEPTION_RECORD_const_> local_14 [8];
  void *local_c;
  undefined4 *local_8;
  
  psVar1 = (shared_ptr<struct__EXCEPTION_RECORD_const_> *)move<>(param_1);
  local_c = (void *)std::shared_ptr<struct__EXCEPTION_RECORD_const_>::
                    shared_ptr<struct__EXCEPTION_RECORD_const_>(local_14,psVar1);
  swap(local_c,local_8);
  FUN_0047c300((int)local_14);
  return local_8;
}

