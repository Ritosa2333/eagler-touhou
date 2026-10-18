/* Entry: 0x0048b0c0; symbol: FUN_0048b0c0; body bytes: 149 */

void FUN_0048b0c0(void)

{
  bool bVar1;
  int local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  FUN_0040c080(&local_c,4);
  FUN_00489bf0(&local_c,&stack0x00000004);
  FUN_0040c080(&local_10,4);
  FUN_00489bf0(&local_10,&stack0x00000008);
  while( true ) {
    bVar1 = FUN_00449400(&local_c,&local_10);
    if (bVar1) break;
    FUN_0048be00(&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

