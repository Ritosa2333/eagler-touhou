/* Entry: 0x00459a70; symbol: FUN_00459a70; body bytes: 757 */

void __thiscall
FUN_00459a70(void *this,int *param_1,char **param_2,uint param_3,undefined4 param_4,
            undefined4 param_5,undefined4 param_6,undefined4 param_7,uint param_8,int param_9,
            char *param_10,char *param_11)

{
  undefined4 *puVar1;
  int iVar2;
  char **ppcVar3;
  uint *puVar4;
  uint *puVar5;
  uint local_3c;
  uint local_34 [4];
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 *local_10;
  int *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
                    /* WARNING: Load size is inaccurate */
  if (*this != 0) {
    local_10 = (undefined4 *)0x0;
    puVar1 = FUN_004594d0();
    iVar2 = FUN_0045ab40(puVar1,param_2,param_11,(char *)0x1);
    if (iVar2 == 0) {
      _memset(local_34,0,0x24);
      local_34[0] = 0x24;
      local_34[1] = param_3 | 0x18188;
      local_20 = param_4;
      local_1c = param_5;
      local_18 = param_6;
      local_14 = param_7;
      local_24 = puVar1[0x24] + 0x20;
      local_c = (int *)0x0;
                    /* WARNING: Load size is inaccurate */
                    /* WARNING: Load size is inaccurate */
      local_34[2] = param_9 * param_8;
      iVar2 = (**(code **)(**this + 0xc))(*this,local_34,&local_10,0);
      if ((((-1 < iVar2) && (local_10 != (undefined4 *)0x0)) &&
          (iVar2 = (**(code **)*local_10)(local_10,&DAT_00576c28,&local_c), -1 < iVar2)) &&
         ((local_c != (int *)0x0 &&
          (ppcVar3 = (char **)FUN_00459350("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\zwave.cpp:154 DSBPOSITIONNOTIFY"
                                           ,param_8), ppcVar3 != (char **)0x0)))) {
        for (local_3c = 0; local_3c < param_8; local_3c = local_3c + 1) {
          ppcVar3[local_3c * 2] = (char *)(param_9 * local_3c + -1 + param_9);
          ppcVar3[local_3c * 2 + 1] = param_10;
        }
        iVar2 = (**(code **)(*local_c + 0xc))(local_c,param_8,ppcVar3);
        if (iVar2 < 0) {
          if (local_c != (int *)0x0) {
            (**(code **)(*local_c + 8))(local_c);
            local_c = (int *)0x0;
          }
          if (ppcVar3 != (char **)0x0) {
            FUN_00413b70(DAT_005b8894,ppcVar3);
          }
        }
        else {
          if (local_c != (int *)0x0) {
            (**(code **)(*local_c + 8))(local_c);
            local_c = (int *)0x0;
          }
          if (ppcVar3 != (char **)0x0) {
            FUN_00413b70(DAT_005b8894,ppcVar3);
          }
          puVar1 = FUN_004593f0();
          *param_1 = (int)puVar1;
          puVar4 = local_34;
          puVar5 = (uint *)(*param_1 + 0x60);
          for (iVar2 = 9; iVar2 != 0; iVar2 = iVar2 + -1) {
            *puVar5 = *puVar4;
            puVar4 = puVar4 + 1;
            puVar5 = puVar5 + 1;
          }
          *(void **)(*param_1 + 0x84) = this;
          *(char **)(*param_1 + 0xa0) = param_10;
          *(undefined4 *)(*param_1 + 0xa4) = 0;
        }
      }
    }
    else {
      FUN_004592e0(puVar1);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

