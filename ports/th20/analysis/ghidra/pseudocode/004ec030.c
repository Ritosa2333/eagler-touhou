/* Entry: 0x004ec030; symbol: FUN_004ec030; body bytes: 129 */

void __cdecl FUN_004ec030(undefined4 *param_1,undefined4 param_2,int param_3,int param_4)

{
  char *pcVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  undefined1 *local_2c;
  char local_20 [24];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar6 = 10;
  iVar4 = param_3;
  iVar5 = param_4;
  pcVar1 = (char *)FUN_004f3310((int)local_20);
  uVar3 = FUN_00459240(local_20,pcVar1,iVar4,iVar5,uVar6);
  local_2c = (undefined1 *)uVar3;
  puVar2 = (undefined4 *)move<>(&param_2);
  FUN_004ef4d0(param_1,local_20,local_2c,*puVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

