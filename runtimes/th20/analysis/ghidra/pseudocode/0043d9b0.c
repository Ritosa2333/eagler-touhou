/* Entry: 0x0043d9b0; symbol: FUN_0043d9b0; body bytes: 809 */

undefined4 __thiscall
FUN_0043d9b0(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            int param_6,float param_7,float param_8)

{
  float *pfVar1;
  uint uVar2;
  int *piVar3;
  float10 fVar4;
  float fVar5;
  float local_18;
  float local_14;
  uint local_10;
  float *local_8;
  
  if (((uint)((param_6 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_6)) {
    pfVar1 = *(float **)((int)this + 0x7d40e80);
    local_14 = param_5;
    FUN_004455c0((int)this);
    *pfVar1 = param_1;
    pfVar1[1] = param_2;
    pfVar1[2] = 0.0;
    pfVar1[3] = DAT_0056c8cc;
    pfVar1[4] = param_7;
    *pfVar1 = *pfVar1 + *(float *)((int)this + 0xd0);
    pfVar1[1] = pfVar1[1] + *(float *)((int)this + 0xd4);
    fVar5 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_10 = 0; local_8 = pfVar1 + 5, (int)local_10 < param_6 + 1; local_10 = local_10 + 1) {
      uVar2 = local_10 & 0x80000001;
      if ((int)uVar2 < 0) {
        uVar2 = (uVar2 - 1 | 0xfffffffe) + 1;
      }
      if (uVar2 == 0) {
        local_18 = param_3;
      }
      else {
        local_18 = param_4;
      }
      FUN_00439330(local_8,local_8,local_14,local_18);
      *local_8 = *local_8 + param_1;
      pfVar1[6] = pfVar1[6] + param_2;
      pfVar1[7] = 0.0;
      pfVar1[8] = DAT_0056c8cc;
      pfVar1[9] = param_8;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      pfVar1[6] = pfVar1[6] + *(float *)((int)this + 0xd4);
      fVar4 = (float10)FUN_00438600(local_14,fVar5 / (float)param_6);
      local_14 = (float)fVar4;
      pfVar1 = local_8;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar3 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar3 + 0x164))(piVar3,0x44);
    piVar3 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar3 + 0x14c))(piVar3,6,param_6,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_6 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

