/* Entry: 0x0051f890; symbol: FUN_0051f890; body bytes: 1155 */

void FUN_0051f890(void)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  char *pcVar4;
  int *unaff_FS_OFFSET;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_190 [28];
  undefined4 local_174;
  undefined4 local_170;
  undefined1 local_16c [4];
  undefined1 local_168 [4];
  undefined1 local_164 [4];
  undefined4 local_160;
  undefined4 local_15c;
  undefined1 local_158 [4];
  undefined1 local_154 [4];
  undefined1 local_150 [4];
  char *local_14c;
  char *local_148;
  char *local_144;
  undefined4 local_140;
  void *local_13c;
  void *local_138;
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
  byte local_101;
  int local_100;
  byte local_f9;
  int local_f8;
  basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_f4 [168];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_4c [28];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_30 [28];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056af60;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_10c = (uint *)FUN_00410aa0((undefined1 (*) [16])"musiccmt.txt",(SIZE_T *)0x0,0);
  FUN_0040c080(local_4c,0x1c);
  FUN_0046a830(local_4c,local_10c);
  local_8 = 0;
  puVar2 = (undefined4 *)FUN_0051b500(local_4c,local_150);
  local_124 = (char *)*puVar2;
  local_101 = 0xd;
  local_114 = local_124;
  puVar2 = (undefined4 *)FUN_0051b500(local_4c,local_154);
  local_118 = *puVar2;
  puVar2 = (undefined4 *)FUN_004fed00(local_4c,local_158);
  local_11c = (char *)*puVar2;
  puVar2 = (undefined4 *)FUN_00515550(&local_15c,local_11c,local_118,&local_101);
  local_128 = (char *)*puVar2;
  local_120 = local_128;
  FUN_0051b5e0(&local_160,local_128,local_124);
  puVar2 = (undefined4 *)FUN_0051b500(local_4c,local_164);
  local_144 = (char *)*puVar2;
  local_f9 = 0xc;
  local_12c = local_144;
  puVar2 = (undefined4 *)FUN_0051b500(local_4c,local_168);
  local_130 = *puVar2;
  puVar2 = (undefined4 *)FUN_004fed00(local_4c,local_16c);
  local_134 = (char *)*puVar2;
  puVar2 = (undefined4 *)FUN_00515550(&local_170,local_134,local_130,&local_f9);
  local_14c = (char *)*puVar2;
  local_148 = local_14c;
  FUN_0051b5e0(&local_174,local_14c,local_144);
  FUN_0040c080(local_f4,0xa8);
  FUN_005142b0(local_f4,(int)local_4c,1,1);
  local_8._0_1_ = 1;
  FUN_0040c080(local_30,0x1c);
  FUN_00447cc0((undefined4 *)local_30);
  local_8 = CONCAT31(local_8._1_3_,2);
  local_100 = 0;
  while( true ) {
    local_110 = (int *)FUN_00515500((int *)local_f4,local_30);
    bVar1 = FUN_00516420((int)local_110 + *(int *)(*local_110 + 4));
    if (!bVar1) break;
    iVar3 = FUN_0041ca90((int)local_30);
    if ((iVar3 != 0) && (pcVar4 = (char *)FUN_0049f1c0(local_30,0), *pcVar4 != '#')) {
      pcVar4 = (char *)FUN_0049f1c0(local_30,0);
      if (*pcVar4 == '\\') break;
      pcVar4 = (char *)FUN_0049f1c0(local_30,0);
      if (*pcVar4 == '@') {
        local_138 = FUN_0051cc80(local_30,local_190,1,0xffffffff);
        iVar3 = FUN_0044baf0((int)local_138);
        FUN_00548610((char *)(local_f8 + 0x490 + local_100 * 0x40),0x40,iVar3);
        FUN_00449240(local_190);
        FUN_00515500((int *)local_f4,local_30);
        iVar3 = FUN_0044baf0((int)local_30);
        FUN_00548610((char *)(local_f8 + 0xc90 + local_100 * 0x42),0x42,iVar3);
        for (local_108 = 0; local_108 < 8; local_108 = local_108 + 1) {
          FUN_00515500((int *)local_f4,local_30);
          iVar3 = FUN_0044baf0((int)local_30);
          FUN_00548610((char *)(local_f8 + 0x14d0 + local_100 * 0x210 + local_108 * 0x42),0x42,iVar3
                      );
        }
        local_100 = local_100 + 1;
      }
    }
  }
  if (local_10c != (uint *)0x0) {
    FUN_0041f670(local_10c);
    local_10c = (uint *)0x0;
  }
  local_13c = (void *)(local_f8 + 0x24);
  FUN_0049c4e0(local_13c,local_100);
  FUN_004bed50((void *)(local_f8 + 0x24),0);
  *(undefined4 *)(local_f8 + 0x56d0) = 0;
  *(int *)(local_f8 + 0x47c) = local_100;
  FUN_0040baa0((void *)(local_f8 + 0x478),1);
  local_140 = 0;
  local_8._0_1_ = 1;
  FUN_00449240(local_30);
  local_8 = (uint)local_8._1_3_ << 8;
  std::basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
  _vbase_destructor_(local_f4);
  local_8 = 0xffffffff;
  FUN_00449240(local_4c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

