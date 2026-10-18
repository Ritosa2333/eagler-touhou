/* Entry: 0x004dc1c0; symbol: FUN_004dc1c0; body bytes: 836 */

void FUN_004dc1c0(undefined4 param_1)

{
  char **ppcVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  int *unaff_FS_OFFSET;
  undefined4 local_1bc [44];
  undefined4 local_10c [44];
  undefined4 local_5c;
  undefined4 local_58;
  undefined1 (*local_54) [16];
  int local_50;
  undefined4 *local_4c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_48 [24];
  function<void___cdecl(void)> local_30 [24];
  SIZE_T local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569dc5;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar1;
  puVar2 = FUN_004b9c10(local_10c);
  puVar5 = &DAT_005c4f08;
  for (iVar4 = 0x2c; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar5 = puVar5 + 1;
  }
  FUN_00417cd0(local_30,&DAT_005b67e1);
  local_8 = 0;
  FUN_004d8620(&param_1);
  FUN_0041e790(local_30,local_48);
  local_8 = CONCAT31(local_8._1_3_,1);
  local_54 = (undefined1 (*) [16])FUN_0040ff30((undefined4 *)local_48);
  local_4c = (undefined4 *)FUN_00410aa0(local_54,&local_18,1);
  if (local_4c == (undefined4 *)0x0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571c40);
  }
  else if (local_4c[1] == 0xb0) {
    puVar2 = local_4c;
    puVar5 = &DAT_005c4f08;
    for (iVar4 = 0x2c; iVar4 != 0; iVar4 = iVar4 + -1) {
      *puVar5 = *puVar2;
      puVar2 = puVar2 + 1;
      puVar5 = puVar5 + 1;
    }
    if (local_4c != (undefined4 *)0x0) {
      FUN_0041f670(local_4c);
      local_4c = (undefined4 *)0x0;
    }
    if (((((DAT_005c4f7c < 2) && (DAT_005c4f7d < 3)) && (DAT_005c4f7e < 2)) &&
        ((DAT_005c4f80 < 10 && (DAT_005c4f84 < 3)))) &&
       ((DAT_005c4f85 < 3 && ((DAT_005c4f08 == 0x200002 && (local_18 == 0xb0))))))
    goto LAB_004dc352;
  }
  else {
    std::_Adl_verify_range<char*,char_const*>((char **)&DAT_00571c74,ppcVar1);
    if (local_4c != (undefined4 *)0x0) {
      FUN_0041f670(local_4c);
      local_4c = (undefined4 *)0x0;
    }
  }
  puVar2 = FUN_004b9c10(local_1bc);
  puVar5 = &DAT_005c4f08;
  for (iVar4 = 0x2c; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar5 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar5 = puVar5 + 1;
  }
LAB_004dc352:
  *(undefined4 *)(local_50 + 0xb34) = 0;
  uVar3 = FUN_004dc140(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571cb0);
  }
  iVar4 = FUN_00412540();
  if (iVar4 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571ccc);
  }
  iVar4 = FUN_00415800(local_50);
  if (iVar4 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571cf4);
  }
  uVar3 = FUN_0041d0a0(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571d14);
  }
  uVar3 = FUN_004dc1a0(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571d3c);
  }
  uVar3 = FUN_00428fa0(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571d74);
  }
  uVar3 = FUN_004dc160(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571d94);
    DAT_005c5874 = 1;
  }
  uVar3 = FUN_004dc120(local_50);
  if (uVar3 != 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_00571dac);
  }
  iVar4 = FUN_00410e70((char **)local_54,&DAT_005c4f08,0xb0);
  if (iVar4 == 0) {
    local_5c = 0;
    local_8 = local_8 & 0xffffff00;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_48);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_30);
  }
  else {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_00571dd0);
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_00571df0);
    local_58 = 0xffffffff;
    local_8 = local_8 & 0xffffff00;
    FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)local_48);
    local_8 = 0xffffffff;
    FUN_0040e6c0(local_30);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

