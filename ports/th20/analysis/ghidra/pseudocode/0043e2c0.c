/* Entry: 0x0043e2c0; symbol: FUN_0043e2c0; body bytes: 1601 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_0043e2c0(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,float param_7,int param_8,int param_9)

{
  float fVar1;
  float fVar2;
  float *pfVar3;
  uint uVar4;
  int *piVar5;
  void *extraout_ECX;
  void *extraout_ECX_00;
  float10 fVar6;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  
  uVar4 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)((int)this + 0x7d40e80) + 0x3cU < (int)this + 0x7d40e80U) {
    pfVar3 = *(float **)((int)this + 0x7d40e80);
    FUN_004455c0((int)this);
    fVar6 = FUN_00439820(extraout_ECX,param_5);
    fVar1 = (float)fVar6;
    fVar6 = FUN_004397c0(extraout_ECX_00,param_5);
    fVar2 = (float)fVar6;
    local_1c = 0.0;
    local_18 = 0.0;
    local_14 = 0.0;
    local_10 = 0.0;
    local_c = 0.0;
    if (param_8 == 0) {
      local_18 = (float)((uint)param_3 ^ _DAT_0056d7d0) / DAT_0056c8d8;
      local_1c = (param_3 * DAT_0056c8d0) / DAT_0056c8d8;
    }
    else if (param_8 == 1) {
      local_18 = 0.0;
      local_1c = param_3;
    }
    else if (param_8 == 2) {
      local_18 = (float)((uint)param_3 ^ _DAT_0056d7d0);
      local_1c = 0.0;
    }
    if (param_9 == 0) {
      local_14 = (float)((uint)param_4 ^ _DAT_0056d7d0) * DAT_0056e0ec;
      local_10 = 0.0;
      local_c = param_4 * DAT_0056e0ec;
    }
    else if (param_9 == 1) {
      local_14 = 0.0;
      local_10 = param_4 / DAT_0056c8d0;
      local_c = param_4;
    }
    else if (param_9 == 2) {
      local_14 = (float)((uint)param_4 ^ _DAT_0056d7d0);
      local_10 = (float)((uint)param_4 ^ _DAT_0056d7d0) / DAT_0056c8d0;
      local_c = 0.0;
    }
    FUN_00445ad0(pfVar3,local_18,local_14,fVar1,fVar2,param_1,param_2);
    FUN_00445ad0(pfVar3 + 5,local_1c,local_10,fVar1,fVar2,param_1,param_2);
    FUN_00445ad0(pfVar3 + 10,local_18,local_c,fVar1,fVar2,param_1,param_2);
    pfVar3[0xc] = 0.0;
    pfVar3[7] = 0.0;
    pfVar3[2] = 0.0;
    *pfVar3 = *pfVar3 + *(float *)((int)this + 0xd0);
    pfVar3[1] = pfVar3[1] + *(float *)((int)this + 0xd4);
    pfVar3[5] = pfVar3[5] + *(float *)((int)this + 0xd0);
    pfVar3[6] = pfVar3[6] + *(float *)((int)this + 0xd4);
    pfVar3[10] = pfVar3[10] + *(float *)((int)this + 0xd0);
    pfVar3[0xb] = pfVar3[0xb] + *(float *)((int)this + 0xd4);
    pfVar3[0xd] = DAT_0056c8cc;
    pfVar3[8] = DAT_0056c8cc;
    pfVar3[3] = DAT_0056c8cc;
    pfVar3[0xe] = param_6;
    pfVar3[4] = param_6;
    pfVar3[9] = param_7;
    FUN_004ddf80(0x5c4d40);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar5 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar5 + 0x164))(piVar5,0x44);
    piVar5 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar5 + 0x14c))(piVar5,5,1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = *(int *)((int)this + 0x7d40e80) + 0x3c;
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  FUN_005429ee(uVar4 ^ (uint)&stack0xfffffffc);
  return;
}

