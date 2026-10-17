/* Entry: 0x0052d6e0; symbol: FUN_0052d6e0; body bytes: 228 */

void __cdecl FUN_0052d6e0(undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined4 in_stack_00000018;
  undefined4 in_stack_0000001c;
  undefined4 local_28 [2];
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&stack0x00000008,(char **)&stack0x00000010);
  FUN_0040c080(&local_18,8);
  puVar1 = (undefined4 *)move<>(&stack0x00000008);
  local_18 = *puVar1;
  local_14 = puVar1[1];
  FUN_0040c080(&local_10,8);
  puVar1 = (undefined4 *)move<>(&stack0x00000010);
  local_10 = *puVar1;
  local_c = puVar1[1];
  FUN_0040c080(&local_20,8);
  FUN_0052d3d0((int)&local_18,&local_10);
  puVar1 = (undefined4 *)move<>(&stack0x00000018);
  local_20 = *puVar1;
  local_1c = puVar1[1];
  puVar1 = FUN_0052d450(local_28);
  FUN_0052d5a0(&stack0x00000018,puVar1);
  *param_1 = in_stack_00000018;
  param_1[1] = in_stack_0000001c;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

