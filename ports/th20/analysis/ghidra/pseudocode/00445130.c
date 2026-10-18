/* Entry: 0x00445130; symbol: FUN_00445130; body bytes: 534 */

undefined4 __thiscall FUN_00445130(void *this,int param_1,float *param_2,int param_3)

{
  code *pcVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  int local_10;
  float *local_c;
  
  if (param_3 < 3) {
    uVar2 = 0xffffffff;
  }
  else {
    if (*(int *)((int)this + 0x6000e70) != 0) {
      FUN_004455c0((int)this);
    }
    if ((*(uint *)(param_1 + 0x49c) >> 0x15 & 1) == 0) {
      *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 0x200000;
      local_c = param_2;
      for (local_10 = 0; local_10 < param_3; local_10 = local_10 + 1) {
        *local_c = *local_c + *(float *)((int)this + 0xd0);
        local_c[1] = local_c[1] + *(float *)((int)this + 0xd4);
        local_c = local_c + 7;
      }
    }
    if (*(char *)((int)this + 0x6000e0e) != '\x03') {
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0x164))(piVar3,0x144);
      *(undefined1 *)((int)this + 0x6000e0e) = 3;
    }
    FUN_00445ba0(this,param_1);
    iVar4 = FUN_00437cd0(param_1);
    if (*(int *)((int)this + 0x6000e08) != *(int *)(iVar4 + 8)) {
      *(undefined4 *)((int)this + 0x6000e08) = *(undefined4 *)(iVar4 + 8);
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      pcVar1 = *(code **)(*piVar3 + 0x104);
      uVar2 = FUN_00445a00(this,*(uint *)((int)this + 0x6000e08));
      (*pcVar1)(piVar3,0,uVar2);
    }
    FUN_004ddf80(0x5c4d40);
    if (*(char *)((int)this + 0x6000e0e) != '\x01') {
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0x10c))(piVar3,0,6,0);
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0x10c))(piVar3,0,3,0);
      *(undefined1 *)((int)this + 0x6000e0e) = 1;
    }
    piVar3 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar3 + 0x14c))(piVar3,6,param_3 + -2,param_2,0x1c);
    uVar2 = 0;
  }
  return uVar2;
}

