/* Entry: 0x00514490; symbol: FUN_00514490; body bytes: 179 */

void __cdecl FUN_00514490(void *param_1,void *param_2,void *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 uVar4;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(param_1,4);
  FUN_0045ee90(param_1,param_2);
  FUN_00489c80(&stack0x00000010);
  iVar1 = FUN_00514470((int)param_2);
  puVar2 = (undefined4 *)move<>(param_1);
  iVar3 = FUN_0048a800(puVar2);
  local_c = FUN_00514550(iVar3,iVar1,param_3);
  uVar4 = move<>(&local_c);
  FUN_004670e0(param_1,uVar4);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

