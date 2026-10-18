/* Entry: 0x0053fca0; symbol: FUN_0053fca0; body bytes: 151 */

void __cdecl FUN_0053fca0(undefined4 *param_1,char *param_2)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>(&param_2,(char **)&stack0x0000000c);
  local_c = (undefined4 *)FUN_004690e0(&param_2);
  puVar2 = (undefined4 *)FUN_004690e0((undefined4 *)&stack0x0000000c);
  while ((local_c != puVar2 && (bVar1 = FUN_0053fdb0(&stack0x00000010,(char *)*local_c), !bVar1))) {
    local_c = local_c + 2;
  }
  FUN_004ee740(&param_2,&local_c);
  *param_1 = param_2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

