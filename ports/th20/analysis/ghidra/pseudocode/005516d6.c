/* Entry: 0x005516d6; symbol: FUN_005516d6; body bytes: 70 */

uint __cdecl FUN_005516d6(undefined4 param_1,undefined4 *param_2,uint param_3)

{
  uint uVar1;
  byte *extraout_ECX;
  byte *pbVar2;
  byte *pbVar3;
  byte bVar4;
  int local_30 [11];
  
  FUN_00547db0(local_30,(undefined4 *)0x0);
  bVar4 = 1;
  pbVar2 = extraout_ECX;
  pbVar3 = extraout_ECX;
  make_c_string_character_source<>((undefined4 *)&stack0xffffffbc,param_1,param_2);
  uVar1 = FUN_00550250(local_30,pbVar2,(undefined4 *)pbVar3,param_3,bVar4);
  FUN_00547df8(local_30);
  return uVar1;
}

