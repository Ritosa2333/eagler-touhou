/* Entry: 0x00510110; symbol: FUN_00510110; body bytes: 1324 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00510110(int param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  float *pfVar4;
  float10 fVar5;
  float fVar6;
  float fVar7;
  float local_30;
  byte *local_2c;
  uint local_28;
  int local_24;
  char local_20;
  int local_18;
  float local_14;
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0044f3d0(DAT_005c0028,0x15,*(int *)(param_1 + 0xf90));
  local_18 = param_1 + 0x600;
  FUN_00422e10(&local_14);
  uVar2 = FUN_004dc140(0x5c4d40);
  if (uVar2 == 0) {
    FUN_004dda60(0x5c4d40);
  }
  local_24 = 0;
  do {
    if (0xc < local_24) {
      for (local_24 = 0; local_24 < 5; local_24 = local_24 + 1) {
        if (*(char *)(local_18 + 0x38) != '\0') {
          FUN_00488b00(DAT_005c0698,2);
          FUN_00488a00(DAT_005c0698,2);
          FUN_00470ad0(DAT_005c0698,*(undefined4 *)(local_18 + 0x1c));
          FUN_004b8620(DAT_005c0698,0,0);
          local_c = *(undefined4 *)(local_18 + 0x14);
          local_14 = DAT_0056e050 + DAT_0056cd94 + *(float *)(local_18 + 0xc);
          local_10 = *(float *)(local_18 + 0x10) + DAT_0056cd90;
          if (*(int *)(local_18 + 0x3c) < 0) {
            FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"NO BONUS");
          }
          else {
            FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"BONUS %.1f");
            local_10 = local_10 + _DAT_005739f8;
            FUN_0046c990(DAT_005c0698,&local_14,L"搥");
          }
          FUN_00470ad0(DAT_005c0698,0xffffffff);
          FUN_00488a00(DAT_005c0698,0);
          FUN_00488b00(DAT_005c0698,0);
          FUN_004b8620(DAT_005c0698,1,1);
        }
        local_18 = local_18 + 0x44;
      }
      FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
      return;
    }
    if (*(char *)(local_18 + 0x38) != '\0') {
      bVar1 = FUN_00423560((void *)(local_18 + 0x20),8);
      if (bVar1) {
        fVar5 = FUN_00423bd0(local_18 + 0x20);
        local_30 = DAT_0056d7bc / (float)fVar5;
      }
      else {
        local_30 = DAT_0056d7bc;
      }
      FUN_00510850((void *)(param_1 + 0x1c),
                   *(float *)(local_18 + 0xc) -
                   ((float)*(byte *)(local_18 + 0x39) * local_30) / DAT_0056c8d0);
      FUN_00510870((void *)(param_1 + 0x1c),*(undefined4 *)(local_18 + 0x10));
      FUN_0044fa10((void *)(param_1 + 0x1c),*(undefined4 *)(local_18 + 0x1c));
      iVar3 = FUN_0040ff90(*(int *)(param_1 + 0xf94));
      pfVar4 = (float *)FUN_00460850(iVar3);
      fVar6 = *pfVar4 - *(float *)(local_18 + 0xc);
      iVar3 = FUN_0040ff90(*(int *)(param_1 + 0xf94));
      iVar3 = FUN_00460850(iVar3);
      fVar7 = *(float *)(iVar3 + 4) - *(float *)(local_18 + 0x10);
      iVar3 = (int)(fVar6 * fVar6 + fVar7 * fVar7);
      if (iVar3 < 0x4001) {
        if (iVar3 < 0x1001) {
          local_20 = -0x80;
        }
        else {
          local_20 = (char)(((iVar3 + -0x1000) * 0x80) / 0x3000) + -0x80;
        }
      }
      else {
        local_20 = -1;
      }
      local_2c = (byte *)(local_18 + -1 + (uint)*(byte *)(local_18 + 0x39));
      for (local_28 = (uint)*(byte *)(local_18 + 0x39); 0 < (int)local_28; local_28 = local_28 - 1)
      {
        bVar1 = FUN_00423560((void *)(local_18 + 0x20),local_28 * -2 + 0x34);
        if ((bVar1) || (*local_2c == 10)) {
          FUN_00470ed0((void *)(param_1 + 0x1c),*local_2c + 0x121);
          FUN_00470b60((void *)(param_1 + 0x1c),local_20);
LAB_00510456:
          iVar3 = FUN_00437cd0(param_1 + 0x1c);
          FUN_00506de0((void *)(param_1 + 0x1c),*(undefined4 *)(iVar3 + 0x4c));
          FUN_0043f550(DAT_005c0028,(void *)(param_1 + 0x1c));
        }
        else {
          bVar1 = FUN_00423560((void *)(local_18 + 0x20),local_28 * -2 + 0x38);
          if (bVar1) {
            FUN_00470ed0((void *)(param_1 + 0x1c),*local_2c + 300);
            FUN_00470b60((void *)(param_1 + 0x1c),local_20);
            goto LAB_00510456;
          }
          bVar1 = FUN_00423560((void *)(local_18 + 0x20),local_28 * -2 + 0x3c);
          if (bVar1) {
            FUN_00470ed0((void *)(param_1 + 0x1c),*local_2c + 0x136);
            FUN_00470b60((void *)(param_1 + 0x1c),local_20);
            goto LAB_00510456;
          }
        }
        pfVar4 = (float *)FUN_0044cb20(param_1 + 0x1c);
        *pfVar4 = *pfVar4 + local_30;
        local_2c = local_2c + -1;
      }
    }
    local_24 = local_24 + 1;
    local_18 = local_18 + 0x44;
  } while( true );
}

