/* Entry: 0x004f8ce0; symbol: FUN_004f8ce0; body bytes: 777 */

void FUN_004f8ce0(float *param_1,float *param_2,int param_3)

{
  bool bVar1;
  float *pfVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined1 local_78 [12];
  undefined1 local_6c [12];
  undefined1 local_60 [12];
  undefined1 local_54 [12];
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  int local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004398a0(&local_18);
  FUN_004398a0(&local_10);
  FUN_00422e10(&local_24);
  FUN_00422e10(&local_30);
  bVar1 = FUN_004ff5e0(local_34);
  if (bVar1) {
    local_38 = (void *)FUN_00460850(local_34);
    pfVar2 = (float *)FUN_00429440(local_38,local_54,(float *)(local_34 + 0x20a8));
    local_24 = *pfVar2;
    local_20 = pfVar2[1];
    local_1c = pfVar2[2];
    local_3c = (void *)FUN_00460850(local_34);
    pfVar2 = (float *)FUN_00429570(local_3c,local_60,(float *)(local_34 + 0x20a8));
    local_30 = *pfVar2;
    local_2c = pfVar2[1];
    local_28 = pfVar2[2];
  }
  else {
    local_40 = (void *)FUN_00460850(local_34);
    pfVar2 = (float *)FUN_00429440(local_40,local_6c,(float *)(local_34 + 0x209c));
    local_24 = *pfVar2;
    local_20 = pfVar2[1];
    local_1c = pfVar2[2];
    local_44 = (void *)FUN_00460850(local_34);
    pfVar2 = (float *)FUN_00429570(local_44,local_78,(float *)(local_34 + 0x209c));
    local_30 = *pfVar2;
    local_2c = pfVar2[1];
    local_28 = pfVar2[2];
  }
  local_18 = *param_1 - *param_2 / DAT_0056c8d0;
  local_14 = param_1[1] - param_2[1] / DAT_0056c8d0;
  local_10 = *param_2 / DAT_0056c8d0 + *param_1;
  local_c = param_2[1] / DAT_0056c8d0 + param_1[1];
  if ((((local_10 < local_24) || (local_c < local_20)) || (local_30 < local_18)) ||
     (local_2c < local_14)) {
    local_18 = *param_1 - DAT_0056fb7c / DAT_0056c8d0;
    local_14 = param_1[1] - DAT_0056fb7c / DAT_0056c8d0;
    local_10 = DAT_0056fb7c / DAT_0056c8d0 + *param_1;
    local_c = DAT_0056fb7c / DAT_0056c8d0 + param_1[1];
  }
  else if ((((DAT_005c06a4 == 0) ||
            (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0)) &&
           ((param_3 == 0 &&
            ((iVar3 = FUN_00411700(local_34), iVar3 != 2 &&
             (iVar3 = FUN_00411700(local_34), iVar3 != 4)))))) &&
          (iVar3 = FUN_00411700(local_34), iVar3 != 3)) {
    local_48 = (void *)(local_34 + 0x2050);
    bVar1 = FUN_00461070(local_48,0);
    if (!bVar1) {
      FUN_004f86f0();
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

