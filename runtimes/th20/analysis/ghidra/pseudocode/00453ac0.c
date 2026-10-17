/* Entry: 0x00453ac0; symbol: FUN_00453ac0; body bytes: 809 */

void __fastcall FUN_00453ac0(float *param_1)

{
  float *pfVar1;
  void *this;
  void *this_00;
  void *extraout_ECX;
  void *this_01;
  float10 fVar2;
  undefined1 local_a0 [12];
  undefined1 local_94 [12];
  undefined1 local_88 [12];
  undefined1 local_7c [12];
  undefined1 local_70 [4];
  undefined1 local_6c [4];
  float *local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float *local_54;
  void *local_50;
  float local_4c;
  float local_48;
  float *local_44;
  void *local_40;
  float local_3c;
  float local_38;
  uint local_34;
  float *local_30;
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
  local_30 = param_1;
  if (((uint)param_1[0x11] >> 5 & 1) == 0) {
    FUN_00422e10(&local_14);
    local_34 = (uint)local_30[0x11] & 0xf;
    switch(local_34) {
    case 0:
      pfVar1 = (float *)FUN_004562c0((int)local_30);
      local_14 = *pfVar1;
      local_10 = pfVar1[1];
      local_c = pfVar1[2];
      FUN_004296e0(local_30,&local_14);
      break;
    case 2:
      local_38 = local_30[8];
      fVar2 = FUN_004292e0(local_30 + 7);
      local_3c = (float)fVar2;
      FUN_00439330(this,&local_14,local_3c,local_38);
      local_c = 0.0;
      local_40 = (void *)FUN_004562c0((int)local_30);
      pfVar1 = (float *)FUN_00429570(local_40,local_7c,&local_14);
      *local_30 = *pfVar1;
      local_30[1] = pfVar1[1];
      local_30[2] = pfVar1[2];
      break;
    case 3:
      local_48 = local_30[8];
      local_44 = local_30 + 7;
      pfVar1 = (float *)FUN_004294e0(local_44,local_6c,local_30 + 10);
      fVar2 = FUN_004292e0(pfVar1);
      fVar2 = FUN_00438540((float)fVar2);
      local_4c = (float)fVar2;
      FUN_00439330(this_00,&local_14,local_4c,local_48);
      local_14 = local_14 * local_30[0xb];
      fVar2 = FUN_004292e0(local_30 + 10);
      FUN_00458fa0(&local_14,&local_14,&local_14,(float)fVar2);
      local_c = 0.0;
      local_50 = (void *)FUN_004562c0((int)local_30);
      pfVar1 = (float *)FUN_00429570(local_50,local_88,&local_14);
      *local_30 = *pfVar1;
      local_30[1] = pfVar1[1];
      local_30[2] = pfVar1[2];
      break;
    case 4:
      FUN_00422e10(&local_20);
      FUN_00422e10(&local_2c);
      local_2c = *local_30;
      local_28 = local_30[1];
      local_24 = local_30[2];
      pfVar1 = (float *)FUN_004562c0((int)local_30);
      FUN_004296e0(local_30 + 3,pfVar1);
      local_54 = local_30 + 10;
      pfVar1 = (float *)FUN_00452fc0(local_54,local_70,DAT_0056e0f0 / DAT_0056c8d0);
      fVar2 = FUN_004292e0(pfVar1);
      fVar2 = FUN_00438540((float)fVar2);
      local_64 = (float)fVar2;
      fVar2 = FUN_004292e0(local_30 + 0xc);
      fVar2 = FUN_00439820(extraout_ECX,(float)fVar2);
      local_58 = (float)fVar2;
      local_5c = local_58 * local_30[8];
      fVar2 = FUN_004292e0((float *)&DAT_005aefe4);
      local_60 = (float)fVar2;
      FUN_00439330(this_01,&local_20,local_64,local_5c * local_60);
      local_18 = 0.0;
      pfVar1 = (float *)FUN_00429570(&local_20,local_94,local_30 + 3);
      *local_30 = *pfVar1;
      local_30[1] = pfVar1[1];
      local_30[2] = pfVar1[2];
      local_68 = local_30;
      pfVar1 = (float *)FUN_00429440(local_30,local_a0,&local_2c);
      local_20 = *pfVar1;
      local_1c = pfVar1[1];
      local_18 = pfVar1[2];
      fVar2 = (float10)FUN_00456210(&local_20);
      FUN_00452f20(local_30 + 7,(float)fVar2);
    }
    FUN_004543d0(local_30);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

