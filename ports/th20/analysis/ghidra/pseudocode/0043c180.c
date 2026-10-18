/* Entry: 0x0043c180; symbol: FUN_0043c180; body bytes: 863 */

void __thiscall
FUN_0043c180(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            int param_6,float param_7,float param_8)

{
  float *pfVar1;
  int *piVar2;
  void *extraout_ECX;
  void *pvVar3;
  void *extraout_ECX_00;
  float10 fVar4;
  float fVar5;
  void *local_24;
  float local_20;
  float *local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((uint)((param_6 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_6)) {
    pfVar1 = *(float **)((int)this + 0x7d40e80);
    local_20 = param_5;
    FUN_004455c0((int)this);
    *pfVar1 = param_1;
    pfVar1[1] = param_2;
    pfVar1[2] = 0.0;
    pfVar1[3] = DAT_0056c8cc;
    pfVar1[4] = param_7;
    *pfVar1 = *pfVar1 + *(float *)((int)this + 0xd0);
    pfVar1[1] = pfVar1[1] + *(float *)((int)this + 0xd4);
    FUN_00422e10(&local_14);
    fVar5 = DAT_0056e0f0 * DAT_0056c8d0;
    local_24 = (void *)0x0;
    pvVar3 = extraout_ECX;
    while (local_18 = pfVar1 + 5, (int)local_24 < param_6 + 1) {
      fVar4 = FUN_004397c0(pvVar3,local_20);
      local_14 = (float)fVar4 * param_3;
      fVar4 = FUN_00439820(extraout_ECX_00,local_20);
      local_10 = (float)fVar4 * param_4;
      FUN_00458fa0(local_18,local_18,&local_14,param_5);
      *local_18 = *local_18 + param_1;
      pfVar1[6] = pfVar1[6] + param_2;
      pfVar1[7] = 0.0;
      pfVar1[8] = DAT_0056c8cc;
      pfVar1[9] = param_8;
      *local_18 = *local_18 + *(float *)((int)this + 0xd0);
      pfVar1[6] = pfVar1[6] + *(float *)((int)this + 0xd4);
      fVar4 = (float10)FUN_00438600(local_20,fVar5 / (float)param_6);
      local_20 = (float)fVar4;
      pvVar3 = (void *)((int)local_24 + 1);
      pfVar1 = local_18;
      local_24 = pvVar3;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x164))(piVar2,0x44);
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x14c))(piVar2,6,param_6,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_6 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

