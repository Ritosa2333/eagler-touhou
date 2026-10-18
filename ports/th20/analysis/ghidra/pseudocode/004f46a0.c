/* Entry: 0x004f46a0; symbol: FUN_004f46a0; body bytes: 794 */

undefined4 * __fastcall FUN_004f46a0(undefined4 *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041fd20(param_1);
  *param_1 = &PTR_FUN_00572bac;
  param_1[4] = 0;
  param_1[5] = 0;
  FUN_00447de0(param_1 + 6);
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  FUN_00448b40(param_1 + 10);
  FUN_00425cc0(param_1 + 0x183);
  FUN_00425cc0(param_1 + 0x184);
  FUN_00422e10(param_1 + 0x185);
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x188));
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x18a));
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x18c));
  FUN_00422e10(param_1 + 0x18e);
  FUN_00422d90(param_1 + 0x191);
  FUN_00422d90(param_1 + 0x195);
  FUN_00422d90(param_1 + 0x199);
  param_1[0x19d] = 0;
  param_1[0x19e] = 0;
  param_1[0x19f] = 0;
  param_1[0x1a0] = 0;
  FUN_004f4110(param_1 + 0x1a1);
  FUN_004f4170(param_1 + 0x48f);
  *(undefined1 *)(param_1 + 0x813) = 0;
  FUN_00422d90(param_1 + 0x814);
  FUN_00422d90(param_1 + 0x818);
  FUN_00422d90(param_1 + 0x81c);
  FUN_004f4650(param_1 + 0x820);
  FUN_004f4610(param_1 + 0x825);
  param_1[0x82d] = 0;
  param_1[0x82e] = 0;
  param_1[0x82f] = 0;
  param_1[0x830] = 0;
  FUN_00422e10(param_1 + 0x831);
  FUN_00422e10(param_1 + 0x834);
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x837));
  param_1[0x839] = 0;
  param_1[0x83a] = 0;
  param_1[0x83b] = 0;
  FUN_00422e10(param_1 + 0x83c);
  FUN_004f40c0(param_1 + 0x83f);
  param_1[0x881] = 0;
  param_1[0x882] = 0;
  FUN_004479e0(param_1 + 0x883);
  param_1[0x88e] = 0;
  param_1[0x88f] = 0;
  param_1[0x890] = 0;
  FUN_004f4580(param_1 + 0x891);
  param_1[0x8a8] = 0;
  param_1[0x8a9] = 0;
  param_1[0x8aa] = 0;
  param_1[0x8ab] = 0;
  param_1[0x8ac] = 0;
  FUN_004f4b80((int)(param_1 + 0x8ad));
  FUN_00447de0(param_1 + 0x5213);
  param_1[0x5215] = 0;
  param_1[0x5216] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize PlayerInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

