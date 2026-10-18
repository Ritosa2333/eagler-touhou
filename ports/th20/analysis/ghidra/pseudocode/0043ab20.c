/* Entry: 0x0043ab20; symbol: FUN_0043ab20; body bytes: 1468 */

undefined4 __thiscall
FUN_0043ab20(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            float param_6,float param_7,int param_8,float param_9,float param_10)

{
  int *piVar1;
  float *this_00;
  float10 fVar2;
  float fVar3;
  float fVar4;
  float local_14;
  int local_10;
  float *local_8;
  
  if (((uint)((param_8 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) &&
     (1 < param_8)) {
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_14 = DAT_0056e0f8;
    fVar3 = DAT_0056e0f0 * DAT_0056c8d0;
    FUN_004455c0((int)this);
    fVar4 = param_6 / DAT_0056c8d0;
    for (local_10 = 0; local_10 < param_8 + 1; local_10 = local_10 + 1) {
      FUN_00439330((void *)(param_8 + 1),local_8,local_14,param_5 - fVar4);
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      if ((local_10 < (int)(param_8 + (param_8 >> 0x1f & 3U)) >> 2) || (local_10 == param_8)) {
        *local_8 = *local_8 - param_3 / DAT_0056c8d0;
        local_8[1] = local_8[1] - param_4 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_8 * 2 + (param_8 * 2 >> 0x1f & 3U)) >> 2) {
        *local_8 = param_3 / DAT_0056c8d0 + *local_8;
        local_8[1] = local_8[1] - param_4 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_8 * 3 + (param_8 * 3 >> 0x1f & 3U)) >> 2) {
        *local_8 = param_3 / DAT_0056c8d0 + *local_8;
        local_8[1] = param_4 / DAT_0056c8d0 + local_8[1];
      }
      else if (local_10 < param_8) {
        *local_8 = *local_8 - param_3 / DAT_0056c8d0;
        local_8[1] = param_4 / DAT_0056c8d0 + local_8[1];
      }
      local_8[4] = param_9;
      FUN_00458fa0(local_8,local_8,local_8,param_7);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      this_00 = local_8 + 5;
      FUN_00439330(this_00,this_00,local_14,param_5 + fVar4);
      local_8[7] = 0.0;
      local_8[8] = DAT_0056c8cc;
      if ((local_10 < (int)(param_8 + (param_8 >> 0x1f & 3U)) >> 2) || (local_10 == param_8)) {
        *this_00 = *this_00 - param_3 / DAT_0056c8d0;
        local_8[6] = local_8[6] - param_4 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_8 * 2 + (param_8 * 2 >> 0x1f & 3U)) >> 2) {
        *this_00 = param_3 / DAT_0056c8d0 + *this_00;
        local_8[6] = local_8[6] - param_4 / DAT_0056c8d0;
      }
      else if (local_10 < (int)(param_8 * 3 + (param_8 * 3 >> 0x1f & 3U)) >> 2) {
        *this_00 = param_3 / DAT_0056c8d0 + *this_00;
        local_8[6] = param_4 / DAT_0056c8d0 + local_8[6];
      }
      else if (local_10 < param_8) {
        *this_00 = *this_00 - param_3 / DAT_0056c8d0;
        local_8[6] = param_4 / DAT_0056c8d0 + local_8[6];
      }
      local_8[9] = param_10;
      FUN_00458fa0(this_00,this_00,this_00,param_7);
      *this_00 = *this_00 + param_1;
      local_8[6] = local_8[6] + param_2;
      *this_00 = *this_00 + *(float *)((int)this + 0xd0);
      local_8[6] = local_8[6] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 10;
      fVar2 = (float10)FUN_00438600(local_14,fVar3 / (float)param_8);
      local_14 = (float)fVar2;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))
              (piVar1,5,param_8 << 1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_8 * 2 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

