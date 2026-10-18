/* Entry: 0x00478680; symbol: FUN_00478680; body bytes: 1385 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00478680(int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  undefined4 *puVar4;
  void *this;
  void *this_00;
  void *this_01;
  float10 fVar5;
  float fVar6;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  int local_98;
  float local_94;
  void *local_90;
  undefined4 *local_8c;
  void *local_88;
  float local_84;
  undefined4 local_80;
  float local_7c;
  float local_78;
  float local_74;
  void *local_70;
  void *local_6c;
  float local_68;
  void *local_64;
  float local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  int local_40;
  int local_3c;
  void *local_38;
  int local_34;
  float local_30 [3];
  float local_24;
  float local_20;
  float local_18 [2];
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_34 = param_1;
  local_38 = (void *)FUN_0044ced0((undefined4 *)(param_1 + 200));
  local_48 = (void *)FUN_00460830(0);
  FUN_00479060(local_48,0x28);
  local_40 = 300;
  if (local_38 == (void *)0x0) {
    FUN_00479040((void *)(local_34 + 0x74));
  }
  else {
    local_4c = (void *)(local_34 + 8);
    bVar1 = FUN_00423590(local_4c,300);
    if (bVar1) {
      local_50 = (void *)(local_34 + 8);
      bVar1 = FUN_004785e0(local_50,local_40);
      if (bVar1) {
        FUN_00479040((void *)(local_34 + 200));
        FUN_00479040((void *)(local_34 + 0x74));
        local_54 = (void *)FUN_00460830(0);
        FUN_00479080(local_54,DAT_0056c8cc);
        iVar2 = FUN_00460830(0);
        FUN_004790c0(iVar2);
      }
      fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
      FUN_00450590(local_38,(float)fVar5);
      iVar2 = FUN_00460830(0);
      pfVar3 = (float *)FUN_00479020(iVar2);
      if (0.0 < *pfVar3 || *pfVar3 == 0.0) {
        iVar2 = FUN_00460830(0);
        pfVar3 = (float *)FUN_00479020(iVar2);
        if (_DAT_0056e0e8 < *pfVar3) {
          FUN_004530c0((void *)(local_34 + 0xc4),DAT_0056e0f0 / _DAT_0056fa40);
        }
      }
      else {
        FUN_00472060((void *)(local_34 + 0xc4),DAT_0056e0f0 / _DAT_0056fa40);
      }
      local_58 = (void *)FUN_00460830(0);
      FUN_00479080(local_58,DAT_0056e0ec);
      iVar2 = FUN_00460830(0);
      puVar4 = (undefined4 *)FUN_00460850(iVar2);
      *(undefined4 *)(local_34 + 0xb8) = *puVar4;
      *(undefined4 *)(local_34 + 0xbc) = puVar4[1];
      *(undefined4 *)(local_34 + 0xc0) = puVar4[2];
      local_5c = (void *)(local_34 + 8);
      iVar2 = FUN_00478e00(local_5c,3);
      if (iVar2 != 0) {
        FUN_00422e10(local_18);
        local_10 = 0;
        fVar6 = DAT_0056fb84;
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        FUN_00439330(this,local_18,(float)fVar5,fVar6);
        FUN_004296e0(local_18,(float *)(local_34 + 0xb8));
        FUN_00425cc0(&local_c);
        local_64 = (void *)FUN_00478ec0(0);
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        local_60 = (float)fVar5;
        puVar4 = FUN_004c1ae0(local_64,&local_9c,local_18,DAT_0056f114,DAT_0056cd94,local_60,0,0x32)
        ;
        local_c = *puVar4;
        FUN_004c1c10(&local_c,&local_a0);
        fVar6 = DAT_0056f108;
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        FUN_00439330(this_00,local_18,(float)fVar5,fVar6);
        FUN_004296e0(local_18,(float *)(local_34 + 0xb8));
        local_6c = (void *)FUN_00478ec0(0);
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        local_68 = (float)fVar5;
        puVar4 = FUN_004c1ae0(local_6c,&local_a4,local_18,DAT_0056f114,DAT_0056cd98,local_68,0,0xf);
        local_c = *puVar4;
        FUN_004c1c10(&local_c,&local_a8);
        fVar6 = DAT_0056fb88;
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        FUN_00439330(this_01,local_18,(float)fVar5,fVar6);
        FUN_004296e0(local_18,(float *)(local_34 + 0xb8));
        local_90 = (void *)FUN_00478ec0(0);
        fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
        local_94 = (float)fVar5;
        puVar4 = FUN_004c1ae0(local_90,&local_ac,local_18,DAT_0056f114,DAT_0056cda0,local_94,0,0xf);
        local_c = *puVar4;
        FUN_004c1c10(&local_c,&local_b0);
      }
    }
    FUN_00422e10(local_30);
    FUN_00422e10(&local_24);
    local_3c = 0;
    local_98 = 0x39;
    while( true ) {
      local_70 = (void *)(local_34 + 200);
      local_44 = (void *)FUN_0044c670(local_70,local_98,local_3c);
      if (local_44 == (void *)0x0) break;
      fVar5 = FUN_004379c0((int)local_38);
      local_74 = (float)fVar5;
      local_24 = local_74 * DAT_0056fb7c;
      fVar5 = FUN_004379a0((int)local_38);
      local_78 = (float)fVar5;
      local_20 = local_78 * DAT_0056fb80;
      FUN_00478f00(local_44,local_30);
      local_80 = FUN_00478e80(0);
      fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
      local_7c = (float)fVar5;
      FUN_0047cc90(local_30,&local_24,local_7c,0,0);
      local_88 = (void *)FUN_00478ee0(0);
      fVar5 = FUN_004292e0((float *)(local_34 + 0xc4));
      local_84 = (float)fVar5;
      FUN_004c9db0(local_88,local_30,&local_24,local_84,0,1);
      local_3c = local_3c + 1;
    }
    local_8c = (undefined4 *)(local_34 + 8);
    FUN_00423540(local_8c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

