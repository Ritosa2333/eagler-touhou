/* Entry: 0x0043b960; symbol: FUN_0043b960; body bytes: 755 */

undefined4 __thiscall
FUN_0043b960(void *this,float param_1,float param_2,float param_3,float param_4,int param_5,
            float param_6,float param_7)

{
  float *pfVar1;
  int *piVar2;
  float10 fVar3;
  float fVar4;
  float local_14;
  void *local_10;
  float *local_8;
  
  if ((uint)((param_5 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80)) < (int)this + 0x7d40e80U) {
    pfVar1 = *(float **)((int)this + 0x7d40e80);
    local_14 = param_4;
    FUN_004455c0((int)this);
    *pfVar1 = param_1;
    pfVar1[1] = param_2;
    pfVar1[2] = 0.0;
    pfVar1[3] = DAT_0056c8cc;
    pfVar1[4] = param_6;
    *pfVar1 = *pfVar1 + *(float *)((int)this + 0xd0);
    pfVar1[1] = pfVar1[1] + *(float *)((int)this + 0xd4);
    fVar4 = DAT_0056e0f0 * DAT_0056c8d0;
    for (local_10 = (void *)0x0; local_8 = pfVar1 + 5, (int)local_10 < param_5 + 1;
        local_10 = (void *)((int)local_10 + 1)) {
      FUN_00439330(local_10,local_8,local_14,param_3);
      *local_8 = *local_8 + param_1;
      pfVar1[6] = pfVar1[6] + param_2;
      pfVar1[7] = 0.0;
      pfVar1[8] = DAT_0056c8cc;
      pfVar1[9] = param_7;
      *local_8 = *local_8 + *(float *)((int)this + 0xd0);
      pfVar1[6] = pfVar1[6] + *(float *)((int)this + 0xd4);
      fVar3 = (float10)FUN_00438600(local_14,fVar4 / (float)param_5);
      local_14 = (float)fVar3;
      pfVar1 = local_8;
    }
    FUN_004ddf80(0x5c4d40);
    FUN_004d9db0(&DAT_005c4d40,0x17,8);
    *(undefined1 *)((int)this + 0x6000e0e) = 1;
    FUN_00446690(this,'\x02');
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x164))(piVar2,0x44);
    piVar2 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar2 + 0x14c))(piVar2,6,param_5,*(undefined4 *)((int)this + 0x7d40e80),0x14);
    *(int *)((int)this + 0x7d40e80) = (param_5 + 2) * 0x14 + *(int *)((int)this + 0x7d40e80);
    *(int *)((int)this + 0xc4) = *(int *)((int)this + 0xc4) + 1;
  }
  return 0;
}

