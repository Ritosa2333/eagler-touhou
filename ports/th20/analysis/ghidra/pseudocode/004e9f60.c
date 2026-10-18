/* Entry: 0x004e9f60; symbol: FUN_004e9f60; body bytes: 125 */

void __cdecl FUN_004e9f60(undefined4 *param_1,undefined4 param_2,uint param_3)

{
  char *pcVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  uint uVar4;
  uint uVar5;
  undefined1 *local_2c;
  char local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar5 = 10;
  uVar4 = param_3;
  pcVar1 = (char *)FUN_004f3310((int)local_20);
  uVar3 = FUN_00459210(local_20,pcVar1,uVar4,uVar5);
  local_2c = (undefined1 *)uVar3;
  puVar2 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,local_2c,*puVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

