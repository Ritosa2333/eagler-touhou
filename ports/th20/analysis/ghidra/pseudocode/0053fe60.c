/* Entry: 0x0053fe60; symbol: FUN_0053fe60; body bytes: 876 */

void __thiscall FUN_0053fe60(void *this,undefined4 param_1)

{
  ushort uVar1;
  undefined4 *puVar2;
  int *piVar3;
  size_t sVar4;
  char **unaff_EDI;
  undefined1 local_ac [4];
  undefined1 local_a8 [4];
  undefined1 local_a4 [4];
  undefined1 local_a0 [4];
  int *local_9c;
  code *local_98;
  int local_94;
  int *local_90;
  int *local_8c;
  int local_88;
  int *local_84;
  int *local_80;
  undefined4 *local_7c;
  char *local_78;
  undefined4 local_74;
  undefined4 local_70;
  char *local_6c;
  int *local_68;
  undefined4 local_64;
  undefined4 local_60;
  int *local_5c;
  int *local_58;
  uint local_54;
  int local_50;
  int *local_4c;
  int *local_48;
  int *local_44;
  int *local_40;
  int *local_3c;
  int *local_38;
  int *local_34;
  int *local_30;
  void *local_2c;
  int local_28;
  int local_24;
  int *local_20;
  int *local_1c;
  int *local_18;
  int local_14;
  int *local_10;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_2c = (void *)((int)this + 0xc);
  local_1c = (int *)this;
  puVar2 = (undefined4 *)FUN_00414580(local_2c,*(int *)((int)this + 4));
  *puVar2 = param_1;
  local_30 = local_1c + 3;
  puVar2 = (undefined4 *)FUN_00414580(local_30,local_1c[1]);
  if (*(int *)*puVar2 == 0x54504353) {
    local_38 = local_1c + 3;
    piVar3 = (int *)FUN_00414580(local_38,local_1c[1]);
    if (*(short *)(*piVar3 + 4) == 1) {
      local_44 = local_1c + 3;
      local_40 = local_1c + 3;
      piVar3 = (int *)FUN_00414580(local_40,local_1c[1]);
      uVar1 = *(ushort *)(*piVar3 + 6);
      piVar3 = (int *)FUN_00414580(local_44,local_1c[1]);
      local_20 = (int *)(*piVar3 + 0x24 + (uint)uVar1);
      local_48 = local_1c + 3;
      piVar3 = (int *)FUN_00414580(local_48,local_1c[1]);
      local_10 = local_20 + *(ushort *)(*piVar3 + 0x10);
      local_4c = local_1c + 3;
      piVar3 = (int *)FUN_00414580(local_4c,local_1c[1]);
      local_54 = (uint)*(ushort *)(*piVar3 + 0x10);
      local_50 = local_1c[2];
      local_1c[2] = local_50 + local_54;
      local_24 = 0;
      while( true ) {
        local_58 = local_1c + 3;
        piVar3 = (int *)FUN_00414580(local_58,local_1c[1]);
        if ((int)(uint)*(ushort *)(*piVar3 + 0x10) <= local_24) break;
        local_18 = local_10;
        local_5c = local_1c + 3;
        piVar3 = (int *)FUN_00414580(local_5c,local_1c[1]);
        local_14 = *local_20 + *piVar3;
        FUN_0040c080(&local_c,4);
        puVar2 = FUN_0040b670(local_ac,&local_10);
        local_70 = *puVar2;
        local_9c = local_1c + 0x83;
        local_60 = local_70;
        puVar2 = (undefined4 *)FUN_00470550(local_9c,local_a0);
        local_74 = *puVar2;
        local_68 = local_1c + 0x83;
        local_64 = local_74;
        puVar2 = (undefined4 *)FUN_004702b0(local_68,local_a4);
        local_78 = (char *)*puVar2;
        local_6c = local_78;
        FUN_0053fca0(&local_c,local_78);
        local_80 = local_1c + 0x83;
        local_7c = local_c;
        CStringT<>(local_80,local_a8,local_c,&local_18);
        sVar4 = _strlen((char *)local_10);
        local_88 = sVar4 + 1;
        local_84 = local_10;
        local_10 = (int *)((int)local_10 + local_88);
        local_24 = local_24 + 1;
        local_20 = local_20 + 1;
      }
      local_28 = local_1c[1];
      local_1c[1] = local_1c[1] + 1;
      local_8c = local_1c + 3;
      piVar3 = (int *)FUN_00414580(local_8c,local_28);
      if (*(short *)(*piVar3 + 6) != 0) {
        local_98 = *(code **)(*local_1c + 4);
        local_90 = local_1c + 3;
        piVar3 = (int *)FUN_00414580(local_90,local_28);
        local_94 = *piVar3 + 0x24;
        (*local_98)(local_94);
      }
    }
    else {
      std::_Adl_verify_range<char*,char_const*>
                ((char **)"error : Spt FileHeader Version error\n",unaff_EDI);
      local_3c = local_1c + 3;
      puVar2 = (undefined4 *)FUN_00414580(local_3c,local_1c[1]);
      *puVar2 = 0;
    }
  }
  else {
    std::_Adl_verify_range<char*,char_const*>
              ((char **)"error : Spt FileHeader ID error\n",unaff_EDI);
    local_34 = local_1c + 3;
    puVar2 = (undefined4 *)FUN_00414580(local_34,local_1c[1]);
    *puVar2 = 0;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

