/* Entry: 0x0050f6b0; symbol: FUN_0050f6b0; body bytes: 1198 */

void FUN_0050f6b0(undefined4 param_1,int param_2)

{
  bool bVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint *puVar6;
  char **ppcVar7;
  int *unaff_FS_OFFSET;
  int local_60;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_5c [24];
  function<void___cdecl(void)> local_44 [24];
  int local_2c [3];
  undefined4 local_20;
  int local_1c;
  int local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a9a5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  if (*(int *)(param_2 + 4) != 0) {
    puVar2 = (uint *)FUN_0041f610(0x200000);
    FUN_00543e20(puVar2,*(uint **)(param_2 + 4),0x2c);
    local_60 = 0x2c;
    local_20 = 0;
    FUN_00461510(&local_18);
    FUN_0050ae60(local_2c + 2);
    while (bVar1 = FUN_00449460(&local_18,local_2c + 2), bVar1) {
      iVar3 = FUN_0040c300(&local_18);
      local_2c[1] = 0;
      FUN_00461510(&local_1c);
      FUN_0050ae80(local_2c);
      while (bVar1 = FUN_00449460(&local_1c,local_2c), bVar1) {
        iVar4 = FUN_0040c300(&local_1c);
        if (*(short *)(param_2 + 0x10 + iVar3 * 0x45228 + iVar4 * 0x7ae8) == 0x5243) {
          *(int *)(param_2 + iVar3 * 0x45228 + 0x1c + iVar4 * 0x7ae8) = iVar3;
          *(int *)(param_2 + iVar3 * 0x45228 + 0x20 + iVar4 * 0x7ae8) = iVar4;
          iVar5 = FUN_0050fc90((void *)(param_2 + 0x10 + iVar3 * 0x45228 + iVar4 * 0x7ae8),0x7ae8);
          *(int *)(param_2 + iVar3 * 0x45228 + 0x14 + iVar4 * 0x7ae8) = iVar5;
          FUN_00543e20((uint *)((int)puVar2 + local_60),
                       (uint *)(param_2 + 0x10 + iVar3 * 0x45228 + iVar4 * 0x7ae8),0x7ae8);
          local_60 = local_60 + 0x7ae8;
        }
        FUN_00461860(&local_1c);
      }
      FUN_00461860(&local_18);
    }
    *(undefined4 *)(param_2 + 0x8a46c) = 2;
    iVar3 = FUN_0050fc90((void *)(param_2 + 0x8a460),0x7ae8);
    *(int *)(param_2 + 0x8a464) = iVar3;
    FUN_00543e20((uint *)((int)puVar2 + local_60),(uint *)(param_2 + 0x8a460),0x7ae8);
    iVar3 = FUN_0050fc90((void *)(param_2 + 0x91f48),0x1f8);
    *(int *)(param_2 + 0x91f4c) = iVar3;
    FUN_00543e20((uint *)((int)puVar2 + local_60 + 0x7ae8),(uint *)(param_2 + 0x91f48),0x1f8);
    *(int *)(*(int *)(param_2 + 4) + 0x28) = local_60 + 0x7cb4;
    puVar6 = (uint *)FUN_00539550((byte *)(puVar2 + 0xb),*(int *)(*(int *)(param_2 + 4) + 0x28),
                                  (int *)(*(int *)(param_2 + 4) + 0x24));
    *(int *)(*(int *)(param_2 + 4) + 4) = *(int *)(*(int *)(param_2 + 4) + 0x24) + 0x2c;
    FUN_004103c0(puVar6,*(uint *)(*(int *)(param_2 + 4) + 0x24),0xac,'5',0x10,
                 *(uint *)(*(int *)(param_2 + 4) + 0x24));
    FUN_00417cd0(local_44,&DAT_005b67e1);
    local_8 = 0;
    FUN_004d8620(&param_1);
    FUN_0041e790(local_44,local_5c);
    local_8 = CONCAT31(local_8._1_3_,1);
    ppcVar7 = (char **)FUN_0040ff30((undefined4 *)local_5c);
    iVar3 = FUN_00410840(ppcVar7);
    if (iVar3 == 0) {
      FUN_00411060(*(LPCVOID *)(param_2 + 4),0x2c);
      FUN_00411060(puVar6,*(DWORD *)(*(int *)(param_2 + 4) + 0x24));
      FUN_004106e0();
      if (puVar6 != (uint *)0x0) {
        FUN_0041f670(puVar6);
      }
      local_8 = local_8 & 0xffffff00;
      FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_5c);
      local_8 = 0xffffffff;
      FUN_0040e6c0(local_44);
      if (puVar2 != (uint *)0x0) {
        FUN_0041f670(puVar2);
      }
    }
    else {
      FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_005738e4);
      if (puVar6 != (uint *)0x0) {
        FUN_0041f670(puVar6);
      }
      if (puVar2 != (uint *)0x0) {
        FUN_0041f670(puVar2);
      }
      local_8 = local_8 & 0xffffff00;
      FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)local_5c);
      local_8 = 0xffffffff;
      FUN_0040e6c0(local_44);
    }
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

