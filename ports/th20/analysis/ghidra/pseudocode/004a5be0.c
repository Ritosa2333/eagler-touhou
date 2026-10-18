/* Entry: 0x004a5be0; symbol: FUN_004a5be0; body bytes: 516 */

undefined4 __thiscall FUN_004a5be0(void *this,int *param_1)

{
  char **ppcVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  size_t sVar5;
  uint uVar6;
  undefined4 uVar7;
  char *local_1c;
  uint local_c;
  char **local_8;
  
  if (*param_1 == 0x4d494e41) {
    local_8 = (char **)(param_1 + 2);
    for (local_c = 0; local_c < (uint)param_1[1]; local_c = local_c + 1) {
      iVar4 = *(int *)((int)this + 0x234);
      pvVar2 = (void *)FUN_00412730(*(int *)((int)this + 0x238));
      uVar6 = local_c + 2;
      uVar7 = 0x4a5c62;
      iVar3 = FUN_004aae10(pvVar2,uVar6);
      if (iVar3 == 0) {
        FUN_0040ddd0(&stack0xffffffc0,(char *)local_8);
        iVar3 = FUN_004aaeb0(DAT_005c0028,uVar7,uVar6);
        if (iVar3 == 0) {
          pvVar2 = (void *)FUN_00412730(*(int *)((int)this + 0x238));
          iVar4 = FUN_0044eb30(DAT_005c0028,iVar4 + local_c * 2 + 0x19,local_8);
          iVar4 = FUN_004ab880(pvVar2,local_c + 2,iVar4);
          if (iVar4 == 0) {
            FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
            return 0xffffffff;
          }
        }
        else {
          pvVar2 = (void *)FUN_00412730(*(int *)((int)this + 0x238));
          FUN_004ab880(pvVar2,local_c + 2,iVar3);
          std::_Adl_verify_range<char*,char_const*>((char **)"%s is skiped\n",local_8);
        }
      }
      sVar5 = _strlen((char *)local_8);
      local_8 = (char **)((int)local_8 + sVar5 + 1);
    }
    uVar6 = (int)local_8 - (int)param_1 & 0x80000003;
    if ((int)uVar6 < 0) {
      uVar6 = (uVar6 - 1 | 0xfffffffc) + 1;
    }
    if (uVar6 != 0) {
      uVar6 = (int)local_8 - (int)param_1 & 0x80000003;
      if ((int)uVar6 < 0) {
        uVar6 = (uVar6 - 1 | 0xfffffffc) + 1;
      }
      local_8 = (char **)((4 - uVar6) + (int)local_8);
    }
    ppcVar1 = local_8;
    if (*local_8 == (char *)0x494c4345) {
      local_8 = local_8 + 2;
      for (local_1c = (char *)0x0; local_1c < ppcVar1[1]; local_1c = local_1c + 1) {
                    /* WARNING: Load size is inaccurate */
        (*(code *)**this)();
        sVar5 = _strlen((char *)local_8);
        local_8 = (char **)((int)local_8 + sVar5 + 1);
      }
    }
  }
  return 0;
}

