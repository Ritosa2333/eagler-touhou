/* Entry: 0x0043c4e0; symbol: FUN_0043c4e0; body bytes: 660 */

void __thiscall
FUN_0043c4e0(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            int param_6,float param_7)

{
  int *piVar1;
  void *extraout_ECX;
  void *this_00;
  float10 fVar2;
  float fVar3;
  int local_24;
  float local_20;
  float *local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((uint)((param_6 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_6)) {
    local_18 = *(float **)((int)this + 0x7d40e80);
    local_20 = param_5;
    FUN_004455c0((int)this);
    FUN_00422e10(&local_14);
    fVar3 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_24 = 0; local_24 < param_6 + 1; local_24 = local_24 + 1) {
      fVar2 = FUN_004397c0((void *)(param_6 + 1),local_20);
      local_14 = (float)fVar2 * param_3;
      fVar2 = FUN_00439820(extraout_ECX,local_20);
      local_10 = (float)fVar2 * param_4;
      FUN_00458fa0(this_00,local_18,&local_14,param_5);
      *local_18 = *local_18 + param_1;
      local_18[1] = local_18[1] + param_2;
      local_18[2] = 0.0;
      local_18[3] = DAT_0056c8cc;
      local_18[4] = param_7;
      *local_18 = *local_18 + *(float *)((int)this + 0xd0);
      local_18[1] = local_18[1] + *(float *)((int)this + 0xd4);
      local_18 = local_18 + 5;
      fVar2 = (float10)FUN_00438600(local_20,fVar3 / (float)param_6);
      local_20 = (float)fVar2;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))(piVar1,3,param_6,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_6 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

