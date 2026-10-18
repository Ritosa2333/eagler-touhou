/* Entry: 0x005257f0; symbol: FUN_005257f0; body bytes: 3502 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005257f0(int param_1)

{
  bool bVar1;
  int iVar2;
  char *pcVar3;
  undefined4 *puVar4;
  undefined1 local_214 [12];
  undefined1 local_208 [12];
  undefined1 local_1fc [12];
  undefined1 local_1f0 [12];
  undefined *local_1e4 [4];
  undefined *local_1d4;
  undefined *local_1d0;
  undefined *local_1cc;
  undefined *local_1c8;
  undefined *local_1c4;
  undefined *local_1c0;
  undefined4 local_1bc;
  void *local_1b8;
  void *local_1b4;
  undefined8 local_1b0;
  void *local_1a8;
  void *local_1a4;
  undefined4 local_1a0;
  void *local_19c;
  float *local_198;
  undefined *local_194;
  undefined4 local_190;
  void *local_18c;
  void *local_188;
  void *local_184;
  undefined *local_180;
  undefined4 local_17c;
  undefined4 local_178;
  void *local_174;
  void *local_170;
  float *local_16c;
  undefined4 local_168;
  undefined4 local_164;
  void *local_160;
  uint local_15c;
  uint local_158;
  int local_154;
  int local_150;
  void *local_14c;
  void *local_148;
  float *local_144;
  void *local_140;
  int local_13c;
  void *local_138;
  float *local_134;
  void *local_130;
  int local_12c;
  int local_128;
  undefined8 local_124;
  undefined8 local_11c;
  int local_114;
  undefined *local_110;
  undefined *local_10c;
  undefined *local_108;
  undefined *local_104;
  uint local_100;
  int local_fc;
  int local_f8;
  int local_f4;
  uint local_f0;
  int local_ec;
  int local_e8;
  int local_e4;
  float local_e0;
  float local_dc;
  tm local_d4;
  char local_b0 [168];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_12c = *(int *)(param_1 + 0x20);
  local_e8 = param_1;
  if (local_12c == 2) {
    FUN_004c6160(DAT_005c0698,0xffffffff);
    FUN_00470ab0(DAT_005c0698,0xff000000);
    FUN_004c6180(DAT_005c0698,4);
    FUN_004b8620(DAT_005c0698,0,1);
    local_130 = (void *)(local_e8 + 0x24);
    local_134 = (float *)FUN_00422dd0(local_208,DAT_0056f10c,DAT_0056fa30,0);
    iVar2 = FUN_0051e0a0(local_130,8);
    FUN_0040ddd0(&stack0xfffffde0,(&PTR_DAT_005aff88)[iVar2]);
    FUN_0046c210(DAT_005c0698,local_134);
    local_140 = (void *)FUN_0051b960();
    local_138 = (void *)(local_e8 + 0x24);
    local_13c = FUN_0051e0c0(local_138,8);
    local_144 = (float *)FUN_00422dd0(local_214,DAT_0056f10c,DAT_0056fe7c,0);
    pcVar3 = (char *)FUN_004dfad0(local_140,local_13c);
    FUN_0040ddd0(&stack0xfffffde0,pcVar3);
    FUN_0046c210(DAT_005c0698,local_144);
    FUN_004e67e0(DAT_005c0698);
    FUN_00422dd0(&local_e0,DAT_0056fb7c,DAT_0057566c,0);
    local_114 = FUN_0040c300((undefined4 *)(local_e8 + 0xbc));
    local_148 = (void *)(local_e8 + 0x24);
    local_150 = FUN_0051e0c0(local_148,8);
    local_14c = (void *)(local_e8 + 0x24);
    local_154 = FUN_0051e0a0(local_14c,8);
    local_f4 = FUN_004bd460(DAT_005c6108,local_154,local_150);
    if (*(int *)(local_e8 + 0x58f8) == 0) {
      FUN_004e6a20(DAT_005c0698,1);
      iVar2 = FUN_0040c300((undefined4 *)(local_e8 + 0x108));
      if (iVar2 == 0) {
        for (local_f8 = 0; local_f8 < 10; local_f8 = local_f8 + 1) {
          local_158 = local_f8 * -0x10 + 0xff;
          FUN_00470ad0(DAT_005c0698,local_158 | 0xffffff00);
          local_128 = local_f4 + 0x18 + local_114 * 400 + local_f8 * 0x28;
          local_ec = local_128;
          if (*(int *)(local_128 + 0x18) == 0 && *(int *)(local_128 + 0x1c) == 0) {
            FUN_0046c990(DAT_005c0698,&local_e0,
                         (wchar_t *)"%2d  %s  %9llu%d  ----/--/-- --:--  Stage -  ---%%");
          }
          else {
            FUN_004ac270(&local_d4,(uint *)(local_128 + 0x18));
            FUN_0046c990(DAT_005c0698,&local_e0,
                         (wchar_t *)"%2d  %s  %9llu%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%");
          }
          local_dc = local_dc + _DAT_00572644;
        }
      }
    }
    else {
      local_e4 = 0;
      local_f0 = 0;
      while (iVar2 = FUN_0040c300((undefined4 *)(local_e8 + 0x108)), (int)local_f0 < iVar2 * 10) {
        iVar2 = FUN_0040c300((undefined4 *)(local_e8 + 0xbc));
        if (*(int *)(&DAT_005af058 + local_e4 * 4) == iVar2) {
          local_f0 = local_f0 + 1;
        }
        local_e4 = local_e4 + 1;
      }
      local_1e4[0] = &DAT_00574f60;
      local_1e4[1] = &DAT_00574f64;
      local_1e4[2] = &DAT_00574f68;
      local_1e4[3] = &DAT_00574f6c;
      local_1d4 = &DAT_00574f70;
      local_1d0 = &DAT_00574f74;
      local_1cc = &DAT_00574f78;
      local_1c8 = &DAT_00574f7c;
      local_1c4 = &DAT_00574f80;
      local_1c0 = &DAT_00574f84;
      FUN_004c6160(DAT_005c0698,0xffffffff);
      FUN_00470ab0(DAT_005c0698,0xff000000);
      FUN_004c6180(DAT_005c0698,4);
      FUN_004b8620(DAT_005c0698,0,1);
      local_e0 = DAT_0056cda0;
      for (local_fc = 0; local_fc < 10; local_fc = local_fc + 1) {
        local_15c = local_fc * -0x10 + 0xff;
        FUN_004c6160(DAT_005c0698,local_15c | 0xffffff00);
        while ((local_e4 < 0x71 &&
               (iVar2 = FUN_0040c300((undefined4 *)(local_e8 + 0xbc)),
               *(int *)(&DAT_005af058 + local_e4 * 4) != iVar2))) {
          local_e4 = local_e4 + 1;
        }
        if (0x70 < local_e4) break;
        iVar2 = FUN_00488700((int)DAT_005c6108);
        local_160 = (void *)(iVar2 + 0xb08);
        iVar2 = FUN_00486e10(local_160,local_e4);
        if (*(int *)(iVar2 + 200) == 0) {
          local_170 = (void *)(local_f4 + 0xb08);
          iVar2 = FUN_00486e10(local_170,local_e4);
          local_178 = *(undefined4 *)(iVar2 + 200);
          local_174 = (void *)(local_f4 + 0xb08);
          iVar2 = FUN_00486e10(local_174,local_e4);
          local_17c = *(undefined4 *)(iVar2 + 0xc0);
          local_180 = local_1e4[(local_e4 + 1) % 10];
          if ((((local_e4 + 1) / 10) % 10 == 0) && ((local_e4 + 1) / 100 == 0)) {
            local_10c = &DAT_00575334;
          }
          else {
            local_10c = local_1e4[((local_e4 + 1) / 10) % 10];
          }
          if ((local_e4 + 1) / 100 == 0) {
            local_110 = &DAT_00575334;
          }
          else {
            local_110 = local_1e4[(local_e4 + 1) / 100];
          }
          FUN_0040ddd0(&stack0xfffffdcc,&DAT_00575350);
          FUN_0046c210(DAT_005c0698,&local_e0);
          local_184 = (void *)(local_e8 + 0x154);
          bVar1 = FUN_004235c0(local_184,local_fc);
          if (bVar1) {
            local_188 = (void *)(local_f4 + 0xb08);
            iVar2 = FUN_00486e10(local_188,local_e4);
            local_190 = *(undefined4 *)(iVar2 + 200);
            local_18c = (void *)(local_f4 + 0xb08);
            iVar2 = FUN_00486e10(local_18c,local_e4);
            local_1bc = *(undefined4 *)(iVar2 + 0xc0);
            local_198 = (float *)FUN_00422dd0(local_1fc,local_e0 + DAT_0056ec9c,local_dc,0);
            FUN_0040ddd0(&stack0xfffffdd8,"%4d/%4d");
            FUN_0046c210(DAT_005c0698,local_198);
          }
        }
        else {
          iVar2 = FUN_00488700((int)DAT_005c6108);
          local_1b8 = (void *)(iVar2 + 0xb08);
          iVar2 = FUN_00486e10(local_1b8,local_e4);
          FUN_00548610(local_b0,0xa5,iVar2);
          for (local_f0 = _strlen(local_b0); (int)local_f0 < 0x2a; local_f0 = local_f0 + 2) {
            FUN_00548610(local_b0 + local_f0,0xa5 - local_f0,0x575334);
          }
          local_100 = local_f0;
          if (0xa4 < local_f0) {
            ___report_rangecheckfailure();
          }
          local_b0[local_100] = '\0';
          local_194 = local_1e4[(local_e4 + 1) % 10];
          if ((((local_e4 + 1) / 10) % 10 == 0) && ((local_e4 + 1) / 100 == 0)) {
            local_104 = &DAT_00575334;
          }
          else {
            local_104 = local_1e4[((local_e4 + 1) / 10) % 10];
          }
          if ((local_e4 + 1) / 100 == 0) {
            local_108 = &DAT_00575334;
          }
          else {
            local_108 = local_1e4[(local_e4 + 1) / 100];
          }
          FUN_0040ddd0(&stack0xfffffdd0,"No.%s%s%s   %s");
          FUN_0046c210(DAT_005c0698,&local_e0);
          local_1b4 = (void *)(local_e8 + 0x154);
          bVar1 = FUN_004235c0(local_1b4,local_fc);
          if (bVar1) {
            local_1a4 = (void *)(local_f4 + 0xb08);
            iVar2 = FUN_00486e10(local_1a4,local_e4);
            local_164 = *(undefined4 *)(iVar2 + 200);
            local_1a8 = (void *)(local_f4 + 0xb08);
            iVar2 = FUN_00486e10(local_1a8,local_e4);
            local_168 = *(undefined4 *)(iVar2 + 0xc0);
            local_16c = (float *)FUN_00422dd0(local_1f0,local_e0 + DAT_0056ec9c,local_dc,0);
            FUN_0040ddd0(&stack0xfffffdd8,"%4d/%4d");
            FUN_0046c210(DAT_005c0698,local_16c);
          }
        }
        local_e4 = local_e4 + 1;
        local_dc = local_dc + _DAT_00572644;
      }
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    local_e0 = DAT_00575680;
    local_dc = DAT_00575690;
    FUN_0046c990(DAT_005c0698,&local_e0,(wchar_t *)"    %5d");
    local_dc = local_dc + _DAT_00572644;
    local_11c = __aulldiv(*(uint *)(local_f4 + 0x76b0),*(uint *)(local_f4 + 0x76b4),100,0);
    local_124 = __aulldiv((uint)local_11c,(uint)((ulonglong)local_11c >> 0x20),0x3c,0);
    local_1b0 = __aulldiv((uint)local_124,(uint)((ulonglong)local_124 >> 0x20),0x3c,0);
    __aullrem((uint)local_11c,(uint)((ulonglong)local_11c >> 0x20),0x3c,0);
    __aullrem((uint)local_124,local_124._4_4_,0x3c,0);
    FUN_0046c990(DAT_005c0698,&local_e0,(wchar_t *)"%3lld:%.2lld:%.2lld");
    local_dc = local_dc + _DAT_00572644;
    local_19c = (void *)(local_f4 + 0x76b8);
    puVar4 = (undefined4 *)FUN_00414580(local_19c,local_114);
    local_1a0 = *puVar4;
    FUN_0046c990(DAT_005c0698,&local_e0,(wchar_t *)"    %5d");
    local_dc = local_dc + _DAT_00572644;
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

