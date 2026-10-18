/* Entry: 0x005596de; symbol: FUN_005596de; body bytes: 275 */

int __thiscall
FUN_005596de(void *this,ushort *param_1,undefined4 *param_2,uint param_3,uint *param_4,int param_5)

{
  char cVar1;
  undefined3 extraout_var;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined3 extraout_var_01;
  int iVar5;
  void *pvVar6;
  ushort *puVar7;
  byte *pbVar8;
  void *local_c;
  void *pvStack_8;
  undefined3 extraout_var_00;
  
  pbVar8 = (byte *)*param_2;
  local_c = this;
  pvStack_8 = this;
  if (param_1 == (ushort *)0x0) {
    iVar2 = 0;
    cVar1 = FUN_0055966f((char *)pbVar8);
    uVar3 = CONCAT31(extraout_var_00,cVar1);
    while (iVar5 = FUN_0055940b((uint *)0x0,pbVar8,uVar3,param_4,param_5), iVar5 != -1) {
      if (iVar5 == 0) {
        return iVar2;
      }
      pbVar8 = pbVar8 + iVar5;
      iVar4 = iVar2 + 1;
      if (iVar5 != 4) {
        iVar4 = iVar2;
      }
      iVar2 = iVar4 + 1;
      cVar1 = FUN_0055966f((char *)pbVar8);
      uVar3 = CONCAT31(extraout_var_01,cVar1);
    }
    *(undefined1 *)(param_5 + 0x1c) = 1;
    *(undefined4 *)(param_5 + 0x18) = 0x2a;
    iVar2 = -1;
  }
  else {
    puVar7 = param_1;
    if (param_3 != 0) {
      do {
        cVar1 = FUN_0055966f((char *)pbVar8);
        iVar2 = FUN_0055940b((uint *)&local_c,pbVar8,CONCAT31(extraout_var,cVar1),param_4,param_5);
        if (iVar2 == -1) {
          *param_2 = pbVar8;
          *(undefined1 *)(param_5 + 0x1c) = 1;
          *(undefined4 *)(param_5 + 0x18) = 0x2a;
          return -1;
        }
        if (iVar2 == 0) {
          pbVar8 = (byte *)0x0;
          *puVar7 = 0;
          break;
        }
        pvVar6 = local_c;
        if ((void *)0xffff < local_c) {
          if (param_3 < 2) break;
          local_c = (void *)((int)local_c + -0x10000);
          param_3 = param_3 - 1;
          *puVar7 = (ushort)((uint)local_c >> 10) | 0xd800;
          puVar7 = puVar7 + 1;
          pvVar6 = (void *)((uint)local_c & 0x3ff | 0xdc00);
        }
        *puVar7 = (ushort)pvVar6;
        pbVar8 = pbVar8 + iVar2;
        puVar7 = puVar7 + 1;
        param_3 = param_3 - 1;
      } while (param_3 != 0);
    }
    iVar2 = (int)puVar7 - (int)param_1 >> 1;
    *param_2 = pbVar8;
  }
  return iVar2;
}

