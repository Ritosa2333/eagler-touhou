/* Entry: 0x004ea0f0; symbol: FUN_004ea0f0; body bytes: 142 */

undefined4 * __cdecl
FUN_004ea0f0(undefined4 *param_1,undefined4 param_2,uint param_3,int *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  
  iVar1 = *param_4;
  iVar2 = param_4[1];
  uVar3 = param_4[2];
  iVar4 = param_4[3];
  iVar5 = param_4[4];
  puVar6 = (undefined4 *)move<>(&param_2);
  FUN_004f0a40(param_1,*puVar6,param_3,iVar1,iVar2,uVar3,iVar4,iVar5,param_5);
  return param_1;
}

