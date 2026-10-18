/* Entry: 0x004e73b0; symbol: FUN_004e73b0; body bytes: 299 */

void FUN_004e73b0(void)

{
  undefined1 *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int local_18;
  int local_14;
  undefined8 local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000004,(char **)&stack0x00000008);
  puVar2 = (undefined4 *)move<>(&stack0x00000004);
  local_18 = FUN_0040b2a0(puVar2);
  puVar2 = (undefined4 *)move<>(&stack0x00000008);
  local_14 = FUN_0040b2a0(puVar2);
  FUN_004eca30(&local_18,&local_14);
  uVar3 = move<>(&stack0x0000000c);
  move<>(uVar3);
  puVar2 = (undefined4 *)move<>(&local_14);
  puVar1 = (undefined1 *)*puVar2;
  puVar2 = (undefined4 *)move<>(&local_18);
  local_10 = FUN_004e7b70((undefined1 *)*puVar2,puVar1);
  uVar3 = move<>(&local_10);
  FUN_004185e0((undefined4 *)&stack0x00000004,uVar3);
  uVar3 = move<>((int)&local_10 + 4);
  FUN_004ee760((undefined4 *)&stack0x0000000c,uVar3);
  move<>(&stack0x00000004);
  move<>(&stack0x0000000c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

