/* Entry: 0x004d2dd0; symbol: FUN_004d2dd0; body bytes: 817 */

/* WARNING: Removing unreachable block (ram,0x004d2f30) */

void __thiscall FUN_004d2dd0(void *this,int param_1)

{
  float *pfVar1;
  code *pcVar2;
  uint uVar3;
  void *pvVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  int local_48;
  undefined1 local_38 [12];
  float local_2c [3];
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pfVar1 = *(float **)((int)this + 0x6d8);
  FUN_004863c0((void *)((int)this + 0x74c),0);
  if ((*(uint *)((int)this + 0x1c) >> 6 & 1) == 0) {
                    /* WARNING: Load size is inaccurate */
    uVar3 = (**(code **)(*this + 0x10))(0);
  }
  else {
    uVar3 = (uint)pfVar1[0xe] >> 3 & 1;
  }
  if (uVar3 != 0) {
    *(undefined4 *)((int)this + 0x84) = DAT_0056c8cc;
    FUN_00422e10(&local_14);
    FUN_00422e10(&local_20);
    local_14 = *pfVar1;
    local_10 = pfVar1[1];
    local_c = pfVar1[2];
    pvVar4 = (void *)FUN_0040ff90(*(int *)((int)this + 0x6f0));
    iVar5 = FUN_004f91d0(pvVar4,&local_14,pfVar1[9],pfVar1[7],pfVar1[6],param_1);
    if (iVar5 == 2) {
      iVar6 = FUN_0040ff90(*(int *)((int)this + 0x6f0));
      puVar7 = (undefined4 *)FUN_00460850(iVar6);
      local_20 = *puVar7;
      local_1c = puVar7[1];
      local_18 = puVar7[2];
    }
    if (iVar5 == 1) {
      FUN_00422dd0(local_38,DAT_0056cd94,DAT_0056cd94,0);
                    /* WARNING: Load size is inaccurate */
      pcVar2 = *(code **)(*this + 0x2c);
      iVar5 = FUN_0040ff90(*(int *)((int)this + 0x6f0));
      iVar5 = FUN_00460850(iVar5);
      (*pcVar2)(iVar5,local_38,0,0,1);
    }
    else if (iVar5 == 2) {
      *(int *)((int)this + 0x6d4) = *(int *)((int)this + 0x6d4) + 1;
      iVar5 = FUN_00472040((void *)((int)this + 0x38),8);
      if (iVar5 == 0) {
        FUN_00422e10(local_2c);
        FUN_00454d80(&local_14,pfVar1[6],pfVar1[9],&local_20,local_2c);
        pvVar4 = (void *)FUN_0040ff90(*(int *)((int)this + 0x6f0));
        FUN_004f8b90(pvVar4,local_2c,*(undefined4 *)((int)this + 0x6d0));
      }
      if ((*(uint *)((int)this + 0x69c) >> 1 & 1) == 0) {
        local_48 = *(int *)((int)this + 0x6d4) * -2 + 0xd0;
        if (0xf0 < local_48) {
          local_48 = 0xf0;
        }
        if (local_48 < 0x60) {
          local_48 = 0x60;
        }
        FUN_004863c0((void *)((int)this + 0x74c),1);
        FUN_004864b0((void *)((int)this + 0x74c),0xff,(char)local_48,0x80,0xff);
      }
      FUN_00423540((undefined4 *)((int)this + 0x38));
    }
    else {
      *(undefined4 *)((int)this + 0x6d4) = 0;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

