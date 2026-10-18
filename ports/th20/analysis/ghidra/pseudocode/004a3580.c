/* Entry: 0x004a3580; symbol: FUN_004a3580; body bytes: 104 */

undefined4 * __fastcall FUN_004a3580(undefined4 *param_1)

{
  *param_1 = 0;
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 1));
  _String_val<>(param_1 + 3);
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  *(undefined1 *)(param_1 + 0xc) = 0;
  FUN_004141c0(param_1 + 0xd);
  param_1[0x11] = 0;
  return param_1;
}

