/* Entry: 0x00445350; symbol: FUN_00445350; body bytes: 612 */

undefined4 __thiscall FUN_00445350(void *this,int param_1,float *param_2,int param_3)

{
  code *pcVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined3 extraout_var;
  int iVar4;
  int *piVar5;
  int local_14;
  float *local_10;
  char local_5;
  
  if (param_3 < 3) {
    uVar3 = 0xffffffff;
  }
  else {
    bVar2 = FUN_00445ab0(param_1);
    if (CONCAT31(extraout_var,bVar2) == 0) {
      uVar3 = 0xffffffff;
    }
    else if ((*(uint *)(param_1 + 0x49c) & 1) == 0) {
      uVar3 = 0xffffffff;
    }
    else if (*(char *)(param_1 + 0x493) == '\0') {
      uVar3 = 0xffffffff;
    }
    else {
      if (*(int *)((int)this + 0x6000e70) != 0) {
        FUN_004455c0((int)this);
      }
      if ((*(uint *)(param_1 + 0x49c) >> 0x15 & 1) == 0) {
        *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 0x200000;
        local_10 = param_2;
        for (local_14 = 0; local_14 < param_3; local_14 = local_14 + 1) {
          *local_10 = *local_10 + *(float *)((int)this + 0xd0);
          local_10[1] = local_10[1] + *(float *)((int)this + 0xd4);
          local_10 = local_10 + 7;
        }
      }
      iVar4 = FUN_00437cd0(param_1);
      if (*(int *)((int)this + 0x6000e08) != *(int *)(iVar4 + 8)) {
        *(undefined4 *)((int)this + 0x6000e08) = *(undefined4 *)(iVar4 + 8);
        piVar5 = (int *)FUN_00412730(0x5c4d40);
        pcVar1 = *(code **)(*piVar5 + 0x104);
        uVar3 = FUN_00445a00(this,*(uint *)((int)this + 0x6000e08));
        (*pcVar1)(piVar5,0,uVar3);
      }
      if (*(char *)((int)this + 0x6000e0e) != '\x03') {
        piVar5 = (int *)FUN_00412730(0x5c4d40);
        (**(code **)(*piVar5 + 0x164))(piVar5,0x144);
        piVar5 = (int *)FUN_00412730(0x5c4d40);
        (**(code **)(*piVar5 + 0x10c))(piVar5,0,6,0);
        piVar5 = (int *)FUN_00412730(0x5c4d40);
        (**(code **)(*piVar5 + 0x10c))(piVar5,0,3,0);
        *(undefined1 *)((int)this + 0x6000e0e) = 3;
      }
      FUN_00445ba0(this,param_1);
      if ((*(byte *)(param_1 + 0x4a0) >> 4 & 3) == 3) {
        local_5 = '\x03';
      }
      else {
        local_5 = '\0';
      }
      FUN_00446690(this,local_5);
      piVar5 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar5 + 0x14c))(piVar5,5,param_3 + -2,param_2,0x1c);
      uVar3 = 0;
    }
  }
  return uVar3;
}

