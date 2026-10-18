/* Entry: 0x0043c780; symbol: FUN_0043c780; body bytes: 1001 */

void __thiscall
FUN_0043c780(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,int param_7,float param_8,float param_9)

{
  float *pfVar1;
  int *piVar2;
  void *extraout_ECX;
  void *this_00;
  void *extraout_ECX_00;
  float10 fVar3;
  float fVar4;
  float fVar5;
  int local_24;
  float local_20;
  float *local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((uint)((param_7 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_7)) {
    local_18 = *(float **)((int)this + 0x7d40e80);
    local_20 = param_6;
    FUN_004455c0((int)this);
    FUN_00422e10(&local_14);
    fVar4 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_24 = 0; local_24 < param_7 + 1; local_24 = local_24 + 1) {
      fVar5 = param_5 / DAT_0056c8d0;
      fVar3 = FUN_004397c0((void *)(param_7 + 1),local_20);
      local_14 = (param_3 - fVar5) * (float)fVar3;
      fVar5 = param_5 / DAT_0056c8d0;
      fVar3 = FUN_00439820(extraout_ECX,local_20);
      local_10 = (param_4 - fVar5) * (float)fVar3;
      FUN_00458fa0(this_00,local_18,&local_14,param_6);
      *local_18 = *local_18 + param_1;
      local_18[1] = local_18[1] + param_2;
      local_18[2] = 0.0;
      local_18[3] = DAT_0056c8cc;
      local_18[4] = param_8;
      *local_18 = *local_18 + *(float *)((int)this + 0xd0);
      local_18[1] = local_18[1] + *(float *)((int)this + 0xd4);
      pfVar1 = local_18 + 5;
      fVar5 = param_5 / DAT_0056c8d0;
      fVar3 = FUN_004397c0(this,local_20);
      local_14 = (fVar5 + param_3) * (float)fVar3;
      fVar5 = param_5 / DAT_0056c8d0;
      fVar3 = FUN_00439820(extraout_ECX_00,local_20);
      local_10 = (fVar5 + param_4) * (float)fVar3;
      FUN_00458fa0(&local_14,pfVar1,&local_14,param_6);
      *pfVar1 = *pfVar1 + param_1;
      local_18[6] = local_18[6] + param_2;
      local_18[7] = 0.0;
      local_18[8] = DAT_0056c8cc;
      local_18[9] = param_9;
      *pfVar1 = *pfVar1 + *(float *)((int)this + 0xd0);
      local_18[6] = local_18[6] + *(float *)((int)this + 0xd4);
      local_18 = local_18 + 10;
      fVar3 = (float10)FUN_00438600(local_20,fVar4 / (float)param_7);
      local_20 = (float)fVar3;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x164))(piVar2,0x44);
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x14c))
              (piVar2,5,param_7 << 1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_7 * 2 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

