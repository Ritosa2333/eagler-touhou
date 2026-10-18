/* Entry: 0x004f9520; symbol: FUN_004f9520; body bytes: 1620 */

void FUN_004f9520(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined1 *puVar3;
  undefined1 (*pauVar4) [16];
  int *piVar5;
  int *unaff_FS_OFFSET;
  char cVar6;
  undefined4 local_e4;
  undefined1 local_e0 [4];
  undefined4 local_dc;
  undefined1 local_d8 [4];
  void *local_d4;
  char *local_d0;
  undefined4 local_cc;
  char *local_c8;
  undefined4 local_c4;
  char *local_c0;
  undefined4 local_bc;
  char *local_b4;
  undefined4 local_b0;
  char *local_ac;
  undefined4 local_a8;
  GpImage *local_a4;
  default_delete<class_std::_Facet_base> *local_a0;
  default_delete<class_std::_Facet_base> *local_9c;
  void *local_98;
  undefined4 local_94;
  void *local_90;
  int local_8c;
  GpImage *local_88;
  undefined4 local_84;
  undefined4 *local_80;
  undefined4 local_7c;
  undefined4 local_78;
  char **local_74;
  undefined4 *local_70;
  undefined4 local_6c;
  undefined4 local_68;
  Image *local_64;
  default_delete<class_std::_Facet_base> *local_60;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_5c;
  default_delete<class_std::_Facet_base> *local_58;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_54;
  int local_50;
  default_delete<class_std::_Facet_base> *local_4c;
  default_delete<class_std::_Facet_base> *local_48;
  GpImage *local_44;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_40 [28];
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a50d;
  local_1c = *unaff_FS_OFFSET;
  local_24 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_64 = (Image *)FUN_0040bbc0(&DAT_005ba568,param_1);
  Gdiplus::Image::SetNativeImage(local_64,local_44);
  FUN_004ffe40(local_44,param_1);
  FUN_004776a0(DAT_005c0028,param_1);
  FUN_0040c080(local_40,0x1c);
  FUN_00447cc0((undefined4 *)local_40);
  local_14 = 0;
  local_ac = "pl00.sht";
  local_a8 = 8;
  cVar6 = '\0';
  iVar1 = FUN_004f3500();
  std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::resize
            (local_40,iVar1 + 1,cVar6);
  iVar1 = FUN_0041caf0(*(int *)(local_44 + 0x14858));
  local_68 = FUN_00412730(iVar1);
  local_b4 = "pl{:02}.anm";
  local_b0 = 0xb;
  local_c0 = "pl{:02}.anm";
  local_bc = 0xb;
  puVar2 = (undefined4 *)FUN_004fed00(local_40,local_e0);
  local_6c = *puVar2;
  local_70 = FUN_004f3460(&local_e4);
  puVar3 = (undefined1 *)FUN_004f5770(local_70,0);
  *puVar3 = 0;
  local_74 = (char **)FUN_0044baf0((int)local_40);
  local_50 = FUN_0044eb30(DAT_005c0028,param_1 + 9,local_74);
  *(int *)(local_44 + 0x1c) = local_50;
  if (local_50 != 0) {
    iVar1 = FUN_0041caf0(*(int *)(local_44 + 0x14858));
    local_78 = FUN_00412730(iVar1);
    local_c8 = "pl{:02}.sht";
    local_c4 = 0xb;
    local_d0 = "pl{:02}.sht";
    local_cc = 0xb;
    puVar2 = (undefined4 *)FUN_004fed00(local_40,local_d8);
    local_7c = *puVar2;
    local_80 = FUN_004f3460(&local_dc);
    puVar3 = (undefined1 *)FUN_004f5770(local_80,0);
    *puVar3 = 0;
    pauVar4 = (undefined1 (*) [16])FUN_0044baf0((int)local_40);
    iVar1 = FUN_004f9b80((int *)(local_44 + 0x2208),pauVar4);
    if (iVar1 == 0) {
      local_14 = 0xffffffff;
      FUN_00449240((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_40);
      FUN_0041df50(local_44,*(undefined4 *)(local_44 + 0x1c));
      if (DAT_005c60f0 == 0) {
        local_88 = local_44 + 0x22b4;
        FUN_00504bd0(local_88,*(undefined4 *)(local_44 + 0x14854));
      }
      else {
        *(int *)(local_44 + 0x2208) = DAT_005c60f0;
        DAT_005c60f0 = 0;
      }
      piVar5 = FUN_00412310(0x1d,FUN_004feda0,local_44);
      *(int **)(local_44 + 8) = piVar5;
      piVar5 = FUN_004123b0(0x1e,FUN_004fefb0,local_44);
      *(int **)(local_44 + 0xc) = piVar5;
      *(undefined4 *)(local_44 + 0x22a0) = 0;
      *(undefined4 *)(local_44 + 0x22a4) = 1;
      *(undefined4 *)(local_44 + 0x22a8) = 2;
      *(undefined4 *)(local_44 + 0x22ac) = 3;
      *(undefined4 *)(local_44 + 0x22b0) = 4;
      local_d4 = *(void **)(local_44 + 0x1c);
      FUN_00438380(local_d4,local_44 + 0x28,*(int *)(local_44 + 0x22a0));
      FUN_004ffd80(local_44,0.0,DAT_00572660);
      *(int *)(local_44 + 0x20b4) =
           (int)(*(float *)(*(int *)(local_44 + 0x2208) + 0x10) * DAT_0056cd98);
      *(int *)(local_44 + 0x20b8) =
           (int)(*(float *)(*(int *)(local_44 + 0x2208) + 0x14) * DAT_0056cd98);
      *(int *)(local_44 + 0x20bc) =
           (int)(*(float *)(*(int *)(local_44 + 0x2208) + 0x18) * DAT_0056cd98);
      *(int *)(local_44 + 0x20c0) =
           (int)(*(float *)(*(int *)(local_44 + 0x2208) + 0x1c) * DAT_0056cd98);
      *(undefined4 *)(*(int *)(local_44 + 0x2208) + 0x24) = 100;
      local_90 = (void *)FUN_0041caf0(*(int *)(local_44 + 0x14858));
      local_8c = *(int *)(*(int *)(local_44 + 0x2208) + 0x20) *
                 *(int *)(*(int *)(local_44 + 0x2208) + 0x24);
      FUN_004be140(local_90,local_8c);
      local_98 = (void *)FUN_0041caf0(*(int *)(local_44 + 0x14858));
      local_94 = *(undefined4 *)(*(int *)(local_44 + 0x2208) + 0x24);
      FUN_004be0f0(local_98,local_94);
      FUN_00423520(local_44 + 0x644,0);
      FUN_00423520(local_44 + 0x2050,0);
      FUN_00423520(local_44 + 0x2060,0);
      *(undefined4 *)(local_44 + 0x20e4) = 0x1e;
      *(undefined4 *)(local_44 + 0x680) = 0;
      *(undefined4 *)(local_44 + 0x20ec) = DAT_0056c8cc;
      local_54 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)(local_44 + 0x684);
      local_48 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first(local_54);
      local_9c = (default_delete<class_std::_Facet_base> *)FUN_004fce60((int)local_54);
      for (; local_48 != local_9c; local_48 = local_48 + 300) {
        local_58 = local_48;
        *(undefined4 *)(local_48 + 0x7c) = 0xffff3800;
        FUN_004ffe80(local_48,*(undefined4 *)(local_44 + 0x14854));
      }
      local_5c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)(local_44 + 0x123c);
      local_4c = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first(local_5c);
      local_a0 = (default_delete<class_std::_Facet_base> *)FUN_004fce80((int)local_5c);
      for (; local_4c != local_a0; local_4c = local_4c + 300) {
        local_60 = local_4c;
        *(undefined4 *)(local_4c + 0x7c) = 0xffff3800;
        FUN_004ffe80(local_4c,*(undefined4 *)(local_44 + 0x14854));
      }
      FUN_004790c0((int)local_44);
      FUN_00429970((int)(local_44 + 0x220c));
      *(undefined4 *)(local_44 + 0x2238) = DAT_0056c8cc;
      local_a4 = local_44 + 0x2244;
      FUN_004f9470(local_a4,param_1);
      goto LAB_004f9b56;
    }
  }
  FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
  local_84 = 0xffffffff;
  local_14 = 0xffffffff;
  FUN_00449240((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_40);
LAB_004f9b56:
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

