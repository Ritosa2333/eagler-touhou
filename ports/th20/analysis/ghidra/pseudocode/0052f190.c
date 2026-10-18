/* Entry: 0x0052f190; symbol: FUN_0052f190; body bytes: 639 */

void FUN_0052f190(void *param_1)

{
  _String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> *p_Var1;
  uint uVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  undefined4 in_stack_ffffff08;
  undefined4 in_stack_ffffff0c;
  undefined4 in_stack_ffffff10;
  undefined1 local_e8 [12];
  undefined1 local_dc [12];
  undefined4 local_d0 [2];
  undefined1 local_c8 [8];
  undefined4 local_c0 [2];
  undefined1 local_b8 [8];
  undefined1 local_b0 [8];
  undefined1 *local_a8;
  undefined1 *local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  void *local_60;
  void *local_5c;
  uint local_58;
  int local_54;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_50;
  _String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> local_4c [12];
  _String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> local_40 [12];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  uint local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 uStack_14;
  
  puStack_20 = &stack0xfffffffc;
  uStack_14 = 0xffffffff;
  puStack_18 = &LAB_0056aff0;
  local_1c = *unaff_FS_OFFSET;
  local_24 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_a4 = &stack0xffffff08;
  _String_const_iterator<>(&stack0xffffff08,(_Iterator_base12 *)&stack0x00000008);
  FUN_0052eb30(local_50,local_40,in_stack_ffffff08,in_stack_ffffff0c,in_stack_ffffff10);
  local_a8 = &stack0xffffff08;
  _String_const_iterator<>(&stack0xffffff08,(_Iterator_base12 *)&stack0x00000014);
  FUN_0052eb30(local_50,local_4c,in_stack_ffffff08,in_stack_ffffff0c,in_stack_ffffff10);
  p_Var1 = (_String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_> *)
           FUN_0052efe0(local_50,local_dc);
  local_58 = std::_String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_>
             ::operator-(local_40,p_Var1);
  local_54 = std::_String_const_iterator<class_std::_String_val<struct_std::_Simple_types<char>_>_>
             ::operator-(local_4c,local_40);
  if (local_54 == 0) {
    FUN_0052d970(param_1,(_Iterator_base12 *)local_40);
  }
  else {
    FUN_0040c080(&local_34,8);
    FUN_0052ef70(local_40,&local_34);
    FUN_0040c080(&local_2c,8);
    FUN_0052ef70(local_4c,&local_2c);
    local_5c = FUN_0052ef10(local_50,local_b0);
    uVar2 = FUN_0052de40(local_5c,(int)&local_2c);
    if (local_58 < uVar2) {
      local_70 = local_2c;
      local_6c = local_28;
      local_78 = local_34;
      local_74 = local_30;
      puVar3 = (undefined4 *)FUN_0052eec0(local_50,local_b8);
      local_80 = *puVar3;
      local_7c = puVar3[1];
      local_68 = local_80;
      local_64 = local_7c;
      FUN_0052d7d0(local_c0);
      for (; local_54 != 0; local_54 = local_54 + -1) {
        FUN_0052f670(local_50);
      }
    }
    else {
      local_90 = local_34;
      local_8c = local_30;
      puVar3 = (undefined4 *)FUN_0052ef10(local_50,local_c8);
      local_98 = *puVar3;
      local_94 = puVar3[1];
      local_a0 = local_2c;
      local_9c = local_28;
      local_88 = local_98;
      local_84 = local_94;
      FUN_0052d6e0(local_d0);
      for (; local_54 != 0; local_54 = local_54 + -1) {
        FUN_0052f5b0(local_50);
      }
    }
    local_60 = FUN_0052efe0(local_50,local_e8);
    FUN_0052de60(local_60,param_1,local_58);
  }
  *unaff_FS_OFFSET = local_1c;
  FUN_005429ee(local_24 ^ (uint)&stack0xfffffff0);
  return;
}

