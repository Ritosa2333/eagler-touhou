/* Entry: 0x004408b0; symbol: FUN_004408b0; body bytes: 2769 */

void FUN_004408b0(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  float *pfVar6;
  uint uVar7;
  uint uVar8;
  float10 fVar9;
  undefined1 local_68 [12];
  undefined1 local_5c [12];
  undefined1 local_50 [12];
  void *local_44;
  float *local_40;
  void *local_3c;
  void *local_38;
  uint local_34;
  uint local_30;
  float local_2c;
  float local_28;
  void *local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar5 = FUN_00440340(param_1);
  if (iVar5 == 0) {
    FUN_00422e10(&local_1c);
    local_2c = DAT_005c5840[0x54] - DAT_005c5840[0x55];
    local_38 = (void *)(param_1 + 0x2c);
    local_3c = FUN_00429570(local_38,local_50,(float *)(param_1 + 0x5bc));
    local_44 = FUN_00429570(local_3c,local_5c,(float *)(param_1 + 0x484));
    local_40 = DAT_005c5840;
    pfVar6 = (float *)FUN_00429440(local_44,local_68,DAT_005c5840);
    local_1c = *pfVar6;
    local_18 = pfVar6[1];
    local_14 = pfVar6[2];
    if (((*(byte *)(param_1 + 0x4a3) & 3) != 0) && (*(int *)(param_1 + 0x55c) == 0)) {
      local_1c = (float)DAT_005b87f8 / DAT_0056c8d0 + local_1c;
      local_18 = ((float)DAT_005b87fc - DAT_0056d7c0) / DAT_0056c8d0 + local_18;
    }
    fVar9 = (float10)FUN_00445700(&local_1c);
    local_20 = (float)fVar9;
    local_30 = (uint)(*(byte *)(param_1 + 0x4a1) >> 2 & 7);
    switch(local_30) {
    case 0:
    case 1:
      if ((*(byte *)(param_1 + 0x4a1) >> 2 & 7) == 0) {
        local_34 = *(uint *)(param_1 + 0x490);
      }
      else {
        local_34 = *(uint *)(param_1 + 0x494);
      }
      local_c = local_34;
      uVar7 = local_c;
      if (*(int *)((int)local_24 + 0x7d40e90) != 0) {
        local_c._2_1_ = (byte)(local_34 >> 0x10);
        bVar3 = local_c._2_1_;
        local_c = uVar7;
        uVar8 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8e));
        uVar7 = local_c;
        local_c._0_3_ = CONCAT12((char)uVar8,(undefined2)local_c);
        uVar1 = local_c;
        local_c._1_1_ = SUB41(uVar7,1);
        bVar3 = local_c._1_1_;
        local_c = uVar1;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8d));
        local_c._0_2_ = CONCAT11((char)uVar7,(byte)local_c);
        uVar7 = FUN_0043f520((byte)local_c,*(byte *)((int)local_24 + 0x7d40e8c));
        uVar1 = local_c;
        local_c = CONCAT31(local_c._1_3_,(char)uVar7);
        uVar2 = local_c;
        local_c._3_1_ = SUB41(uVar1,3);
        bVar3 = local_c._3_1_;
        local_c = uVar2;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8f));
        local_c = CONCAT13((char)uVar7,(undefined3)local_c);
      }
      if (local_20 < DAT_005c5840[0x54] || local_20 == DAT_005c5840[0x54]) {
        DAT_005aef20 = local_c;
      }
      else {
        local_28 = (DAT_005c5840[0x54] - local_20) / local_2c;
        if (DAT_0056c8cc <= local_28) goto LAB_00441370;
        DAT_005aef20._0_2_ =
             CONCAT11(local_c._1_1_ -
                      (char)(int)((float)(int)((uint)local_c._1_1_ - (int)DAT_005c5840[0x57]) *
                                 local_28),
                      (byte)local_c -
                      (char)(int)((float)(int)((local_c & 0xff) - (int)DAT_005c5840[0x56]) *
                                 local_28));
        DAT_005aef20._0_3_ =
             CONCAT12(local_c._2_1_ -
                      (char)(int)((float)(int)((uint)local_c._2_1_ - (int)DAT_005c5840[0x58]) *
                                 local_28),(undefined2)DAT_005aef20);
        DAT_005aef20 = CONCAT13((char)(int)((float)(local_c >> 0x18) *
                                           (DAT_0056c8cc - local_28 * local_28 * local_28)),
                                (undefined3)DAT_005aef20);
      }
      if ((*(uint *)(param_1 + 0x49c) >> 0x14 & 1) != 0) {
        if (local_20 <= *(float *)(param_1 + 0x4bc)) goto LAB_00441370;
        if (local_20 < *(float *)(param_1 + 0x4b8)) {
          local_28 = (*(float *)(param_1 + 0x4b8) - local_20) /
                     (*(float *)(param_1 + 0x4b8) - *(float *)(param_1 + 0x4bc));
          DAT_005aef20 = CONCAT13((char)(int)((float)(DAT_005aef20 >> 0x18) *
                                             (DAT_0056c8cc - local_28)),(undefined3)DAT_005aef20);
        }
      }
      DAT_005aef3c = DAT_005aef20;
      DAT_005aef58 = DAT_005aef20;
      DAT_005aef74 = DAT_005aef20;
      break;
    case 2:
    case 3:
      local_c = *(uint *)(param_1 + 0x490);
      local_10 = *(uint *)(param_1 + 0x494);
      if (*(int *)((int)local_24 + 0x7d40e90) != 0) {
        local_c._2_1_ = (byte)(local_c >> 0x10);
        bVar3 = local_c._2_1_;
        uVar8 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8e));
        uVar7 = local_c;
        local_c._0_3_ = CONCAT12((char)uVar8,(undefined2)local_c);
        uVar1 = local_c;
        local_c._1_1_ = SUB41(uVar7,1);
        bVar3 = local_c._1_1_;
        local_c = uVar1;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8d));
        local_c._0_2_ = CONCAT11((char)uVar7,(byte)local_c);
        uVar7 = FUN_0043f520((byte)local_c,*(byte *)((int)local_24 + 0x7d40e8c));
        uVar1 = local_c;
        local_c = CONCAT31(local_c._1_3_,(char)uVar7);
        uVar2 = local_c;
        local_c._3_1_ = SUB41(uVar1,3);
        bVar3 = local_c._3_1_;
        local_c = uVar2;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8f));
        local_c = CONCAT13((char)uVar7,(undefined3)local_c);
        uVar8 = FUN_0043f520(local_10._2_1_,*(byte *)((int)local_24 + 0x7d40e8e));
        uVar7 = local_10;
        local_10._0_3_ = CONCAT12((char)uVar8,(undefined2)local_10);
        uVar1 = local_10;
        local_10._1_1_ = SUB41(uVar7,1);
        bVar3 = local_10._1_1_;
        local_10 = uVar1;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8d));
        local_10._0_2_ = CONCAT11((char)uVar7,(byte)local_10);
        uVar7 = FUN_0043f520((byte)local_10,*(byte *)((int)local_24 + 0x7d40e8c));
        uVar1 = local_10;
        local_10 = CONCAT31(local_10._1_3_,(char)uVar7);
        uVar2 = local_10;
        local_10._3_1_ = SUB41(uVar1,3);
        bVar3 = local_10._3_1_;
        local_10 = uVar2;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8f));
        local_10 = CONCAT13((char)uVar7,(undefined3)local_10);
      }
      if (local_20 < DAT_005c5840[0x54] || local_20 == DAT_005c5840[0x54]) {
        DAT_005aef20 = local_c;
        DAT_005aef74 = local_10;
      }
      else {
        local_20 = (DAT_005c5840[0x54] - local_20) / local_2c;
        if (DAT_0056c8cc <= local_20) goto LAB_00441370;
        DAT_005aef20._0_2_ =
             CONCAT11(local_c._1_1_ -
                      (char)(int)((float)(int)((uint)local_c._1_1_ - (int)DAT_005c5840[0x57]) *
                                 local_20),
                      (byte)local_c -
                      (char)(int)((float)(int)((local_c & 0xff) - (int)DAT_005c5840[0x56]) *
                                 local_20));
        DAT_005aef20._0_3_ =
             CONCAT12(local_c._2_1_ -
                      (char)(int)((float)(int)((uint)local_c._2_1_ - (int)DAT_005c5840[0x58]) *
                                 local_20),(undefined2)DAT_005aef20);
        DAT_005aef20 = CONCAT13((char)(int)((float)(local_c >> 0x18) * (DAT_0056c8cc - local_20)),
                                (undefined3)DAT_005aef20);
        DAT_005aef74 = CONCAT13((char)(int)((float)(local_10 >> 0x18) * (DAT_0056c8cc - local_20)),
                                CONCAT12(local_10._2_1_ -
                                         (char)(int)((float)(int)((uint)local_10._2_1_ -
                                                                 (int)DAT_005c5840[0x58]) * local_20
                                                    ),
                                         CONCAT11(local_10._1_1_ -
                                                  (char)(int)((float)(int)((uint)local_10._1_1_ -
                                                                          (int)DAT_005c5840[0x57]) *
                                                             local_20),
                                                  (byte)local_10 -
                                                  (char)(int)((float)(int)((local_10 & 0xff) -
                                                                          (int)DAT_005c5840[0x56]) *
                                                             local_20))));
      }
      if ((*(byte *)(param_1 + 0x4a1) >> 2 & 7) == 2) {
        DAT_005aef58 = DAT_005aef20;
        DAT_005aef3c = DAT_005aef74;
      }
      else {
        DAT_005aef3c = DAT_005aef20;
        DAT_005aef58 = DAT_005aef74;
      }
      break;
    case 4:
      local_10._2_1_ = (byte)(*(uint *)(param_1 + 0x494) >> 0x10);
      bVar3 = local_10._2_1_;
      local_c._2_1_ = (byte)((uint)*(undefined4 *)(param_1 + 0x490) >> 0x10);
      bVar4 = local_c._2_1_;
      local_10 = *(uint *)(param_1 + 0x494);
      local_c = *(undefined4 *)(param_1 + 0x490);
      uVar7 = FUN_0043f4f0(bVar4,bVar3);
      uVar1 = local_c;
      local_c._0_3_ = CONCAT12((char)uVar7,(undefined2)local_c);
      uVar2 = local_c;
      local_c._1_1_ = SUB41(uVar1,1);
      bVar3 = local_c._1_1_;
      local_c = uVar2;
      uVar7 = FUN_0043f4f0(bVar3,local_10._1_1_);
      local_c._0_2_ = CONCAT11((char)uVar7,(byte)local_c);
      uVar7 = FUN_0043f4f0((byte)local_c,(byte)local_10);
      uVar1 = local_c;
      local_c = CONCAT31(local_c._1_3_,(char)uVar7);
      uVar2 = local_c;
      local_c._3_1_ = SUB41(uVar1,3);
      bVar3 = local_c._3_1_;
      local_c = uVar2;
      uVar7 = FUN_0043f4f0(bVar3,local_10._3_1_);
      uVar1 = local_c;
      local_c = CONCAT13((char)uVar7,(undefined3)local_c);
      uVar7 = local_c;
      if (*(int *)((int)local_24 + 0x7d40e90) != 0) {
        local_c._2_1_ = SUB41(uVar1,2);
        bVar3 = local_c._2_1_;
        local_c = uVar7;
        uVar8 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8e));
        uVar7 = local_c;
        local_c._0_3_ = CONCAT12((char)uVar8,(undefined2)local_c);
        uVar1 = local_c;
        local_c._1_1_ = SUB41(uVar7,1);
        bVar3 = local_c._1_1_;
        local_c = uVar1;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8d));
        local_c._0_2_ = CONCAT11((char)uVar7,(byte)local_c);
        uVar7 = FUN_0043f520((byte)local_c,*(byte *)((int)local_24 + 0x7d40e8c));
        uVar1 = local_c;
        local_c = CONCAT31(local_c._1_3_,(char)uVar7);
        uVar2 = local_c;
        local_c._3_1_ = SUB41(uVar1,3);
        bVar3 = local_c._3_1_;
        local_c = uVar2;
        uVar7 = FUN_0043f520(bVar3,*(byte *)((int)local_24 + 0x7d40e8f));
        local_c = CONCAT13((char)uVar7,(undefined3)local_c);
      }
      if (local_20 < DAT_005c5840[0x54] || local_20 == DAT_005c5840[0x54]) {
        DAT_005aef20 = local_c;
      }
      else {
        local_20 = (DAT_005c5840[0x54] - local_20) / local_2c;
        if (DAT_0056c8cc <= local_20) goto LAB_00441370;
        DAT_005aef20._0_2_ =
             CONCAT11(local_c._1_1_ -
                      (char)(int)((float)(int)((uint)local_c._1_1_ - (int)DAT_005c5840[0x57]) *
                                 local_20),
                      (byte)local_c -
                      (char)(int)((float)(int)((local_c & 0xff) - (int)DAT_005c5840[0x56]) *
                                 local_20));
        DAT_005aef20._0_3_ =
             CONCAT12(local_c._2_1_ -
                      (char)(int)((float)(int)((uint)local_c._2_1_ - (int)DAT_005c5840[0x58]) *
                                 local_20),(undefined2)DAT_005aef20);
        DAT_005aef20 = CONCAT13((char)(int)((float)(local_c >> 0x18) *
                                           (DAT_0056c8cc - local_20 * local_20 * local_20)),
                                (undefined3)DAT_005aef20);
      }
      DAT_005aef3c = DAT_005aef20;
      DAT_005aef58 = DAT_005aef20;
      DAT_005aef74 = DAT_005aef20;
    }
    FUN_0043f6a0(local_24,param_1,2);
  }
LAB_00441370:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

