/* Entry: 0x004d2750; symbol: FUN_004d2750; body bytes: 720 */

void __thiscall FUN_004d2750(void *this,int param_1)

{
  code *pcVar1;
  bool bVar2;
  void *pvVar3;
  int iVar4;
  int iVar5;
  undefined4 *puVar6;
  int local_58;
  float *local_54;
  float *local_4c;
  undefined1 local_44 [12];
  float local_38 [3];
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  float local_20;
  float local_1c;
  float local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_54 = *(float **)((int)this + 0x6d8);
  if ((*(uint *)((int)this + 0x1c) >> 6 & 1) == 0) {
                    /* WARNING: Load size is inaccurate */
    (**(code **)(*this + 0x10))(0);
  }
  if (*(int *)((int)this + 0x6e0) != 0) {
    FUN_00422e10(&local_20);
    FUN_00422e10(&local_14);
    FUN_00422e10(&local_2c);
    bVar2 = false;
    local_4c = local_54;
    for (local_58 = 0; local_58 < *(int *)((int)this + 0x6e0); local_58 = local_58 + 1) {
      local_20 = *local_4c;
      local_1c = local_4c[1];
      local_18 = local_4c[2];
      pvVar3 = (void *)FUN_0040ff90(*(int *)((int)this + 0x6f0));
      iVar4 = FUN_004f91d0(pvVar3,&local_20,local_4c[9],local_4c[7],local_4c[6],param_1);
      if (iVar4 == 2) {
        iVar5 = FUN_0040ff90(*(int *)((int)this + 0x6f0));
        puVar6 = (undefined4 *)FUN_00460850(iVar5);
        local_14 = *puVar6;
        local_10 = puVar6[1];
        local_c = puVar6[2];
      }
      if (iVar4 == 1) {
        FUN_00422dd0(local_44,DAT_0056cd94,DAT_0056cd94,0);
                    /* WARNING: Load size is inaccurate */
        pcVar1 = *(code **)(*this + 0x2c);
        iVar4 = FUN_0040ff90(*(int *)((int)this + 0x6f0));
        iVar4 = FUN_00460850(iVar4);
        (*pcVar1)(iVar4,local_44,0,0,1);
      }
      else if ((iVar4 != 2) || (bVar2)) {
        *(undefined4 *)((int)this + 0x6d4) = 0;
      }
      else {
        bVar2 = true;
        local_54 = local_4c;
        local_2c = local_14;
        local_28 = local_10;
        local_24 = local_c;
      }
      local_4c = local_4c + 0xf;
    }
    if ((bVar2) && (iVar4 = FUN_00472040((void *)((int)this + 0x38),8), iVar4 == 0)) {
      FUN_00422e10(local_38);
      FUN_00454d80(local_54,local_54[6],local_54[9],&local_2c,local_38);
      pvVar3 = (void *)FUN_0040ff90(*(int *)((int)this + 0x6f0));
      FUN_004f8b90(pvVar3,local_38,*(undefined4 *)((int)this + 0x6d0));
      *(int *)((int)this + 0x6d4) = *(int *)((int)this + 0x6d4) + 1;
    }
    FUN_00423540((undefined4 *)((int)this + 0x38));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

