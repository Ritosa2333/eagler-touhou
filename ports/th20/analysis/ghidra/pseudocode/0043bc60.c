/* Entry: 0x0043bc60; symbol: FUN_0043bc60; body bytes: 541 */

undefined4 __thiscall
FUN_0043bc60(void *this,float param_1,float param_2,float param_3,float param_4,int param_5,
            float param_6)

{
  int *piVar1;
  float10 fVar2;
  float fVar3;
  float local_14;
  int local_10;
  float *local_8;
  
  if (((uint)((param_5 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_5)) {
    FUN_004455c0((int)this);
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_14 = param_4;
    fVar3 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_10 = 0; local_10 < param_5 + 1; local_10 = local_10 + 1) {
      FUN_00439330((void *)(param_5 + 1),local_8,local_14,param_3);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      local_8[4] = param_6;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 5;
      fVar2 = (float10)FUN_00438600(local_14,fVar3 / (float)param_5);
      local_14 = (float)fVar2;
    }
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))(piVar1,3,param_5,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_5 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

