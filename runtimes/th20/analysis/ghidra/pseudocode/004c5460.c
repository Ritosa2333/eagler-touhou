/* Entry: 0x004c5460; symbol: FUN_004c5460; body bytes: 150 */

void FUN_004c5460(void)

{
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_10 = FUN_0040b2a0((undefined4 *)&stack0x00000004);
  local_c = FUN_0040b2a0((undefined4 *)&stack0x00000008);
  FUN_004c52c0(&local_10,&local_c);
  local_14 = FUN_0040b2a0((undefined4 *)&stack0x0000000c);
  local_18 = FUN_004c52e0();
  FUN_004185e0((undefined4 *)&stack0x0000000c,&local_18);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

