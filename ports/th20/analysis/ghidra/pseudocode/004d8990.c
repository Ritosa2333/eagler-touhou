/* Entry: 0x004d8990; symbol: FUN_004d8990; body bytes: 941 */

undefined4 * __fastcall FUN_004d8990(undefined4 *param_1)

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
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  _memset(param_1 + 7,0,0x2c);
  param_1[0x12] = 0;
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)(param_1 + 0x13));
  std::
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
  _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)(param_1 + 0x23));
  param_1[0x33] = 0;
  param_1[0x34] = 0;
  param_1[0x35] = 0;
  param_1[0x36] = 0;
  param_1[0x37] = 0;
  param_1[0x38] = 0;
  _memset(param_1 + 0x39,0,0x38);
  _memset(param_1 + 0x47,0,0x38);
  _memset(param_1 + 0x55,0,0x38);
  param_1[99] = 0;
  param_1[100] = 0;
  param_1[0x65] = 0;
  param_1[0x66] = 0;
  param_1[0x67] = 0;
  param_1[0x68] = 0;
  param_1[0x69] = 0;
  param_1[0x6a] = 0;
  param_1[0x6b] = 0;
  param_1[0x6c] = 0;
  param_1[0x6d] = 0;
  param_1[0x6e] = 0;
  param_1[0x6f] = 0;
  param_1[0x70] = 0;
  FUN_00425cc0(param_1 + 0x71);
  FUN_004b9c10(param_1 + 0x72);
  FUN_0040bc20(param_1 + 0x9e,0x16c,6,FUN_00471790);
  param_1[0x2c0] = 0;
  param_1[0x2c1] = 0;
  param_1[0x2c2] = 0xfffffffe;
  param_1[0x2c3] = 0xfffffffe;
  param_1[0x2c4] = 0xfffffffe;
  param_1[0x2c5] = 0;
  param_1[0x2c6] = 0;
  param_1[0x2c7] = 0;
  param_1[0x2c8] = 0;
  param_1[0x2c9] = 0;
  param_1[0x2ca] = 0;
  param_1[0x2cb] = 0;
  param_1[0x2cc] = 0;
  param_1[0x2cd] = 0;
  param_1[0x2ce] = 0;
  param_1[0x2cf] = 0;
  param_1[0x2d0] = 0;
  param_1[0x2d1] = 0;
  FUN_004d88c0(param_1 + 0x2d2);
  param_1[0x2d3] = 0;
  param_1[0x2d4] = 0;
  param_1[0x321] = 0;
  param_1[0x322] = 0;
  _memset(param_1 + 0x323,0,0x104);
  FUN_0040b780(param_1 + 0x364);
  FUN_0040b780(param_1 + 0x368);
  param_1[0x36d] = 0;
  param_1[0x36e] = 0;
  param_1[0x36f] = 0;
  param_1[0x371] = 0;
  param_1[0x372] = 0;
  param_1[0x373] = 0;
  param_1[0x374] = 0;
  param_1[0x375] = 0;
  *(undefined8 *)(param_1 + 0x376) = 0;
  param_1[0x378] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

