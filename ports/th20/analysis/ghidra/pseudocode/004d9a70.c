/* Entry: 0x004d9a70; symbol: FUN_004d9a70; body bytes: 211 */

void FUN_004d9a70(undefined4 param_1,int param_2)

{
  size_t sVar1;
  char local_108 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00548610(local_108,0x100,param_2);
  sVar1 = _strlen(local_108);
  builtin_strncpy(local_108 + sVar1,".wav",5);
  FUN_00428c90(&DAT_005ba830,(char **)0x1,param_1,(int)local_108);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

