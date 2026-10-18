/* Entry: 0x0040f100; symbol: FUN_0040f100; body bytes: 93 */

void __cdecl FUN_0040f100(undefined4 *param_1,undefined4 *param_2,uint param_3)

{
  LPCWSTR pWVar1;
  DWORD *pDVar2;
  uint uVar3;
  DWORD local_28 [8];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040dea0(param_1);
  uVar3 = 0xffffffff;
  pDVar2 = local_28;
  pWVar1 = (LPCWSTR)FUN_0040ff70(param_2);
  uVar3 = FUN_00541217(pWVar1,pDVar2,param_3,uVar3);
  param_1[2] = uVar3;
  FUN_0040f560(param_1,uVar3,(int)local_28);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

