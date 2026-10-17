/* Entry: 0x00558d59; symbol: FUN_00558d59; body bytes: 199 */

void __cdecl
FUN_00558d59(undefined4 *param_1,char *param_2,uint param_3,char *param_4,int param_5,size_t param_6
            ,char param_7,int param_8,int param_9,int *param_10)

{
  uint *puVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  bool bVar5;
  int local_14;
  int local_10;
  undefined4 uStack_c;
  int local_8;
  
  local_14 = 0;
  local_10 = 0;
  uStack_c = 0;
  iVar2 = FUN_0056281d((uint *****)*param_1,param_1[1],param_6,0,&local_14,param_4,param_5);
  bVar5 = local_14 == 0x2d;
  local_8 = local_10 + -1;
  uVar3 = param_3 - bVar5;
  if (param_3 == 0xffffffff) {
    uVar3 = 0xffffffff;
  }
  iVar2 = FUN_00562724((uint *)(param_2 + bVar5),uVar3,param_6,&local_14,iVar2,param_9,param_10);
  if (iVar2 == 0) {
    iVar2 = local_10 + -1;
    if ((iVar2 < -4) || ((int)param_6 <= iVar2)) {
      FUN_00558aad(param_2,param_3,param_6,param_7,param_8,&local_14,1,param_10);
    }
    else {
      puVar1 = (uint *)(param_2 + bVar5);
      if (local_8 < iVar2) {
        do {
          puVar4 = puVar1;
          puVar1 = (uint *)((int)puVar4 + 1);
        } while ((char)*puVar4 != '\0');
        *(undefined1 *)((int)puVar4 + -1) = 0;
      }
      FUN_00558c79(param_2,param_3,param_6,&local_14,'\x01',param_10);
    }
  }
  else {
    *param_2 = '\0';
  }
  return;
}

