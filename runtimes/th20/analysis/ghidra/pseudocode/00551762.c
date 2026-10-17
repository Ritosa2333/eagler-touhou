/* Entry: 0x00551762; symbol: FUN_00551762; body bytes: 76 */

undefined8 __cdecl FUN_00551762(undefined4 param_1,undefined4 *param_2,uint param_3)

{
  wchar_t *extraout_ECX;
  undefined8 uVar1;
  wchar_t *pwVar2;
  wchar_t *pwVar3;
  byte bVar4;
  int local_30 [11];
  
  FUN_00547db0(local_30,(undefined4 *)0x0);
  bVar4 = 0;
  pwVar2 = extraout_ECX;
  pwVar3 = extraout_ECX;
  make_c_string_character_source<>((undefined4 *)&stack0xffffffb8,param_1,param_2);
  uVar1 = FUN_00550abe(local_30,pwVar2,(int *)pwVar3,param_3,bVar4);
  FUN_00547df8(local_30);
  return uVar1;
}

