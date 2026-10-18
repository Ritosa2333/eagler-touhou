/* Entry: 0x0043dce0; symbol: FUN_0043dce0; body bytes: 583 */

undefined4 __thiscall
FUN_0043dce0(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            int param_6,float param_7)

{
  int *piVar1;
  uint uVar2;
  float10 fVar3;
  float fVar4;
  float local_18;
  float local_14;
  uint local_10;
  float *local_8;
  
  if (((uint)((param_6 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_6)) {
    FUN_004455c0((int)this);
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_14 = param_5;
    fVar4 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_10 = 0; (int)local_10 < param_6 + 1; local_10 = local_10 + 1) {
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
      FUN_00439330((void *)(param_6 + 1),local_8,local_14,local_18);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      local_8[4] = param_7;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 5;
      fVar3 = (float10)FUN_00438600(local_14,fVar4 / (float)param_6);
      local_14 = (float)fVar3;
    }
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))(piVar1,3,param_6,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_6 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

