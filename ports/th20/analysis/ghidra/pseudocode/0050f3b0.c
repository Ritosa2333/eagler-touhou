/* Entry: 0x0050f3b0; symbol: FUN_0050f3b0; body bytes: 562 */

void FUN_0050f3b0(void)

{
  char **ppcVar1;
  undefined4 uVar2;
  SIZE_T SVar3;
  int *unaff_FS_OFFSET;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_7c [24];
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *local_64;
  undefined1 (*local_60) [16];
  int local_5c;
  int local_58;
  int local_54;
  int local_50;
  SIZE_T *local_4c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_48 [24];
  function<void___cdecl(void)> local_30 [24];
  undefined4 local_18;
  char **local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a95d;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  local_14 = ppcVar1;
  uVar2 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  FUN_00417cd0(local_30,&DAT_005b67e1);
  local_8._0_1_ = 1;
  FUN_00417e40("scoreth20bak.dat");
  FUN_0041e790(local_30,local_48);
  local_8 = CONCAT31(local_8._1_3_,2);
  local_60 = (undefined1 (*) [16])FUN_0040ff30((undefined4 *)local_48);
  SVar3 = FUN_00410aa0(local_60,local_4c + 0x24850,1);
  local_4c[0x24851] = SVar3;
  FUN_0050f090((undefined2 *)(local_4c + 0x49022));
  for (local_50 = 0; local_50 < 2; local_50 = local_50 + 1) {
    for (local_54 = 0; local_54 < 10; local_54 = local_54 + 1) {
      FUN_0050ef00((undefined2 *)(local_4c + local_50 * 0x1148a + local_54 * 0x1eba + 0x24854));
    }
  }
  FUN_0050eb70((int *)(local_4c + 0x24850));
  FUN_00417df0(&DAT_005b67e1);
  FUN_00417e40("scoreth20.dat");
  local_64 = FUN_0041e790(local_30,local_7c);
  FUN_00418eb0(local_48,(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)local_64);
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_7c);
  local_60 = (undefined1 (*) [16])FUN_0040ff30((undefined4 *)local_48);
  SVar3 = FUN_00410aa0(local_60,local_4c,1);
  local_4c[1] = SVar3;
  FUN_0050f090((undefined2 *)(local_4c + 0x247d2));
  for (local_58 = 0; local_58 < 2; local_58 = local_58 + 1) {
    for (local_5c = 0; local_5c < 10; local_5c = local_5c + 1) {
      FUN_0050ef00((undefined2 *)(local_4c + local_58 * 0x1148a + local_5c * 0x1eba + 4));
    }
  }
  FUN_0050f5f0((int *)local_4c);
  std::_Adl_verify_range<char*,char_const*>((char **)"Score.LoadThread end\n",ppcVar1);
  local_8._0_1_ = 1;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_48);
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040e6c0(local_30);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee((uint)local_14 ^ (uint)&stack0xfffffffc);
  return;
}

