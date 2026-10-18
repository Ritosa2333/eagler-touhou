/* Entry: 0x004d3230; symbol: FUN_004d3230; body bytes: 2665 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004d3230(void *this,undefined4 *param_1)

{
  int iVar1;
  undefined *puVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  float10 fVar6;
  undefined1 local_e8 [92];
  void *local_8c;
  undefined4 *local_88;
  float *local_84;
  float *local_80;
  void *local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  void *local_64;
  void *local_60;
  int local_5c;
  void *local_58;
  int local_54;
  undefined *local_50;
  int local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  int local_38;
  void *local_34;
  int local_30;
  int local_2c;
  int *local_28;
  int *local_24;
  int local_20;
  int local_1c;
  void *local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = this;
  FUN_004c8d70((void *)((int)this + 0x6f8),local_e8,param_1);
  FUN_0048b8b0((int)local_e8);
  *(undefined4 *)((int)local_18 + 0x1320) = *(undefined4 *)((int)local_18 + 0x748);
  *(undefined4 *)((int)local_18 + 0x20) = 2;
  *(undefined4 *)((int)local_18 + 0x24) = 2;
  *(undefined4 *)((int)local_18 + 0x6e4) = *(undefined4 *)((int)local_18 + 0x710);
  *(undefined4 *)((int)local_18 + 0x6e8) = *(undefined4 *)((int)local_18 + 0x714);
  FUN_004d7dd0(local_18,*(undefined4 *)((int)local_18 + 0x750));
  FUN_004299d0((int)local_18 + 0x754);
  local_2c = *(int *)((int)local_18 + 0x6e4);
  if (local_2c == 1) {
    iVar1 = FUN_0041ca90(*(int *)((int)local_18 + 0x6f0));
    local_40 = *(void **)(iVar1 + 0x48);
    FUN_004383a0(local_40,(void *)((int)local_18 + 0x754),0x147);
    *(undefined4 *)((int)local_18 + 0x131c) = DAT_0056f798;
  }
  else if (local_2c == 2) {
    local_44 = (void *)FUN_00412730(*(int *)((int)local_18 + 0x6f0));
    local_48 = (void *)FUN_004aae10(local_44,3);
    FUN_004383a0(local_48,(void *)((int)local_18 + 0x754),0xb);
    *(undefined4 *)((int)local_18 + 0x131c) = DAT_00571784;
  }
  else {
    local_34 = (void *)((int)local_18 + 0x754);
    FUN_00438a30(local_34,FUN_004cf5c0);
    *(void **)((int)local_18 + 0xd1c) = local_18;
    iVar1 = FUN_0041ca90(*(int *)((int)local_18 + 0x6f0));
    local_3c = *(void **)(iVar1 + 0x48);
    local_38 = *(int *)((int)local_18 + 0x6e4) + 0x91;
    FUN_004383a0(local_3c,(void *)((int)local_18 + 0x754),local_38);
    *(undefined4 *)((int)local_18 + 0x131c) = DAT_0056f798;
  }
  *(undefined4 *)((int)local_18 + 0x6d0) = 0xffd08080;
  puVar2 = FUN_004856e0(*(int *)((int)local_18 + 0x6e4));
  local_4c = *(int *)(puVar2 + 0x14c);
  if (local_4c == 6) {
    local_50 = FUN_004856e0(*(int *)((int)local_18 + 0x6e4));
    local_50 = local_50 + 4;
    local_54 = *(int *)((int)local_18 + 0x6e8) * 0x14;
    *(undefined4 *)((int)local_18 + 0x6d0) = *(undefined4 *)(local_50 + local_54 + 0x10);
  }
  local_58 = (void *)((int)local_18 + 0x754);
  FUN_004388f0(local_58,2);
  FUN_0042b5d0((void *)((int)local_18 + 0x754));
  FUN_0045e5b0((int)local_18 + 0x754);
  *(undefined1 *)((int)local_18 + 0xbec) = 1;
  *(undefined4 *)((int)local_18 + 0xbfc) = 0;
  *(undefined4 *)((int)local_18 + 0xc00) = 2;
  *(byte *)((int)local_18 + 0xbf7) = *(byte *)((int)local_18 + 0xbf7) & 0xfc | 1;
  *(undefined4 *)((int)local_18 + 0x1324) = *(undefined4 *)((int)local_18 + 0x74c);
  iVar1 = FUN_0041ca90(*(int *)((int)local_18 + 0x6f0));
  local_60 = *(void **)(iVar1 + 0x48);
  local_5c = *(int *)((int)local_18 + 0x714) + 0x3a;
  FUN_00438380(local_60,(void *)((int)local_18 + 0xd38),local_5c);
  local_64 = (void *)((int)local_18 + 0xd38);
  FUN_004388f0(local_64,2);
  FUN_0042b5d0((void *)((int)local_18 + 0xd38));
  FUN_0045e5b0((int)local_18 + 0xd38);
  *(undefined1 *)((int)local_18 + 0x11d0) = 1;
  *(byte *)((int)local_18 + 0x11db) = *(byte *)((int)local_18 + 0x11db) & 0xfc | 1;
  uVar3 = FUN_0041f610(*(int *)((int)local_18 + 0x718) * 0x38);
  *(undefined4 *)((int)local_18 + 0x132c) = uVar3;
  if (*(int *)((int)local_18 + 0x132c) != 0) {
    uVar3 = FUN_0041f610(*(int *)((int)local_18 + 0x718) << 5);
    *(undefined4 *)((int)local_18 + 0x1328) = uVar3;
    if (*(int *)((int)local_18 + 0x1328) != 0) {
      _memset(*(void **)((int)local_18 + 0x132c),0,*(int *)((int)local_18 + 0x718) * 0x38);
      _memset(*(void **)((int)local_18 + 0x1328),0,*(int *)((int)local_18 + 0x718) << 5);
      *(undefined4 *)((int)local_18 + 0x58) = *(undefined4 *)((int)local_18 + 0x6f8);
      *(undefined4 *)((int)local_18 + 0x5c) = *(undefined4 *)((int)local_18 + 0x6fc);
      *(undefined4 *)((int)local_18 + 0x60) = *(undefined4 *)((int)local_18 + 0x700);
      if (*(float *)((int)local_18 + 0x71c) != _DAT_0056e0e8) {
        FUN_00422e10(&local_14);
        FUN_00439330(&local_14,&local_14,*(float *)((int)local_18 + 0x704),
                     *(float *)((int)local_18 + 0x71c));
        *(float *)((int)local_18 + 0x58) = *(float *)((int)local_18 + 0x58) + local_14;
        *(float *)((int)local_18 + 0x5c) = *(float *)((int)local_18 + 0x5c) + local_10;
        *(undefined4 *)((int)local_18 + 0x6f8) = *(undefined4 *)((int)local_18 + 0x58);
        *(undefined4 *)((int)local_18 + 0x6fc) = *(undefined4 *)((int)local_18 + 0x5c);
        *(undefined4 *)((int)local_18 + 0x700) = *(undefined4 *)((int)local_18 + 0x60);
        *(undefined4 *)((int)local_18 + 0x71c) = 0;
      }
      *(undefined4 *)((int)local_18 + 0x78) = *(undefined4 *)((int)local_18 + 0x708);
      *(undefined4 *)((int)local_18 + 0x70) = *(undefined4 *)((int)local_18 + 0x704);
      *(undefined4 *)((int)local_18 + 0x7c) = *(undefined4 *)((int)local_18 + 0x70c);
      FUN_00439330((void *)((int)local_18 + 100),(float *)((int)local_18 + 100),
                   *(float *)((int)local_18 + 0x70),*(float *)((int)local_18 + 0x7c));
      *(undefined4 *)((int)local_18 + 0x6c) = 0;
      for (local_20 = 0; local_20 < *(int *)((int)local_18 + 0x718); local_20 = local_20 + 1) {
        puVar4 = (undefined4 *)(*(int *)((int)local_18 + 0x1328) + local_20 * 0x20);
        *puVar4 = *(undefined4 *)((int)local_18 + 0x58);
        puVar4[1] = *(undefined4 *)((int)local_18 + 0x5c);
        puVar4[2] = *(undefined4 *)((int)local_18 + 0x60);
        puVar4 = (undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0xc + local_20 * 0x20);
        *puVar4 = DAT_00571068;
        puVar4[1] = DAT_0057106c;
        puVar4[2] = DAT_00571070;
        *(undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0x18 + local_20 * 0x20) =
             *(undefined4 *)((int)local_18 + 0x704);
        *(undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0x1c + local_20 * 0x20) =
             *(undefined4 *)((int)local_18 + 0x70c);
      }
      FUN_0047c5f0((void *)((int)local_18 + 0x48),*(float *)((int)local_18 + 0x744));
      if (*(int *)((int)local_18 + 0x740) == 0) {
        *(undefined4 *)((int)local_18 + 0x1330) = 0;
        *(undefined4 *)((int)local_18 + 0x1360) = *(undefined4 *)((int)local_18 + 0x7c);
        FUN_00452f20((void *)((int)local_18 + 0x135c),*(float *)((int)local_18 + 0x70));
        FUN_00439330((void *)((int)local_18 + 0x1344),(float *)((int)local_18 + 0x1344),
                     *(float *)((int)local_18 + 0x70),DAT_0056c8cc);
        *(undefined4 *)((int)local_18 + 0x134c) = 0;
        *(undefined4 *)((int)local_18 + 0x1340) = 0;
        *(undefined4 *)((int)local_18 + 0x1338) = 0;
        *(undefined4 *)((int)local_18 + 0x1350) = *(undefined4 *)((int)local_18 + 0x58);
        *(undefined4 *)((int)local_18 + 0x1354) = *(undefined4 *)((int)local_18 + 0x5c);
        *(undefined4 *)((int)local_18 + 0x1358) = *(undefined4 *)((int)local_18 + 0x60);
        *(undefined4 *)((int)local_18 + 0x133c) = DAT_00571790;
        *(undefined4 *)((int)local_18 + 0x690) = *(undefined4 *)((int)local_18 + 0x73c);
      }
      else {
        puVar4 = *(undefined4 **)((int)local_18 + 0x740);
        puVar5 = (undefined4 *)((int)local_18 + 0x1330);
        for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
          *puVar5 = *puVar4;
          puVar4 = puVar4 + 1;
          puVar5 = puVar5 + 1;
        }
        local_24 = (int *)((int)local_18 + 0x1330);
        for (local_28 = *(int **)((int)local_18 + 0x740); local_28 != (int *)0x0;
            local_28 = (int *)*local_28) {
          if (*local_28 != 0) {
            puVar4 = FUN_004c7f90();
            *local_24 = (int)puVar4;
            if (*local_24 == 0) goto LAB_004d3c87;
            puVar4 = (undefined4 *)*local_28;
            puVar5 = (undefined4 *)*local_24;
            for (iVar1 = 0xf; iVar1 != 0; iVar1 = iVar1 + -1) {
              *puVar5 = *puVar4;
              puVar4 = puVar4 + 1;
              puVar5 = puVar5 + 1;
            }
            local_24 = (int *)*local_24;
          }
        }
        *(undefined4 *)((int)local_18 + 0x740) = 0;
        *(undefined4 *)((int)local_18 + 0x690) = 99;
      }
      iVar1 = *(int *)((int)local_18 + 0x1328);
      *(undefined4 *)(iVar1 + 0xc) = *(undefined4 *)((int)local_18 + 100);
      *(undefined4 *)(iVar1 + 0x10) = *(undefined4 *)((int)local_18 + 0x68);
      *(undefined4 *)(iVar1 + 0x14) = *(undefined4 *)((int)local_18 + 0x6c);
      local_30 = 0;
      for (local_1c = 0; local_1c < *(int *)((int)local_18 + 0x718); local_1c = local_1c + 1) {
        fVar6 = FUN_00423bd0((int)local_18 + 0x48);
        local_68 = (float)fVar6;
        if (local_68 - (float)local_1c < _DAT_0056e0e8) {
          puVar4 = (undefined4 *)(*(int *)((int)local_18 + 0x1328) + local_1c * 0x20);
          *puVar4 = *(undefined4 *)((int)local_18 + 0x6f8);
          puVar4[1] = *(undefined4 *)((int)local_18 + 0x6fc);
          puVar4[2] = *(undefined4 *)((int)local_18 + 0x700);
          puVar4 = (undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0xc + local_1c * 0x20);
          *puVar4 = DAT_00571068;
          puVar4[1] = DAT_0057106c;
          puVar4[2] = DAT_00571070;
          *(undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0x18 + local_1c * 0x20) =
               *(undefined4 *)((int)local_18 + 0x704);
          *(undefined4 *)(*(int *)((int)local_18 + 0x1328) + 0x1c + local_1c * 0x20) =
               *(undefined4 *)((int)local_18 + 0x70c);
        }
        else {
          local_8c = (void *)((int)local_18 + 0x1330);
          fVar6 = FUN_00423bd0((int)local_18 + 0x48);
          local_6c = (float)fVar6;
          local_70 = local_6c - (float)local_1c;
          local_74 = *(float *)(*(int *)((int)local_18 + 0x1328) + 0x18 + (local_1c + -1) * 0x20);
          local_78 = *(float *)(*(int *)((int)local_18 + 0x1328) + 0x1c + (local_1c + -1) * 0x20);
          local_7c = (void *)(*(int *)((int)local_18 + 0x1328) + (local_1c + -1) * 0x20);
          local_80 = (float *)(*(int *)((int)local_18 + 0x1328) + 0x18 + local_1c * 0x20);
          local_84 = (float *)(*(int *)((int)local_18 + 0x1328) + 0x1c + local_1c * 0x20);
          local_88 = (undefined4 *)(*(int *)((int)local_18 + 0x1328) + local_1c * 0x20);
          FUN_004d62e0(local_8c,local_88,local_84,local_80,local_7c,local_78,local_74,local_70,
                       local_30);
          local_30 = 1;
        }
      }
      FUN_00423520((void *)((int)local_18 + 0x6ac),0x1e);
      if (-1 < *(int *)((int)local_18 + 0x734)) {
        FUN_00426eb0(&DAT_005ba830,*(int *)((int)local_18 + 0x734),0.0);
      }
      FUN_00423520((void *)((int)local_18 + 0x38),0);
      *(float *)((int)local_18 + 0x88) = DAT_0056c8cc;
      *(float *)((int)local_18 + 0x84) = DAT_0056c8cc;
      *(int *)((int)local_18 + 0x6dc) = *(int *)((int)local_18 + 0x718) + -1;
      uVar3 = FUN_0041f610(*(int *)((int)local_18 + 0x6dc) * 0x3c);
      *(undefined4 *)((int)local_18 + 0x6d8) = uVar3;
    }
  }
LAB_004d3c87:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

