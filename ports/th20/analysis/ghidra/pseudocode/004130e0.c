/* Entry: 0x004130e0; symbol: FUN_004130e0; body bytes: 61 */

void __thiscall FUN_004130e0(void *this,undefined4 param_1)

{
  shared_ptr<struct__EXCEPTION_RECORD_const_> *this_00;
  shared_ptr<struct__EXCEPTION_RECORD_const_> *psVar1;
  
                    /* WARNING: Load size is inaccurate */
  this_00 = (shared_ptr<struct__EXCEPTION_RECORD_const_> *)move<>(0x28,*this);
  psVar1 = (shared_ptr<struct__EXCEPTION_RECORD_const_> *)move<>(param_1);
  std::shared_ptr<struct__EXCEPTION_RECORD_const_>::shared_ptr<struct__EXCEPTION_RECORD_const_>
            (this_00,psVar1);
  return;
}

