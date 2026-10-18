/* Entry: 0x00519960; symbol: FUN_00519960; body bytes: 1822 */

void FUN_00519960(undefined4 param_1)

{
  bool bVar1;
  int *piVar2;
  undefined4 *puVar3;
  char *pcVar4;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  int *unaff_FS_OFFSET;
  undefined4 local_734 [176];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_474 [28];
  undefined4 local_458;
  undefined1 local_454 [4];
  undefined1 local_450 [4];
  undefined1 local_44c [4];
  undefined1 local_448 [4];
  undefined4 local_444;
  undefined4 local_440;
  undefined1 local_43c [4];
  undefined1 local_438 [4];
  undefined1 local_434 [4];
  undefined4 local_430;
  char *local_42c;
  char *local_428;
  undefined4 local_424;
  char *local_420;
  undefined1 *local_41c;
  undefined1 *local_418;
  undefined4 local_414;
  undefined4 local_410;
  void *local_40c;
  void *local_408;
  void *local_404;
  char *local_400;
  char *local_3fc;
  char *local_3f8;
  char *local_3f4;
  undefined4 local_3f0;
  char *local_3ec;
  char *local_3e8;
  char *local_3e4;
  int *local_3e0;
  undefined1 *local_3dc;
  int *local_3d8;
  int local_3d4;
  uint *local_3d0;
  int local_3cc;
  undefined1 *local_3c8;
  undefined1 local_3c4;
  undefined1 local_3c3;
  byte local_3c2 [2];
  uint local_3c0;
  void *local_3bc;
  undefined4 local_3b8 [176];
  basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_f8 [168];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_50 [28];
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_34 [28];
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056acbb;
  local_10 = *unaff_FS_OFFSET;
  local_14 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize StoneMenuInf\n",local_14);
  FUN_0051c7a0(local_3bc,param_1);
  FUN_004776a0(DAT_005c0028,param_1);
  local_3d4 = FUN_0044ee50(DAT_005c0028,0x18,(char **)"stone.anm");
  *(int *)((int)local_3bc + 0x10) = local_3d4;
  if (local_3d4 != 0) {
    piVar2 = FUN_004122c0(0x1b,FUN_0051b1d0,local_3bc);
    *(int **)((int)local_3bc + 8) = piVar2;
    piVar2 = FUN_00412360(0x37,FUN_0051b4b0,local_3bc);
    *(int **)((int)local_3bc + 0xc) = piVar2;
    local_3d0 = (uint *)FUN_00410aa0((undefined1 (*) [16])"stonetext.txt",(SIZE_T *)0x0,0);
    FUN_0040c080(local_50,0x1c);
    FUN_0046a830(local_50,local_3d0);
    local_8 = 0;
    puVar3 = (undefined4 *)FUN_0051b500(local_50,local_44c);
    local_420 = (char *)*puVar3;
    local_3c2[1] = 0xd;
    local_3e4 = local_420;
    puVar3 = (undefined4 *)FUN_0051b500(local_50,local_450);
    local_424 = *puVar3;
    puVar3 = (undefined4 *)FUN_004fed00(local_50,local_454);
    local_428 = (char *)*puVar3;
    puVar3 = (undefined4 *)FUN_00515550(&local_458,local_428,local_424,local_3c2 + 1);
    local_42c = (char *)*puVar3;
    local_3e8 = local_42c;
    FUN_0051b5e0(&local_430,local_42c,local_3e4);
    puVar3 = (undefined4 *)FUN_0051b500(local_50,local_434);
    local_3fc = (char *)*puVar3;
    local_3c2[0] = 0xc;
    local_3ec = local_3fc;
    puVar3 = (undefined4 *)FUN_0051b500(local_50,local_438);
    local_3f0 = *puVar3;
    puVar3 = (undefined4 *)FUN_004fed00(local_50,local_43c);
    local_3f4 = (char *)*puVar3;
    puVar3 = (undefined4 *)FUN_00515550(&local_440,local_3f4,local_3f0,local_3c2);
    local_400 = (char *)*puVar3;
    local_3f8 = local_400;
    FUN_0051b5e0(&local_444,local_400,local_3fc);
    FUN_0040c080(local_f8,0xa8);
    FUN_005142b0(local_f8,(int)local_50,1,1);
    local_8._0_1_ = 1;
    FUN_0040c080(local_34,0x1c);
    FUN_00447cc0((undefined4 *)local_34);
    local_8 = CONCAT31(local_8._1_3_,2);
LAB_00519c17:
    do {
      local_3d8 = (int *)FUN_00515500((int *)local_f8,local_34);
      bVar1 = FUN_00516420((int)local_3d8 + *(int *)(*local_3d8 + 4));
      if ((!bVar1) || (pcVar4 = (char *)FUN_0049f1c0(local_34,0), *pcVar4 == '\\'))
      goto LAB_0051a01d;
      pcVar4 = (char *)FUN_0049f1c0(local_34,0);
      if (*pcVar4 == '@') {
        do {
          puVar3 = FUN_00515690(local_734,&PTR_s_REIMU_MAIN_RED_005742e0);
          puVar7 = local_3b8;
          for (iVar5 = 0xb0; iVar5 != 0; iVar5 = iVar5 + -1) {
            *puVar7 = *puVar3;
            puVar3 = puVar3 + 1;
            puVar7 = puVar7 + 1;
          }
          FUN_0040c080(&local_18,4);
          local_3c3 = 0;
          local_3c4 = 0;
          local_40c = FUN_0051cc80(local_34,local_474,1,0xffffffff);
          local_8._0_1_ = 3;
          local_408 = local_40c;
          local_404 = local_40c;
          FUN_00514490(&local_18,local_3b8,local_40c);
          local_8 = CONCAT31(local_8._1_3_,2);
          FUN_00449240(local_474);
          local_410 = local_18;
          puVar3 = (undefined4 *)begin(local_3b8,local_448);
          local_414 = *puVar3;
          local_3c0 = FUN_005152e0();
          uVar6 = local_3c0 & 0x80000003;
          if ((int)uVar6 < 0) {
            uVar6 = (uVar6 - 1 | 0xfffffffc) + 1;
          }
          if ((uVar6 == 0) && ((int)local_3c0 < 0x48)) {
            FUN_00515500((int *)local_f8,local_34);
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + local_3c0 * 0x100 + 0xf4),0x100,iVar5);
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + (local_3c0 + 1) * 0x100 + 0xf4),0x100,iVar5);
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + (local_3c0 + 2) * 0x100 + 0xf4),0x100,iVar5);
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + (local_3c0 + 3) * 0x100 + 0xf4),0x100,iVar5);
          }
          if (0x47 < (int)local_3c0) {
            FUN_00515500((int *)local_f8,local_34);
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + local_3c0 * 0x100 + 0xf4),0x100,iVar5);
          }
          local_3dc = (undefined1 *)((int)local_3bc + local_3c0 * 0x500 + 0x58f4);
          local_418 = local_3dc + 0x500;
          for (local_3c8 = local_3dc; local_3c8 != local_418; local_3c8 = local_3c8 + 0x100) {
            local_41c = local_3c8;
            *local_3c8 = 0;
          }
          local_3cc = 0;
          while( true ) {
            do {
              local_3e0 = (int *)FUN_00515500((int *)local_f8,local_34);
              bVar1 = FUN_00516420((int)local_3e0 + *(int *)(*local_3e0 + 4));
              if (!bVar1) goto LAB_00519c17;
              iVar5 = FUN_0041ca90((int)local_34);
            } while ((iVar5 == 0) || (pcVar4 = (char *)FUN_0049f1c0(local_34,0), *pcVar4 == '#'));
            pcVar4 = (char *)FUN_0049f1c0(local_34,0);
            if (*pcVar4 == '\\') goto LAB_0051a01d;
            pcVar4 = (char *)FUN_0049f1c0(local_34,0);
            if (*pcVar4 == '@') break;
            iVar5 = FUN_0044baf0((int)local_34);
            FUN_00548610((char *)((int)local_3bc + local_3cc * 0x100 + local_3c0 * 0x500 + 0x58f4),
                         0x100,iVar5);
            local_3cc = local_3cc + 1;
            if (4 < local_3cc) goto LAB_00519c17;
          }
        } while( true );
      }
    } while( true );
  }
  FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
LAB_0051a06f:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
LAB_0051a01d:
  if (local_3d0 != (uint *)0x0) {
    FUN_0041f670(local_3d0);
    local_3d0 = (uint *)0x0;
  }
  local_8._0_1_ = 1;
  FUN_00449240(local_34);
  local_8 = (uint)local_8._1_3_ << 8;
  std::basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
  _vbase_destructor_(local_f8);
  local_8 = 0xffffffff;
  FUN_00449240(local_50);
  goto LAB_0051a06f;
}

