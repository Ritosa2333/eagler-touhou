/* Entry: 0x0052e210; symbol: FUN_0052e210; body bytes: 1298 */

void FUN_0052e210(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  undefined4 *puVar4;
  char *pcVar5;
  uint *puVar6;
  int *unaff_FS_OFFSET;
  undefined1 auStack_1d4 [8];
  undefined4 uStack_1cc;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_1b4 [28];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_198 [24];
  undefined1 local_180 [4];
  undefined1 local_17c [4];
  undefined1 local_178 [4];
  undefined1 *local_174;
  undefined1 *local_170;
  undefined4 local_16c;
  undefined4 local_168;
  undefined1 local_164 [4];
  undefined1 local_160 [4];
  undefined1 local_15c [4];
  undefined4 local_158;
  undefined4 local_154;
  void *local_150;
  char *local_14c;
  char *local_148;
  void *local_144;
  byte *local_140;
  byte *local_13c;
  char *local_138;
  char *local_134;
  undefined4 local_130;
  char *local_12c;
  char *local_128;
  char *local_124;
  char *local_120;
  char *local_11c;
  undefined4 local_118;
  char *local_114;
  int *local_110;
  uint *local_10c;
  int local_108;
  int local_104;
  int local_100;
  byte local_fa [2];
  basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_f8 [168];
  SIZE_T local_50;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_4c [28];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_30 [28];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056afc6;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  iVar3 = FUN_0044ee50(DAT_005c0028,0x14,(char **)"trophy.anm");
  if (iVar3 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571f5c);
  }
  else {
    DAT_005c685c = FUN_0052d5c0("D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\trophy.cpp:92 TrophyMsgInf"
                                ,0x80);
    local_10c = (uint *)FUN_00410aa0((undefined1 (*) [16])"trophy.txt",&local_50,0);
    FUN_0040c080(local_4c,0x1c);
    FUN_0046a830(local_4c,local_10c);
    local_8 = 0;
    puVar4 = (undefined4 *)FUN_0051b500(local_4c,local_178);
    local_124 = (char *)*puVar4;
    local_fa[0] = 0xd;
    local_114 = local_124;
    puVar4 = (undefined4 *)FUN_0051b500(local_4c,local_17c);
    local_118 = *puVar4;
    puVar4 = (undefined4 *)FUN_004fed00(local_4c,local_180);
    local_11c = (char *)*puVar4;
    uStack_1cc = 0x52e337;
    puVar4 = (undefined4 *)FUN_00515550(&local_154,local_11c,local_118,local_fa);
    local_128 = (char *)*puVar4;
    local_120 = local_128;
    FUN_0051b5e0(&local_158,local_128,local_124);
    puVar4 = (undefined4 *)FUN_0051b500(local_4c,local_15c);
    local_14c = (char *)*puVar4;
    local_fa[1] = 0xc;
    local_12c = local_14c;
    puVar4 = (undefined4 *)FUN_0051b500(local_4c,local_160);
    local_130 = *puVar4;
    puVar4 = (undefined4 *)FUN_004fed00(local_4c,local_164);
    local_134 = (char *)*puVar4;
    uStack_1cc = 0x52e3e5;
    puVar4 = (undefined4 *)FUN_00515550(&local_168,local_134,local_130,local_fa + 1);
    local_148 = (char *)*puVar4;
    local_138 = local_148;
    FUN_0051b5e0(&local_16c,local_148,local_14c);
    FUN_0040c080(local_f8,0xa8);
    FUN_005142b0(local_f8,(int)local_4c,1,1);
    local_8._0_1_ = 1;
    FUN_0040c080(local_30,0x1c);
    FUN_00447cc0((undefined4 *)local_30);
    local_8._0_1_ = 2;
    while( true ) {
      local_110 = (int *)FUN_00515500((int *)local_f8,local_30);
      bVar1 = FUN_00516420((int)local_110 + *(int *)(*local_110 + 4));
      if (!bVar1) break;
      iVar3 = FUN_0041ca90((int)local_30);
      if ((iVar3 != 0) && (pcVar5 = (char *)FUN_0049f1c0(local_30,0), *pcVar5 != '#')) {
        pcVar5 = (char *)FUN_0049f1c0(local_30,0);
        if (*pcVar5 == '\\') break;
        pcVar5 = (char *)FUN_0049f1c0(local_30,0);
        if (*pcVar5 == '@') {
          local_150 = FUN_0051cc80(local_30,local_1b4,1,0xffffffff);
          local_8._0_1_ = 3;
          local_144 = local_150;
          puVar6 = (uint *)FUN_0044baf0((int)local_150);
          FUN_0040db70(local_198,puVar6);
          local_8._0_1_ = 4;
          local_100 = FUN_0052f9b0((undefined4 *)local_198,(int *)0x0,10);
          local_8._0_1_ = 3;
          FUN_0040e5a0(local_198);
          local_8._0_1_ = 2;
          FUN_00449240(local_1b4);
          FUN_0052f590((undefined4 *)(local_100 * 0x704 + DAT_005c685c));
          *(int *)(DAT_005c685c + local_100 * 0x704) = local_100;
          FUN_00515500((int *)local_f8,local_30);
          local_13c = (byte *)(DAT_005c685c + 4 + local_100 * 0x704);
          local_170 = auStack_1d4;
          FUN_0046a780(local_30);
          FUN_0052f100(local_13c);
          for (local_104 = 0; local_104 < 2; local_104 = local_104 + 1) {
            for (local_108 = 0; local_108 < 3; local_108 = local_108 + 1) {
              do {
                FUN_00515500((int *)local_f8,local_30);
                cVar2 = FUN_00446de0(local_30,(uint *)&DAT_00570afc);
              } while (cVar2 != '\0');
              local_140 = (byte *)(local_100 * 0x704 + DAT_005c685c + 0x104 + local_104 * 0x300 +
                                  local_108 * 0x100);
              local_174 = auStack_1d4;
              FUN_0046a780(local_30);
              FUN_0052f100(local_140);
            }
          }
        }
      }
    }
    if (local_10c != (uint *)0x0) {
      FUN_0041f670(local_10c);
      local_10c = (uint *)0x0;
    }
    local_8._0_1_ = 1;
    FUN_00449240(local_30);
    local_8 = (uint)local_8._1_3_ << 8;
    std::basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
    _vbase_destructor_(local_f8);
    local_8 = 0xffffffff;
    FUN_00449240(local_4c);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

