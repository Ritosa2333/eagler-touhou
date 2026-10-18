/* Entry: 0x0046a420; symbol: FUN_0046a420; body bytes: 139 */

void FUN_0046a420(void)

{
  bool bVar1;
  uint *puVar2;
  uint *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_c = (uint *)FUN_0040b2a0((undefined4 *)&stack0x00000004);
  puVar2 = (uint *)FUN_0040b2a0((undefined4 *)&stack0x00000008);
  while ((local_c != puVar2 && (bVar1 = FUN_0046b360(&stack0x0000000c,*local_c), !bVar1))) {
    local_c = local_c + 1;
  }
  FUN_004185e0((undefined4 *)&stack0x00000004,&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

