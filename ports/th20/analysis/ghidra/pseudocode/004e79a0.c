/* Entry: 0x004e79a0; symbol: FUN_004e79a0; body bytes: 226 */

undefined4 * __thiscall FUN_004e79a0(void *this,undefined4 *param_1,byte param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  void *local_20;
  undefined4 local_1c;
  void *local_18;
  undefined4 *local_14;
  
                    /* WARNING: Load size is inaccurate */
  local_18 = *this;
  local_14 = (undefined4 *)this;
  puVar7 = FUN_004fc010(local_18,&local_30);
  local_28 = *puVar7;
  piVar1 = (int *)local_14[1];
  iVar2 = *piVar1;
  iVar3 = piVar1[1];
  uVar4 = piVar1[2];
  uVar5 = piVar1[3];
  iVar6 = piVar1[4];
  local_20 = (void *)*local_14;
  local_1c = local_28;
  puVar7 = FUN_004ff9e0(local_20,&local_34);
  local_2c = *puVar7;
  local_24 = local_2c;
  FUN_004e9c70(param_1,local_2c,param_2,iVar2,iVar3,uVar4,uVar5,iVar6,local_28);
  return param_1;
}

