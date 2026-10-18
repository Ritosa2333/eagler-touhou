/* Entry: 0x00489110; symbol: FUN_00489110; body bytes: 56 */

void __fastcall FUN_00489110(undefined4 *param_1)

{
  _Ptr_base<struct__EXCEPTION_RECORD_const_> *_Dst;
  
  _Dst = (_Ptr_base<struct__EXCEPTION_RECORD_const_> *)move<>(0x4c,*param_1);
  _memset(_Dst,0,0x4c);
  FUN_0048b500(_Dst);
  return;
}

