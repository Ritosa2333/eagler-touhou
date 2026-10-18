/* Entry: 0x00418930; symbol: FUN_00418930; body bytes: 140 */

void FUN_00418930(void)

{
  char cVar1;
  short *psVar2;
  short *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_c = (short *)FUN_0040b2a0((undefined4 *)&stack0x00000004);
  psVar2 = (short *)FUN_0040b2a0((undefined4 *)&stack0x00000008);
  while ((local_c != psVar2 && (cVar1 = FUN_00419150(*local_c), cVar1 != '\0'))) {
    local_c = local_c + 1;
  }
  FUN_004185e0((undefined4 *)&stack0x00000004,&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

