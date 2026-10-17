/* Entry: 0x00453e40; symbol: FUN_00453e40; body bytes: 689 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00453e40(void *param_1)

{
  float *pfVar1;
  void *this;
  void *this_00;
  float10 fVar2;
  undefined1 local_60 [12];
  float *local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  void *local_24;
  float *local_20;
  float local_1c;
  float local_18;
  float local_14;
  uint local_10;
  float local_c;
  void *local_8;
  
  if ((*(uint *)((int)param_1 + 0x44) >> 5 & 1) == 0) {
    local_10 = *(uint *)((int)param_1 + 0x44) & 0xf;
    local_8 = param_1;
    switch(local_10) {
    case 0:
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_14 = (float)fVar2;
      local_18 = local_14 * *(float *)((int)local_8 + 0x18);
      fVar2 = FUN_004292e0((float *)((int)local_8 + 0x1c));
      local_1c = (float)fVar2;
      local_20 = (float *)FUN_004562c0((int)local_8);
      FUN_00439330(this,local_20,local_1c,local_18);
      if (_DAT_0056e0e8 < *(float *)((int)local_8 + 0x34)) {
        local_24 = (void *)((int)local_8 + 0x38);
        pfVar1 = (float *)FUN_004292f0(local_24,local_60,*(float *)((int)local_8 + 0x34));
        FUN_00429740((void *)((int)local_8 + 0x38),pfVar1);
      }
      if ((*(uint *)((int)local_8 + 0x44) >> 4 & 1) != 0) {
        fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
        local_28 = (float)fVar2;
        local_c = local_28 * *(float *)((int)local_8 + 0x24);
        local_2c = local_c - local_c * *(float *)((int)local_8 + 0x34);
        fVar2 = FUN_004292e0((float *)((int)local_8 + 0x1c));
        local_30 = (float)fVar2;
        FUN_00452f20((void *)((int)local_8 + 0x1c),local_2c + local_30);
      }
      break;
    case 2:
    case 3:
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_34 = (float)fVar2;
      local_c = local_34 * *(float *)((int)local_8 + 0x24);
      *(float *)((int)local_8 + 0x20) =
           (local_c - local_c * *(float *)((int)local_8 + 0x34)) + *(float *)((int)local_8 + 0x20);
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_38 = (float)fVar2;
      local_c = local_38 * *(float *)((int)local_8 + 0x18);
      local_3c = local_c - local_c * *(float *)((int)local_8 + 0x34);
      fVar2 = FUN_004292e0((float *)((int)local_8 + 0x1c));
      local_40 = (float)fVar2;
      FUN_00452f20((void *)((int)local_8 + 0x1c),local_3c + local_40);
      break;
    case 4:
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_44 = (float)fVar2;
      local_c = local_44 * *(float *)((int)local_8 + 0x24);
      FUN_004530c0((void *)((int)local_8 + 0x30),local_c - local_c * *(float *)((int)local_8 + 0x34)
                  );
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_48 = (float)fVar2;
      local_4c = local_48 * *(float *)((int)local_8 + 0x18);
      fVar2 = FUN_004292e0((float *)((int)local_8 + 0x28));
      local_50 = (float)fVar2;
      local_54 = (float *)FUN_004562c0((int)local_8);
      FUN_00439330(this_00,local_54,local_50,local_4c);
      FUN_004591f0(local_8,0);
    }
  }
  return;
}

