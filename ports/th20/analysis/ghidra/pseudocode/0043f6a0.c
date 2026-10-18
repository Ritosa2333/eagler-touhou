/* Entry: 0x0043f6a0; symbol: FUN_0043f6a0; body bytes: 3159 */

void __thiscall FUN_0043f6a0(void *this,int param_1,uint param_2)

{
  code *pcVar1;
  undefined4 uVar2;
  byte bVar3;
  byte bVar4;
  uint uVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  float10 fVar12;
  float fVar13;
  float fVar14;
  undefined4 local_40;
  char local_15;
  undefined4 local_10;
  undefined4 local_c;
  
  uVar5 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  fVar13 = *(float *)((int)this + 200) + *(float *)((int)this + 0xd0);
  fVar14 = *(float *)((int)this + 0xcc) + *(float *)((int)this + 0xd4);
  DAT_005aef10 = DAT_005aef10 + fVar13;
  DAT_005aef14 = DAT_005aef14 + fVar14;
  DAT_005aef2c = DAT_005aef2c + fVar13;
  DAT_005aef30 = DAT_005aef30 + fVar14;
  DAT_005aef48 = DAT_005aef48 + fVar13;
  DAT_005aef4c = DAT_005aef4c + fVar14;
  DAT_005aef64 = DAT_005aef64 + fVar13;
  DAT_005aef68 = DAT_005aef68 + fVar14;
  if ((param_2 & 1) != 0) {
    fVar12 = (float10)FUN_00445b80(DAT_005aef10);
    DAT_005aef10 = (float)fVar12 - DAT_0056e0ec;
    DAT_005aef48 = DAT_005aef10;
    fVar12 = (float10)FUN_00445b80(DAT_005aef14);
    DAT_005aef14 = (float)fVar12 - DAT_0056e0ec;
    DAT_005aef30 = DAT_005aef14;
    fVar12 = (float10)FUN_00445b80(DAT_005aef2c);
    DAT_005aef2c = (float)fVar12 - DAT_0056e0ec;
    DAT_005aef64 = DAT_005aef2c;
    fVar12 = (float10)FUN_00445b80(DAT_005aef4c);
    DAT_005aef4c = (float)fVar12 - DAT_0056e0ec;
    DAT_005aef68 = DAT_005aef4c;
  }
  if ((((((*(float *)(DAT_005c5840 + 0x11c) <= DAT_005aef10) ||
         (*(float *)(DAT_005c5840 + 0x11c) <= DAT_005aef2c)) ||
        (*(float *)(DAT_005c5840 + 0x11c) <= DAT_005aef48)) ||
       (*(float *)(DAT_005c5840 + 0x11c) <= DAT_005aef64)) &&
      (((*(float *)(DAT_005c5840 + 0x120) <= DAT_005aef14 ||
        (*(float *)(DAT_005c5840 + 0x120) <= DAT_005aef30)) ||
       ((*(float *)(DAT_005c5840 + 0x120) <= DAT_005aef4c ||
        (*(float *)(DAT_005c5840 + 0x120) <= DAT_005aef68)))))) &&
     ((((DAT_005aef10 < *(float *)(DAT_005c5840 + 0x124) ||
         DAT_005aef10 == *(float *)(DAT_005c5840 + 0x124) ||
        (DAT_005aef2c < *(float *)(DAT_005c5840 + 0x124) ||
         DAT_005aef2c == *(float *)(DAT_005c5840 + 0x124))) ||
       ((DAT_005aef48 < *(float *)(DAT_005c5840 + 0x124) ||
         DAT_005aef48 == *(float *)(DAT_005c5840 + 0x124) ||
        (DAT_005aef64 < *(float *)(DAT_005c5840 + 0x124) ||
         DAT_005aef64 == *(float *)(DAT_005c5840 + 0x124))))) &&
      ((((DAT_005aef14 < *(float *)(DAT_005c5840 + 0x128) ||
          DAT_005aef14 == *(float *)(DAT_005c5840 + 0x128) ||
         (DAT_005aef30 < *(float *)(DAT_005c5840 + 0x128) ||
          DAT_005aef30 == *(float *)(DAT_005c5840 + 0x128))) ||
        (DAT_005aef4c < *(float *)(DAT_005c5840 + 0x128) ||
         DAT_005aef4c == *(float *)(DAT_005c5840 + 0x128))) ||
       (DAT_005aef68 < *(float *)(DAT_005c5840 + 0x128) ||
        DAT_005aef68 == *(float *)(DAT_005c5840 + 0x128))))))) {
    iVar6 = FUN_00437cd0(param_1);
    if (*(int *)((int)this + 0x6000e08) != *(int *)(iVar6 + 8)) {
      *(undefined4 *)((int)this + 0x6000e08) = *(undefined4 *)(iVar6 + 8);
      FUN_004455c0((int)this);
      piVar7 = (int *)FUN_00412730(0x5c4d40);
      pcVar1 = *(code **)(*piVar7 + 0x104);
      uVar8 = FUN_00445a00(this,*(uint *)((int)this + 0x6000e08));
      (*pcVar1)(piVar7,0,uVar8);
    }
    if (*(char *)((int)this + 0x6000e0e) != '\x01') {
      FUN_004455c0((int)this);
      *(undefined1 *)((int)this + 0x6000e0e) = 1;
    }
    if ((param_2 & 2) == 0) {
      switch(*(byte *)(param_1 + 0x4a1) >> 2 & 7) {
      case 0:
      case 1:
        if ((*(byte *)(param_1 + 0x4a1) >> 2 & 7) == 0) {
          local_40 = *(undefined4 *)(param_1 + 0x490);
        }
        else {
          local_40 = *(undefined4 *)(param_1 + 0x494);
        }
        local_c = local_40;
        if (((*(uint *)(param_1 + 0x49c) >> 0x18 & 1) != 0) && (*(int *)(param_1 + 0x55c) != 0)) {
          uVar8 = *(undefined4 *)(*(int *)(param_1 + 0x55c) + 0x5cc);
          local_10._2_1_ = (byte)((uint)uVar8 >> 0x10);
          local_c._2_1_ = (byte)((uint)local_40 >> 0x10);
          uVar9 = FUN_0043f520(local_c._2_1_,local_10._2_1_);
          local_c._3_1_ = (byte)((uint)local_40 >> 0x18);
          bVar4 = local_c._3_1_;
          local_10._1_1_ = (byte)((uint)uVar8 >> 8);
          local_c._1_1_ = (byte)((uint)local_40 >> 8);
          uVar10 = FUN_0043f520(local_c._1_1_,local_10._1_1_);
          local_c._0_1_ = (byte)local_40;
          local_c._0_2_ = CONCAT11((char)uVar10,(byte)local_c);
          local_c = CONCAT22(CONCAT11(local_c._3_1_,(char)uVar9),(undefined2)local_c);
          local_10._0_1_ = (byte)uVar8;
          uVar9 = FUN_0043f520((byte)local_c,(byte)local_10);
          local_c = CONCAT31(local_c._1_3_,(char)uVar9);
          local_10._3_1_ = (byte)((uint)uVar8 >> 0x18);
          uVar9 = FUN_0043f520(bVar4,local_10._3_1_);
          local_c = CONCAT13((char)uVar9,(undefined3)local_c);
        }
        *(undefined4 *)(param_1 + 0x5cc) = local_c;
        if (*(int *)((int)this + 0x7d40e90) != 0) {
          uVar9 = FUN_0043f520(local_c._2_1_,*(byte *)((int)this + 0x7d40e8e));
          bVar4 = local_c._3_1_;
          uVar10 = FUN_0043f520(local_c._1_1_,*(byte *)((int)this + 0x7d40e8d));
          local_c._0_2_ = CONCAT11((char)uVar10,(byte)local_c);
          local_c = CONCAT22(CONCAT11(local_c._3_1_,(char)uVar9),(undefined2)local_c);
          uVar9 = FUN_0043f520((byte)local_c,*(byte *)((int)this + 0x7d40e8c));
          local_c = CONCAT31(local_c._1_3_,(char)uVar9);
          uVar9 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8f));
          local_c = CONCAT13((char)uVar9,(undefined3)local_c);
        }
        DAT_005aef20 = local_c;
        DAT_005aef3c = local_c;
        DAT_005aef58 = local_c;
        DAT_005aef74 = local_c;
        break;
      case 2:
      case 3:
        uVar8 = *(undefined4 *)(param_1 + 0x490);
        uVar2 = *(undefined4 *)(param_1 + 0x494);
        local_10 = uVar2;
        local_c = uVar8;
        if (*(int *)((int)this + 0x7d40e90) != 0) {
          local_c._2_1_ = (byte)((uint)uVar8 >> 0x10);
          uVar9 = FUN_0043f520(local_c._2_1_,*(byte *)((int)this + 0x7d40e8e));
          local_c._3_1_ = (byte)((uint)uVar8 >> 0x18);
          bVar4 = local_c._3_1_;
          local_c._1_1_ = (byte)((uint)uVar8 >> 8);
          uVar10 = FUN_0043f520(local_c._1_1_,*(byte *)((int)this + 0x7d40e8d));
          local_c._0_1_ = (byte)uVar8;
          local_c._0_2_ = CONCAT11((char)uVar10,(byte)local_c);
          local_c = CONCAT22(CONCAT11(local_c._3_1_,(char)uVar9),(undefined2)local_c);
          uVar9 = FUN_0043f520((byte)local_c,*(byte *)((int)this + 0x7d40e8c));
          local_c = CONCAT31(local_c._1_3_,(char)uVar9);
          uVar9 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8f));
          local_c = CONCAT13((char)uVar9,(undefined3)local_c);
          local_10._2_1_ = (byte)((uint)uVar2 >> 0x10);
          uVar9 = FUN_0043f520(local_10._2_1_,*(byte *)((int)this + 0x7d40e8e));
          local_10._3_1_ = (byte)((uint)uVar2 >> 0x18);
          bVar4 = local_10._3_1_;
          local_10._1_1_ = (byte)((uint)uVar2 >> 8);
          uVar10 = FUN_0043f520(local_10._1_1_,*(byte *)((int)this + 0x7d40e8d));
          local_10._0_1_ = (byte)uVar2;
          local_10 = CONCAT22(CONCAT11(local_10._3_1_,(char)uVar9),
                              CONCAT11((char)uVar10,(byte)local_10));
          uVar9 = FUN_0043f520((byte)local_10,*(byte *)((int)this + 0x7d40e8c));
          local_10 = CONCAT31(local_10._1_3_,(char)uVar9);
          uVar9 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8f));
          local_10 = CONCAT13((char)uVar9,(undefined3)local_10);
        }
        if ((*(byte *)(param_1 + 0x4a1) >> 2 & 7) == 2) {
          DAT_005aef3c = local_10;
          DAT_005aef58 = local_c;
        }
        else {
          DAT_005aef3c = local_c;
          DAT_005aef58 = local_10;
        }
        DAT_005aef74 = local_10;
        DAT_005aef20 = local_c;
        break;
      case 4:
        uVar8 = *(undefined4 *)(param_1 + 0x490);
        uVar2 = *(undefined4 *)(param_1 + 0x494);
        local_10._2_1_ = (byte)((uint)uVar2 >> 0x10);
        local_c._2_1_ = (byte)((uint)uVar8 >> 0x10);
        uVar9 = FUN_0043f4f0(local_c._2_1_,local_10._2_1_);
        local_c._3_1_ = (byte)((uint)uVar8 >> 0x18);
        bVar4 = local_c._3_1_;
        local_10._1_1_ = (byte)((uint)uVar2 >> 8);
        local_c._1_1_ = (byte)((uint)uVar8 >> 8);
        uVar10 = FUN_0043f4f0(local_c._1_1_,local_10._1_1_);
        local_c._0_1_ = (byte)uVar8;
        local_c._0_2_ = CONCAT11((byte)uVar10,(byte)local_c);
        local_c = CONCAT22(CONCAT11(local_c._3_1_,(byte)uVar9),(undefined2)local_c);
        local_10._0_1_ = (byte)uVar2;
        uVar11 = FUN_0043f4f0((byte)local_c,(byte)local_10);
        bVar3 = (byte)uVar11;
        local_c = CONCAT31(local_c._1_3_,bVar3);
        local_10._3_1_ = (byte)((uint)uVar2 >> 0x18);
        uVar11 = FUN_0043f4f0(bVar4,local_10._3_1_);
        bVar4 = (byte)uVar11;
        local_c = CONCAT13(bVar4,(undefined3)local_c);
        if (((*(uint *)(param_1 + 0x49c) >> 0x18 & 1) != 0) && (*(int *)(param_1 + 0x55c) != 0)) {
          uVar8 = *(undefined4 *)(*(int *)(param_1 + 0x55c) + 0x5cc);
          local_10._2_1_ = (byte)((uint)uVar8 >> 0x10);
          uVar9 = FUN_0043f520((byte)uVar9,local_10._2_1_);
          local_10._1_1_ = (byte)((uint)uVar8 >> 8);
          uVar10 = FUN_0043f520((byte)uVar10,local_10._1_1_);
          local_c._0_2_ = CONCAT11((char)uVar10,bVar3);
          local_c = CONCAT22(CONCAT11(bVar4,(char)uVar9),(undefined2)local_c);
          local_10._0_1_ = (byte)uVar8;
          uVar9 = FUN_0043f520(bVar3,(byte)local_10);
          local_c = CONCAT31(local_c._1_3_,(char)uVar9);
          local_10._3_1_ = (byte)((uint)uVar8 >> 0x18);
          uVar9 = FUN_0043f520(bVar4,local_10._3_1_);
          local_c = CONCAT13((char)uVar9,(undefined3)local_c);
        }
        *(undefined4 *)(param_1 + 0x5cc) = local_c;
        if (*(int *)((int)this + 0x7d40e90) != 0) {
          uVar9 = FUN_0043f520(local_c._2_1_,*(byte *)((int)this + 0x7d40e8e));
          bVar4 = local_c._3_1_;
          uVar10 = FUN_0043f520(local_c._1_1_,*(byte *)((int)this + 0x7d40e8d));
          local_c._0_2_ = CONCAT11((char)uVar10,(byte)local_c);
          local_c = CONCAT22(CONCAT11(local_c._3_1_,(char)uVar9),(undefined2)local_c);
          uVar9 = FUN_0043f520((byte)local_c,*(byte *)((int)this + 0x7d40e8c));
          local_c = CONCAT31(local_c._1_3_,(char)uVar9);
          uVar9 = FUN_0043f520(bVar4,*(byte *)((int)this + 0x7d40e8f));
          local_c = CONCAT13((char)uVar9,(undefined3)local_c);
        }
        DAT_005aef20 = local_c;
        DAT_005aef3c = local_c;
        DAT_005aef58 = local_c;
        DAT_005aef74 = local_c;
      }
    }
    FUN_00445ba0(this,param_1);
    if ((*(byte *)(param_1 + 0x4a0) >> 4 & 3) == 3) {
      local_15 = '\x03';
    }
    else {
      local_15 = '\0';
    }
    FUN_00446690(this,local_15);
    DAT_005aef24 = *(float *)(param_1 + 0x378) + *(float *)(param_1 + 0x78);
    DAT_005aef40 = *(float *)(param_1 + 0x398) * *(float *)(param_1 + 0x68) + DAT_005aef24;
    DAT_005aef5c = *(float *)(param_1 + 0x388) + *(float *)(param_1 + 0x78);
    DAT_005aef78 = *(float *)(param_1 + 0x398) * *(float *)(param_1 + 0x68) + DAT_005aef5c;
    DAT_005aef28 = *(float *)(param_1 + 0x37c) + *(float *)(param_1 + 0x7c);
    DAT_005aef60 = *(float *)(param_1 + 0x39c) * *(float *)(param_1 + 0x6c) + DAT_005aef28;
    DAT_005aef44 = *(float *)(param_1 + 900) + *(float *)(param_1 + 0x7c);
    DAT_005aef7c = *(float *)(param_1 + 0x39c) * *(float *)(param_1 + 0x6c) + DAT_005aef44;
    FUN_0043e9b0(this,&DAT_005aef10);
  }
  FUN_005429ee(uVar5 ^ (uint)&stack0xfffffffc);
  return;
}

