/* Entry: 0x0043be80; symbol: FUN_0043be80; body bytes: 766 */

undefined4 __thiscall
FUN_0043be80(void *this,float param_1,float param_2,float param_3,float param_4,float param_5,
            int param_6,float param_7,float param_8)

{
  float *this_00;
  int *piVar1;
  void *extraout_ECX;
  void *this_01;
  float10 fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  void *local_14;
  float local_10;
  float *local_8;
  
  if (((uint)((param_6 * 2 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U)
     && (1 < param_6)) {
    FUN_004455c0((int)this);
    local_8 = *(float **)((int)this + 0x7d40e80);
    local_10 = param_5;
    fVar3 = param_4 / DAT_0056c8d0;
    fVar4 = param_4 / DAT_0056c8d0;
    fVar5 = DAT_0056e0f0 * DAT_0056c8d0;
    local_14 = (void *)0x0;
    this_01 = extraout_ECX;
    while ((int)local_14 < param_6 + 1) {
      FUN_00439330(this_01,local_8,local_10,param_3 - fVar3);
      *local_8 = *local_8 + param_1;
      local_8[1] = local_8[1] + param_2;
      local_8[2] = 0.0;
      local_8[3] = DAT_0056c8cc;
      local_8[4] = param_7;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      local_8[1] = local_8[1] + *(float *)((int)this + 0xd4);
      this_00 = local_8 + 5;
      FUN_00439330(this_00,this_00,local_10,fVar4 + param_3);
      *this_00 = *this_00 + param_1;
      local_8[6] = local_8[6] + param_2;
      local_8[7] = 0.0;
      local_8[8] = DAT_0056c8cc;
      local_8[9] = param_8;
      *this_00 = *this_00 + *(float *)((int)this + 0xd0);
      local_8[6] = local_8[6] + *(float *)((int)this + 0xd4);
      local_8 = local_8 + 10;
      fVar2 = (float10)FUN_00438600(local_10,fVar5 / (float)param_6);
      local_10 = (float)fVar2;
      this_01 = (void *)((int)local_14 + 1);
      local_14 = this_01;
    }
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x164))(piVar1,0x44);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x14c))
              (piVar1,5,param_6 << 1,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_6 * 2 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

