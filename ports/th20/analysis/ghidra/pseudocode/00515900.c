/* Entry: 0x00515900; symbol: FUN_00515900; body bytes: 212 */

basic_streambuf<char,struct_std::char_traits<char>_> * __fastcall
FUN_00515900(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  locale *this;
  undefined4 local_10;
  
  *(undefined ***)param_1 = &PTR_FID_conflict__scalar_deleting_destructor__00574da0;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 0xc) = 0;
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x20) = 0;
  *(undefined4 *)(param_1 + 0x24) = 0;
  *(undefined4 *)(param_1 + 0x28) = 0;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x30) = 0;
  this = (locale *)operator_new(8);
  if (this == (locale *)0x0) {
    local_10 = 0;
  }
  else {
    FUN_0040c080(this,8);
    local_10 = std::locale::locale(this);
  }
  *(undefined4 *)(param_1 + 0x34) = local_10;
  std::basic_streambuf<char,struct_std::char_traits<char>_>::_Init(param_1);
  return param_1;
}

