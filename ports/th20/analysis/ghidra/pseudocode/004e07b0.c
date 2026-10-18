/* Entry: 0x004e07b0; symbol: FUN_004e07b0; body bytes: 1670 */

void __fastcall FUN_004e07b0(int param_1)

{
  int iVar1;
  undefined1 local_ac [12];
  undefined1 local_a0 [12];
  undefined1 local_94 [12];
  undefined1 local_88 [12];
  undefined1 local_7c [12];
  undefined1 local_70 [12];
  undefined1 local_64 [12];
  float *local_58;
  float *local_54;
  float *local_50;
  float *local_4c;
  float *local_48;
  float *local_44;
  float *local_40;
  uint local_3c;
  int local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  int local_24;
  float local_20;
  float local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_24 = param_1;
  if (DAT_005c4d28 == 0) {
    FUN_00488b00(DAT_005c0698,7);
    local_14 = *(float *)(local_24 + 0x70);
    local_10 = *(float *)(local_24 + 0x74);
    local_c = *(undefined4 *)(local_24 + 0x78);
    FUN_00422e10(&local_20);
    FUN_004b8620(DAT_005c0698,0,1);
    local_14 = local_14 + DAT_00571058;
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"Option");
    local_10 = local_10 + DAT_0056fd48;
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    local_14 = local_14 - DAT_00571058;
    local_10 = local_10 + DAT_0056f7a8;
    FUN_004b8620(DAT_005c0698,1,1);
    for (local_28 = 0; local_28 < 6; local_28 = local_28 + 1) {
      local_20 = local_14;
      local_1c = local_10;
      local_18 = local_c;
      local_38 = FUN_0040ff90(local_24 + 0x7c);
      local_3c = FUN_0040ff90(local_24 + 0x8c);
      FUN_0046b590(&local_14,local_28,(void *)(local_24 + 0x14),local_3c,local_38);
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)(&PTR_s_Screen_005afe88)[local_28]);
      local_14 = local_14 + DAT_0056f108;
      FUN_004b8620(DAT_005c0698,0,1);
      local_2c = local_28;
      if (local_28 == 0) {
        local_30 = FUN_0041cc70(0x5b6758);
        switch(local_30) {
        case 0:
        case 1:
        case 2:
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"FullScreen");
          break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"Window");
          break;
        case 8:
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"BorderlessDBD");
          break;
        case 9:
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"Borderless");
        }
      }
      else if (local_28 == 1) {
        FUN_0046c990(DAT_005c0698,&local_14,L"搥┥");
      }
      else if (local_28 == 2) {
        FUN_0046c990(DAT_005c0698,&local_14,L"搥┥");
      }
      FUN_004b8620(DAT_005c0698,1,1);
      local_14 = local_20;
      local_c = local_18;
      local_10 = local_1c + DAT_0056fd48;
    }
    FUN_00488b00(DAT_005c0698,0);
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004c6180(DAT_005c0698,3);
    FUN_00470ad0(DAT_005c0698,0xffc0c080);
    FUN_004c6160(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004b8620(DAT_005c0698,0,0);
    local_34 = FUN_0040c300((undefined4 *)(local_24 + 0x14));
    switch(local_34) {
    case 0:
      local_40 = (float *)FUN_00422dd0(local_64,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_0057228c);
      FUN_0046c210(DAT_005c0698,local_40);
      local_44 = (float *)FUN_00422dd0(local_70,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056f10c,0);
      iVar1 = FUN_0041cc70(0x5b6758);
      FUN_0040ddd0(&stack0xffffff48,(&PTR_DAT_005afeb0)[iVar1]);
      FUN_0046c210(DAT_005c0698,local_44);
      break;
    case 1:
      local_48 = (float *)FUN_00422dd0(local_7c,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_005722a4);
      FUN_0046c210(DAT_005c0698,local_48);
      break;
    case 2:
      local_4c = (float *)FUN_00422dd0(local_88,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_005722bc);
      FUN_0046c210(DAT_005c0698,local_4c);
      break;
    case 3:
      local_50 = (float *)FUN_00422dd0(local_94,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_005722d8);
      FUN_0046c210(DAT_005c0698,local_50);
      break;
    case 4:
      local_54 = (float *)FUN_00422dd0(local_a0,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_00570f94);
      FUN_0046c210(DAT_005c0698,local_54);
      break;
    case 5:
      local_58 = (float *)FUN_00422dd0(local_ac,*(float *)(local_24 + 0x70) + DAT_00571058,
                                       *(float *)(local_24 + 0x74) + DAT_0056ec9c,0);
      FUN_0040ddd0(&stack0xffffff48,&DAT_005722fc);
      FUN_0046c210(DAT_005c0698,local_58);
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004b8d90(DAT_005c0698,DAT_0056c8cc,DAT_0056c8cc);
    FUN_004b8620(DAT_005c0698,1,1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

