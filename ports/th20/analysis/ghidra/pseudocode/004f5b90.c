/* Entry: 0x004f5b90; symbol: FUN_004f5b90; body bytes: 1063 */

undefined4 * __thiscall FUN_004f5b90(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var2;
  default_delete<class_std::_Facet_base> *pdVar3;
  int *unaff_FS_OFFSET;
  undefined1 *puVar4;
  undefined1 local_e4 [8];
  locale local_dc [8];
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined1 *local_90;
  undefined1 *local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined1 *local_80;
  undefined1 *local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined1 *local_70;
  int local_6c;
  undefined4 local_68;
  undefined4 local_64;
  void *local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  locale *local_4c;
  locale *local_48;
  void *local_44;
  undefined4 local_40;
  int local_3c;
  undefined4 local_38;
  undefined4 local_34;
  int *local_30;
  undefined1 local_2b;
  undefined1 local_2a;
  undefined1 local_29;
  undefined1 local_28;
  undefined1 local_27;
  char local_26;
  char local_25;
  void *local_24;
  undefined1 *puStack_20;
  int local_1c;
  undefined1 *puStack_18;
  undefined4 local_14;
  
  puStack_20 = &stack0xfffffffc;
  local_14 = 0xffffffff;
  puStack_18 = &LAB_0056a4b0;
  local_1c = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_1c;
  local_25 = **(char **)((int)this + 4);
                    /* WARNING: Load size is inaccurate */
  local_26 = **this;
  local_24 = this;
  puVar1 = (undefined4 *)move<>(&param_2);
  local_38 = *puVar1;
  local_34 = local_38;
  puVar1 = FUN_004f1b60(&local_d0,local_38,local_26,local_25);
  param_2 = *puVar1;
  if ((**(char **)((int)local_24 + 8) != '\0') &&
     (**(int **)((int)local_24 + 0xc) < **(int **)((int)local_24 + 0x10))) {
    local_27 = 0x30;
    local_3c = **(int **)((int)local_24 + 0x10) - **(int **)((int)local_24 + 0xc);
    puVar1 = (undefined4 *)move<>(&param_2);
    local_40 = *puVar1;
    puVar1 = (undefined4 *)FUN_004e6f50(&local_c4,local_40,local_3c,&local_27);
    param_2 = *puVar1;
  }
  if (*(char *)(*(int *)((int)local_24 + 0x10) + 0xc) != '\0') {
    local_44 = *(void **)((int)local_24 + 0x14);
    local_4c = FUN_004fbf20(local_44,local_dc);
    local_14 = 0;
    local_48 = local_4c;
    local_30 = (int *)FUN_004f3910(local_4c);
    local_14 = 0xffffffff;
    FUN_004f5600((int)local_dc);
    puVar1 = (undefined4 *)move<>(&param_2);
    local_64 = *puVar1;
    local_68 = **(undefined4 **)((int)local_24 + 0x24);
    local_5c = local_64;
    local_58 = local_64;
    local_54 = local_64;
    local_50 = local_64;
    local_28 = FUN_00500060(local_30);
    local_60 = *(void **)((int)local_24 + 0x20);
    puVar1 = (undefined4 *)FUN_004494f0(local_60,local_e4);
    local_b8 = *puVar1;
    local_b4 = puVar1[1];
    local_6c = **(int **)((int)local_24 + 0x1c);
    local_70 = (undefined1 *)**(undefined4 **)((int)local_24 + 0x18);
    local_b0 = local_b8;
    local_ac = local_b4;
    local_a8 = local_b8;
    local_a4 = local_b4;
    local_a0 = local_b8;
    local_9c = local_b4;
    local_98 = local_b8;
    local_94 = local_b4;
    puVar1 = (undefined4 *)FUN_004f19c0(&local_cc,local_70,local_6c);
    param_2 = *puVar1;
    if ((**(int **)((int)local_24 + 0x28) != **(int **)((int)local_24 + 0x2c)) ||
       (**(char **)((int)local_24 + 0x30) != '\0')) {
      local_29 = FUN_004fee90(local_30);
      puVar4 = &local_29;
      p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)FUN_004f57f0(&param_2,&local_c8);
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(p_Var2);
      FUN_004f5650(pdVar3,puVar4);
      **(undefined1 **)((int)local_24 + 0x30) = 0;
    }
    **(undefined4 **)((int)local_24 + 0x18) = **(undefined4 **)((int)local_24 + 0x1c);
    if (**(int **)((int)local_24 + 0x28) != **(int **)((int)local_24 + 0x2c)) {
      **(int **)((int)local_24 + 0x18) = **(int **)((int)local_24 + 0x18) + 1;
    }
  }
  puVar1 = (undefined4 *)move<>(&param_2);
  local_78 = *puVar1;
  local_7c = (undefined1 *)**(undefined4 **)((int)local_24 + 0x34);
  local_80 = (undefined1 *)**(undefined4 **)((int)local_24 + 0x18);
  local_74 = local_78;
  puVar1 = FUN_004ef4d0(&local_d4,local_80,local_7c,local_78);
  param_2 = *puVar1;
  if ((*(char *)(*(int *)((int)local_24 + 0x10) + 0xb) != '\0') &&
     (**(char **)((int)local_24 + 0x30) != '\0')) {
    local_2a = 0x2e;
    puVar4 = &local_2a;
    p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_004f57f0(&param_2,&local_bc);
    pdVar3 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var2);
    FUN_004f5650(pdVar3,puVar4);
  }
  while (0 < **(int **)((int)local_24 + 0x38)) {
    local_2b = 0x30;
    puVar4 = &local_2b;
    p_Var2 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_004f57f0(&param_2,&local_c0);
    pdVar3 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var2);
    FUN_004f5650(pdVar3,puVar4);
    **(int **)((int)local_24 + 0x38) = **(int **)((int)local_24 + 0x38) + -1;
  }
  puVar1 = (undefined4 *)move<>(&param_2);
  local_88 = *puVar1;
  local_8c = (undefined1 *)**(undefined4 **)((int)local_24 + 0x2c);
  local_90 = (undefined1 *)**(undefined4 **)((int)local_24 + 0x34);
  local_84 = local_88;
  FUN_004ef4d0(param_1,local_90,local_8c,local_88);
  *unaff_FS_OFFSET = local_1c;
  return param_1;
}

