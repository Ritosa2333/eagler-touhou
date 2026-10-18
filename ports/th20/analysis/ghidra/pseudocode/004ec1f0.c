/* Entry: 0x004ec1f0; symbol: FUN_004ec1f0; body bytes: 146 */

undefined4 * __cdecl
FUN_004ec1f0(undefined4 *param_1,undefined4 param_2,uint param_3,int param_4,int *param_5,
            undefined4 param_6)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  
  iVar1 = *param_5;
  iVar2 = param_5[1];
  uVar3 = param_5[2];
  iVar4 = param_5[3];
  iVar5 = param_5[4];
  puVar6 = (undefined4 *)move<>(&param_2);
  FUN_004f1480(param_1,*puVar6,param_3,param_4,iVar1,iVar2,uVar3,iVar4,iVar5,param_6);
  return param_1;
}

