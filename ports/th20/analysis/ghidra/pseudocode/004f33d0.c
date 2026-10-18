/* Entry: 0x004f33d0; symbol: FUN_004f33d0; body bytes: 143 */

void FUN_004f33d0(void)

{
  char cVar1;
  uint *puVar2;
  uint *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  local_c = (uint *)FUN_0040b2a0((undefined4 *)&stack0x00000004);
  puVar2 = (uint *)FUN_0040b2a0((undefined4 *)&stack0x00000008);
  while ((local_c != puVar2 &&
         (cVar1 = FUN_004f6f60(&stack0x0000000c,*local_c,local_c[1]), cVar1 == '\0'))) {
    local_c = local_c + 2;
  }
  FUN_004185e0((undefined4 *)&stack0x00000004,&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

