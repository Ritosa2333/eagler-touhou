/* Entry: 0x004f37d0; symbol: FUN_004f37d0; body bytes: 218 */

void __cdecl FUN_004f37d0(undefined4 param_1,undefined4 param_2,uint *param_3)

{
  char cVar1;
  int iVar2;
  uint *puVar3;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  std::_Adl_verify_range<char*,char_const*>((char **)&param_1,(char **)&param_2);
  local_c = FUN_0040b2a0(&param_1);
  iVar2 = FUN_0040b2a0(&param_2);
  iVar2 = FUN_004f3300(local_c,iVar2);
  while (local_10 = iVar2, 0 < local_10) {
    iVar2 = local_10 / 2;
    puVar3 = (uint *)FUN_004f38f0(local_c,iVar2);
    cVar1 = FUN_004e6dc0(puVar3,param_3);
    if (cVar1 != '\0') {
      local_c = FUN_004ed740((int)puVar3);
      iVar2 = local_10 - (iVar2 + 1);
    }
  }
  FUN_004185e0(&param_1,&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

