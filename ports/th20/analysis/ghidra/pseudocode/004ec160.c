/* Entry: 0x004ec160; symbol: FUN_004ec160; body bytes: 129 */

void __cdecl
FUN_004ec160(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char *pcVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  uint uVar6;
  undefined1 *local_2c;
  char local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar6 = 10;
  uVar4 = param_3;
  uVar5 = param_4;
  pcVar1 = (char *)FUN_004f3310((int)local_20);
  uVar3 = FUN_005001f0(local_20,pcVar1,uVar4,uVar5,uVar6);
  local_2c = (undefined1 *)uVar3;
  puVar2 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,local_2c,*puVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

