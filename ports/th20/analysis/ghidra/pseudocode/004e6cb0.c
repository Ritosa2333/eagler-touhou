/* Entry: 0x004e6cb0; symbol: FUN_004e6cb0; body bytes: 262 */

void __cdecl FUN_004e6cb0(int *param_1)

{
  undefined4 uVar1;
  undefined1 *puVar2;
  undefined4 *puVar3;
  undefined1 *puVar4;
  undefined8 local_14;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004eca10((int)param_1);
  FUN_0040c080(local_c,4);
  FUN_004e6c30(local_c,param_1);
  uVar1 = move<>(&stack0x00000008);
  move<>(uVar1);
  puVar2 = (undefined1 *)FUN_004e6c70(param_1);
  puVar3 = (undefined4 *)move<>(local_c);
  puVar4 = (undefined1 *)FUN_004ee850(puVar3);
  local_14 = FUN_004e7b70(puVar4,puVar2);
  uVar1 = move<>(&local_14);
  FUN_004670e0(local_c,uVar1);
  uVar1 = move<>((int)&local_14 + 4);
  FUN_004ee760((undefined4 *)&stack0x00000008,uVar1);
  move<>(local_c);
  move<>(&stack0x00000008);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

