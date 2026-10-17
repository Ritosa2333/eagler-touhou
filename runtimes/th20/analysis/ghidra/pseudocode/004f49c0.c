/* Entry: 0x004f49c0; symbol: FUN_004f49c0; body bytes: 440 */

undefined4 * __fastcall FUN_004f49c0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  FUN_00422e10(param_1 + 1);
  FUN_00422e10(param_1 + 4);
  FUN_0040bc20(param_1 + 7,0xc,7,FUN_00422e10);
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x1c));
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x1e));
  FUN_0040bc20(param_1 + 0x20,8,7,
               std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::
               _Ptr_base<struct__EXCEPTION_RECORD_const_>);
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x2e));
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x30));
  FUN_00422e10(param_1 + 0x32);
  FUN_00447de0(param_1 + 0x35);
  param_1[0x36] = 0;
  FUN_00425cc0(param_1 + 0x37);
  FUN_00425cc0(param_1 + 0x38);
  FUN_00422d90(param_1 + 0x39);
  param_1[0x3d] = 0;
  param_1[0x3e] = 0;
  param_1[0x3f] = 0;
  param_1[0x40] = 0;
  param_1[0x41] = 0;
  param_1[0x42] = 0;
  param_1[0x43] = 0;
  param_1[0x44] = 0;
  FUN_00425cc0(param_1 + 0x45);
  param_1[0x46] = 0;
  param_1[0x47] = 0;
  param_1[0x48] = 0;
  param_1[0x49] = 0;
  param_1[0x4a] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

