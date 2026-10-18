/* Entry: 0x004e8820; symbol: FUN_004e8820; body bytes: 1269 */

void __cdecl FUN_004e8820(uint *param_1,uint *param_2,undefined8 param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  size_t *psVar3;
  int *unaff_FS_OFFSET;
  undefined8 uVar4;
  undefined1 local_3d8 [24];
  undefined8 local_3c0;
  undefined8 local_3b8;
  undefined4 local_3b0;
  undefined8 local_3ac;
  char *local_3a4;
  uint *local_3a0;
  undefined4 local_39c;
  uint *local_398;
  undefined4 local_394;
  uint *local_390;
  undefined4 local_38c;
  undefined8 local_388;
  int local_380;
  undefined4 local_37c;
  uint *local_378;
  undefined4 local_374;
  int local_370;
  uint *local_36c;
  int local_368;
  int local_364;
  int local_360;
  int local_35c;
  uint *local_358;
  uint local_354;
  uint *local_350;
  uint local_34c;
  int local_348;
  size_t local_344;
  char local_33d;
  uint *local_33c;
  char local_336;
  char local_335;
  undefined *local_334;
  undefined *local_330;
  int local_32c [2];
  undefined8 local_324;
  uint local_31c [194];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_0056a130;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar4 = FUN_004e7ae0(&param_3);
  local_324._0_4_ = (int)uVar4;
  local_324._4_4_ = (int)((ulonglong)uVar4 >> 0x20);
  local_324 = uVar4;
  if ((int)local_324 == 0 && local_324._4_4_ == 0) {
    if (param_1 == param_2) {
      local_378 = param_2;
      local_374 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = 0x30;
      local_380 = (int)param_1 + 1;
      local_37c = 0;
    }
  }
  else {
    if (param_4 < 0) {
      param_4 = 6;
    }
    else if (param_4 == 0) {
      param_4 = 1;
    }
    else if (999999 < param_4) {
      param_4 = 1000000;
    }
    if (param_4 < 0x10) {
      iVar1 = (((param_4 + -1) * (param_4 + 10)) / 2) * 8;
      local_330 = &DAT_00596020 + iVar1;
      local_334 = &UNK_00596048 + param_4 * 8 + iVar1;
    }
    else {
      local_330 = &DAT_00596638;
      piVar2 = std::_Min_value<int>(&param_4,(int *)&DAT_00572948);
      local_334 = local_330 + *piVar2 * 8 + 0x28;
    }
    piVar2 = FUN_004f3b90(local_3d8,&param_4,&local_330,&local_334,(undefined4 *)&local_324);
    local_360 = FUN_004f5950(piVar2);
    local_370 = local_360 - (int)local_330 >> 3;
    local_348 = local_370 + -5;
    if ((local_348 < param_4) && (-5 < local_348)) {
      local_336 = '\x01';
    }
    else {
      local_336 = '\0';
    }
    local_335 = local_336;
    local_3b0 = 0x305;
    local_32c[1] = 0x42;
    local_32c[0] = 0x2fe;
    local_358 = local_31c;
    local_33c = (uint *)0x0;
    local_350 = (uint *)0x0;
    local_35c = 0;
    if (local_336 == '\0') {
      local_368 = param_4 + -1;
      psVar3 = (size_t *)std::_Min_value<int>(&local_368,local_32c);
      local_344 = *psVar3;
      local_3c0 = param_3;
      local_36c = (uint *)FUN_004f3320((int)local_31c);
      local_388 = FUN_004e9600(local_31c,local_36c,local_3c0,local_344);
      local_33d = 'e';
      local_350 = (uint *)FUN_0046a3b0((char *)local_31c,(int)local_388,&local_33d);
      local_35c = (int)local_388;
      local_33c = local_350;
    }
    else {
      local_364 = param_4 - (local_370 + -4);
      psVar3 = (size_t *)std::_Min_value<int>(&local_364,local_32c + 1);
      local_344 = *psVar3;
      local_3b8 = param_3;
      local_3a4 = (char *)FUN_004f3320((int)local_31c);
      local_3ac = FUN_004e8770((char *)local_31c,local_3a4,local_3b8,local_344);
      local_33c = (uint *)local_3ac;
    }
    if (0 < (int)local_344) {
      for (; *(char *)((int)local_33c + -1) == '0'; local_33c = (uint *)((int)local_33c + -1)) {
      }
      if (*(char *)((int)local_33c + -1) == '.') {
        local_33c = (uint *)((int)local_33c + -1);
      }
    }
    local_34c = (int)local_33c - (int)local_358;
    if ((int)param_2 - (int)param_1 < (int)local_34c) {
      local_390 = param_2;
      local_38c = 0x84;
    }
    else {
      FUN_00543e20(param_1,local_358,local_34c);
      param_1 = (uint *)((int)param_1 + local_34c);
      if (local_335 == '\0') {
        local_354 = local_35c - (int)local_350;
        if ((int)param_2 - (int)param_1 < (int)local_354) {
          local_398 = param_2;
          local_394 = 0x84;
          goto LAB_004e8cfc;
        }
        FUN_00543e20(param_1,local_350,local_354);
        param_1 = (uint *)((int)param_1 + local_354);
      }
      local_3a0 = param_1;
      local_39c = 0;
    }
  }
LAB_004e8cfc:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

