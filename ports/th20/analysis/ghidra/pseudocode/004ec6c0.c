/* Entry: 0x004ec6c0; symbol: FUN_004ec6c0; body bytes: 201 */

_Ptr_base<struct__EXCEPTION_RECORD_const_> * __cdecl
FUN_004ec6c0(_Ptr_base<struct__EXCEPTION_RECORD_const_> *param_1,undefined1 param_2,int param_3)

{
  switch(param_2) {
  case 0x42:
    FUN_0040eb80(param_1,&DAT_005730b0,2);
    break;
  default:
    std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
              (param_1);
    break;
  case 0x58:
    FUN_0040eb80(param_1,&DAT_005730b8,2);
    break;
  case 0x62:
    FUN_0040eb80(param_1,&DAT_005730ac,2);
    break;
  case 0x6f:
    if (param_3 == 0) {
      std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
                (param_1);
    }
    else {
      FUN_0040eb80(param_1,&DAT_0056cf60,1);
    }
    break;
  case 0x78:
    FUN_0040eb80(param_1,&DAT_005730b4,2);
  }
  return param_1;
}

