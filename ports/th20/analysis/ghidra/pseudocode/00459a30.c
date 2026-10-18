/* Entry: 0x00459a30; symbol: FUN_00459a30; body bytes: 64 */

undefined4 __fastcall FUN_00459a30(char **param_1)

{
  if (param_1[0x1e] == (char *)0x1) {
    std::_Adl_verify_range<char*,char_const*>((char **)"Streaming File Close \r\n",param_1);
    CloseHandle(param_1[0x23]);
    param_1[0x23] = (char *)0xffffffff;
  }
  return 0;
}

