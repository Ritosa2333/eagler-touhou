/* Entry: 0x0046a8e0; symbol: FUN_0046a8e0; body bytes: 687 */

undefined4 * __fastcall FUN_0046a8e0(undefined4 *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041fd20(param_1);
  *param_1 = &PTR_FUN_0056f5c0;
  FUN_00448b40(param_1 + 4);
  FUN_00448b40(param_1 + 0x17d);
  FUN_00448b40(param_1 + 0x2f6);
  FUN_0046a6b0(param_1 + 0x46f);
  param_1[0x686f] = 0;
  param_1[0x6870] = 0xffffffff;
  param_1[0x6871] = 0xff000000;
  param_1[0x6872] = 0xffffffff;
  param_1[0x6873] = DAT_0056c8cc;
  param_1[0x6874] = DAT_0056c8cc;
  param_1[0x6875] = 0;
  param_1[0x6876] = 0;
  param_1[0x6877] = 0;
  param_1[0x6878] = 0;
  param_1[0x6879] = 2;
  param_1[0x687a] = 0;
  param_1[0x687b] = 0;
  param_1[0x687c] = 0;
  param_1[0x687d] = 1;
  param_1[0x687e] = 1;
  param_1[0x687f] = 0;
  std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
            ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)(param_1 + 0x6880));
  FUN_00429210(param_1 + 0x6882,0.0);
  FUN_00418a50(param_1 + 0x6883);
  FUN_0040b780(param_1 + 0x6889);
  FUN_0046a8b0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0x688d));
  param_1[0x688f] = 9;
  param_1[0x6890] = 0;
  param_1[0x6891] = 0;
  FUN_00425cc0(param_1 + 0x6892);
  FUN_00425cc0(param_1 + 0x6893);
  param_1[0x6894] = 0;
  param_1[0x6895] = 0;
  param_1[0x6896] = 0;
  param_1[0x6897] = 0;
  _memset(param_1 + 0x6898,0,0x100);
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize AsciiInf\n",ppcVar1);
  DAT_005c0698 = param_1;
  param_1[0x6870] = 0xffffffff;
  param_1[0x6871] = 0xff000000;
  param_1[0x6873] = DAT_0056c8cc;
  param_1[0x6874] = DAT_0056c8cc;
  param_1[0x6876] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

