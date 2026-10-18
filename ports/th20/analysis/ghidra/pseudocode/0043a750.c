/* Entry: 0x0043a750; symbol: FUN_0043a750; body bytes: 971 */

undefined4 __thiscall
FUN_0043a750(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,int param_7,float param_8)

{
  int *piVar1;
  float10 fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float local_14;
  int local_10;
  float *local_8;
  
  if (((uint)((param_7 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_7)) {
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_14 = DAT_0056e0f8;
    fVar3 = DAT_0056e0f0 * DAT_0056c8d0;
    FUN_004455c0((int)this);
    fVar4 = param_3 - param_5 * DAT_0056c8d0;
    fVar5 = param_4 - param_5 * DAT_0056c8d0;
    for (local_10 = 0; local_10 < param_7 + 1; local_10 = local_10 + 1) {
      FUN_00439330((void *)(param_7 + 1),local_8,local_14,param_5);
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      if ((local_10 < (int)(param_7 + (param_7 >> 0x1f & 3U)) >> 2) || (local_10 == param_7)) {
        *local_8 = *local_8 - fVar4 / DAT_0056c8d0;
        local_8[1] = local_8[1] - fVar5 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_7 * 2 + (param_7 * 2 >> 0x1f & 3U)) >> 2) {
        *local_8 = fVar4 / DAT_0056c8d0 + *local_8;
        local_8[1] = local_8[1] - fVar5 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_7 * 3 + (param_7 * 3 >> 0x1f & 3U)) >> 2) {
        *local_8 = fVar4 / DAT_0056c8d0 + *local_8;
        local_8[1] = fVar5 / DAT_0056c8d0 + local_8[1];
      }
      else if (local_10 < param_7) {
        *local_8 = *local_8 - fVar4 / DAT_0056c8d0;
        local_8[1] = fVar5 / DAT_0056c8d0 + local_8[1];
      }
      local_8[4] = param_8;
      FUN_00458fa0(local_8,local_8,local_8,param_6);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 5;
      fVar2 = (float10)FUN_00438600(local_14,fVar3 / (float)param_7);
      local_14 = (float)fVar2;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))(piVar1,3,param_7,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_7 + 1) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

