/* Entry: 0x0043df30; symbol: FUN_0043df30; body bytes: 899 */

undefined4 __thiscall
FUN_0043df30(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,int param_7,float param_8,float param_9)

{
  uint uVar1;
  int *piVar2;
  float *this_00;
  float10 fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float local_1c;
  float local_18;
  float local_14;
  uint local_10;
  float *local_8;
  
  if (((uint)((param_7 * 2 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U)
     && (1 < param_7)) {
    FUN_004455c0((int)this);
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_14 = param_6;
    fVar4 = param_5 / DAT_0056c8d0;
    fVar5 = param_5 / DAT_0056c8d0;
    fVar6 = param_5 / DAT_0056c8d0;
    fVar7 = param_5 / DAT_0056c8d0;
    fVar8 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_10 = 0; (int)local_10 < param_7 + 1; local_10 = local_10 + 1) {
      uVar1 = local_10 & 0x80000001;
      if ((int)uVar1 < 0) {
        uVar1 = (uVar1 - 1 | 0xfffffffe) + 1;
      }
      local_18 = param_4 - fVar6;
      if (uVar1 == 0) {
        local_18 = param_3 - fVar4;
      }
      FUN_00439330(local_8,local_8,local_14,local_18);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      local_8[4] = param_8;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      this_00 = local_8 + 5;
      uVar1 = local_10 & 0x80000001;
      if ((int)uVar1 < 0) {
        uVar1 = (uVar1 - 1 | 0xfffffffe) + 1;
      }
      local_1c = fVar7 + param_4;
      if (uVar1 == 0) {
        local_1c = fVar5 + param_3;
      }
      FUN_00439330(this_00,this_00,local_14,local_1c);
      *this_00 = *this_00 + param_1;
      local_8[6] = local_8[6] + param_2;
      local_8[7] = 0.0;
      local_8[8] = DAT_0056c8cc;
      local_8[9] = param_9;
      *this_00 = *this_00 + *(float *)((int)this + 0xd0);
      local_8[6] = local_8[6] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 10;
      fVar3 = (float10)FUN_00438600(local_14,fVar8 / (float)param_7);
      local_14 = (float)fVar3;
    }
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
  return 0;
}

