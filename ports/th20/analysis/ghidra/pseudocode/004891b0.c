/* Entry: 0x004891b0; symbol: FUN_004891b0; body bytes: 219 */

void __cdecl FUN_004891b0(void *param_1,void *param_2)

{
  undefined1 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 uVar5;
  undefined1 local_14;
  undefined1 local_13;
  undefined1 local_12;
  undefined1 local_11;
  undefined1 local_10;
  undefined1 local_f;
  undefined1 local_e;
  undefined1 local_d;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(param_1,4);
  FUN_0045edf0(param_1,param_2);
  local_12 = FUN_00489c80(&stack0x00000010);
  local_f = local_12;
  local_e = local_12;
  local_d = local_12;
  puVar1 = (undefined1 *)FUN_00489ca0(&local_14);
  local_13 = *puVar1;
  local_11 = local_13;
  local_10 = local_13;
  iVar2 = FUN_00489190((int)param_2);
  puVar3 = (undefined4 *)move<>(param_1);
  iVar4 = FUN_0048a800(puVar3);
  local_c = FUN_00489a70(iVar4,iVar2);
  uVar5 = move<>(&local_c);
  FUN_004670e0(param_1,uVar5);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

