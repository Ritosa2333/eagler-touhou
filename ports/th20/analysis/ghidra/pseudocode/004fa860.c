/* Entry: 0x004fa860; symbol: FUN_004fa860; body bytes: 1081 */

void FUN_004fa860(int *param_1)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int local_a4 [2];
  undefined1 local_9c [8];
  undefined1 local_94 [8];
  undefined1 local_8c [8];
  undefined1 local_84 [8];
  undefined1 local_7c [8];
  undefined4 local_74;
  undefined4 local_70;
  int *local_6c;
  int *local_68;
  int *local_64;
  void *local_60;
  void *local_5c;
  int local_58;
  int *local_54;
  void *local_50;
  void *local_4c;
  int *local_48;
  undefined4 local_44;
  int local_40;
  int local_3c;
  int local_38;
  int *local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  float local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422e10(&local_1c);
  if (*param_1 != 0) {
    iVar4 = param_1[0x41];
    iVar3 = FUN_00460830(0);
    bVar1 = FUN_004ff5e0(iVar3);
    if ((bool)(char)iVar4 != bVar1) {
      iVar4 = FUN_0041caf0(*(int *)(local_20 + 0x14858));
      iVar4 = FUN_004993b0(iVar4);
      iVar3 = FUN_0041caf0(*(int *)(local_20 + 0x14858));
      iVar3 = FUN_004b8210(iVar3);
      if (iVar3 <= iVar4) {
        FUN_0044fcd0(param_1 + 0x38);
        local_30 = FUN_00421810(local_20);
        local_24 = *(int *)(local_20 + 0x2208) + 0xa8;
        iVar4 = FUN_00464230(0);
        local_28 = FUN_00534130(iVar4);
        local_28 = local_28 * 4;
        local_2c = *(int *)(local_24 + local_28);
        piVar5 = FUN_00450c70(&local_70,(uint *)0x0,local_2c,-1,(undefined4 *)0x0);
        param_1[0x38] = *piVar5;
        local_34 = param_1 + 0x38;
        FUN_00464510(local_34,0);
      }
      FUN_0044fcd0(param_1 + 0x37);
      local_44 = FUN_00421810(local_20);
      local_38 = *(int *)(local_20 + 0x2208) + 0x88;
      iVar4 = FUN_00464230(0);
      local_3c = FUN_00534130(iVar4);
      local_3c = local_3c * 4;
      local_40 = *(int *)(local_38 + local_3c);
      piVar5 = FUN_00450c70(&local_74,(uint *)0x0,local_40,0xe,(undefined4 *)0x0);
      param_1[0x37] = *piVar5;
      local_48 = param_1 + 0x37;
      FUN_00464510(local_48,0);
      iVar4 = FUN_00460830(0);
      bVar1 = FUN_004ff5e0(iVar4);
      *(bool *)(param_1 + 0x41) = bVar1;
    }
    if ((*(uint *)(local_20 + 0x14) >> 1 & 1) == 0) {
      if (((uint)param_1[0x40] >> 1 & 1) == 0) {
        bVar1 = FUN_004ff5e0(local_20);
        if (bVar1) {
          local_50 = (void *)FUN_004ff690(local_20);
          piVar5 = (int *)FUN_004f58f0(local_50,local_84,param_1 + 0x22);
          iVar4 = piVar5[1];
          param_1[0x1c] = *piVar5;
          param_1[0x1d] = iVar4;
          if (param_1[0x47] != 0) {
            (*(code *)param_1[0x47])();
          }
        }
        else {
          local_4c = (void *)FUN_004ff690(local_20);
          piVar5 = (int *)FUN_004f58f0(local_4c,local_7c,param_1 + 0x20);
          iVar4 = piVar5[1];
          param_1[0x1c] = *piVar5;
          param_1[0x1d] = iVar4;
          if (param_1[0x48] != 0) {
            (*(code *)param_1[0x48])();
          }
        }
      }
    }
    else {
      piVar5 = (int *)FUN_004ff690(local_20);
      iVar4 = piVar5[1];
      param_1[0x1c] = *piVar5;
      param_1[0x1d] = iVar4;
      if (0x1d < *(int *)(local_20 + 0x20e8)) {
        *param_1 = 0;
        FUN_00479040(param_1 + 0x37);
        FUN_00479040(param_1 + 0x38);
        goto LAB_004fac88;
      }
    }
    if (param_1[0x3f] == 0) {
      std::_Ptr_base<struct__EXCEPTION_RECORD_const_>::_Ptr_base<struct__EXCEPTION_RECORD_const_>
                ((_Ptr_base<struct__EXCEPTION_RECORD_const_> *)&local_10);
      if (0x1d < *(int *)(local_20 + 0x20e4)) {
        local_54 = param_1 + 0x1c;
        local_5c = FUN_004f5890(local_54,local_8c,param_1 + 0x1e);
        local_58 = *(int *)(local_20 + 0x20e4);
        local_60 = FUN_004f57c0(local_5c,local_94,local_58);
        piVar5 = (int *)FUN_004f5920(local_60,local_9c,100);
        local_10 = *piVar5;
        local_c = piVar5[1];
        cVar2 = FUN_004f5700(&local_10,0);
        if (cVar2 == '\0') {
          param_1[0x1e] = param_1[0x1c];
          param_1[0x1f] = param_1[0x1d];
        }
        else {
          FUN_004f70f0(param_1 + 0x1e,local_a4,&local_10);
        }
      }
    }
    else {
      param_1[0x3f] = param_1[0x3f] + -1;
      param_1[0x1e] = param_1[0x1c];
      param_1[0x1f] = param_1[0x1d];
    }
    local_64 = param_1 + 0x1e;
    FUN_004fee00(local_64,&local_1c);
    local_68 = param_1 + 0x37;
    FUN_004502c0(local_68,&local_1c);
    local_6c = param_1 + 0x38;
    FUN_004502c0(local_6c,&local_1c);
    param_1[4] = (int)local_1c;
    param_1[5] = local_18;
    param_1[6] = local_14;
  }
LAB_004fac88:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

