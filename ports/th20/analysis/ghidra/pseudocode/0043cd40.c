/* Entry: 0x0043cd40; symbol: FUN_0043cd40; body bytes: 1116 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_0043cd40(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,int param_7)

{
  float *pfVar1;
  uint uVar2;
  int *piVar3;
  void *extraout_ECX;
  void *extraout_ECX_00;
  float10 fVar4;
  float10 fVar5;
  float local_10;
  float local_c;
  
  uVar2 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)((int)this + 0x7d40e80) + 0x28U < (int)this + 0x7d40e80U) {
    pfVar1 = *(float **)((int)this + 0x7d40e80);
    FUN_004455c0((int)this);
    fVar4 = FUN_00439820(extraout_ECX,param_4);
    fVar5 = FUN_004397c0(extraout_ECX_00,param_4);
    local_10 = 0.0;
    local_c = 0.0;
    if (param_7 == 0) {
      local_10 = (float)((uint)param_3 ^ _DAT_0056d7d0) * DAT_0056e0ec;
      local_c = param_3 * DAT_0056e0ec;
    }
    else if (param_7 == 1) {
      local_10 = 0.0;
      local_c = param_3;
    }
    else if (param_7 == 2) {
      local_10 = (float)((uint)param_3 ^ _DAT_0056d7d0);
      local_c = 0.0;
    }
    FUN_00445ad0(pfVar1,local_10,0.0,(float)fVar4,(float)fVar5,param_1,param_2);
    FUN_00445ad0(pfVar1 + 5,local_c,0.0,(float)fVar4,(float)fVar5,param_1,param_2);
    pfVar1[7] = 0.0;
    pfVar1[2] = 0.0;
    *pfVar1 = *pfVar1 + *(float *)((int)this + 0xd0);
    pfVar1[1] = pfVar1[1] + *(float *)((int)this + 0xd4);
    pfVar1[5] = pfVar1[5] + *(float *)((int)this + 0xd0);
    pfVar1[6] = pfVar1[6] + *(float *)((int)this + 0xd4);
    pfVar1[0x17] = DAT_0056c8cc;
    pfVar1[0x12] = DAT_0056c8cc;
    pfVar1[0xd] = DAT_0056c8cc;
    pfVar1[8] = DAT_0056c8cc;
    pfVar1[3] = DAT_0056c8cc;
    pfVar1[4] = param_5;
    pfVar1[9] = param_6;
    FUN_004ddf80(0x5c4d40);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar3 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar3 + 0x164))(piVar3,0x44);
    piVar3 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar3 + 0x14c))(piVar3,3,1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = *(int *)((int)this + 0x7d40e80) + 0x28;
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  FUN_005429ee(uVar2 ^ (uint)&stack0xfffffffc);
  return;
}

