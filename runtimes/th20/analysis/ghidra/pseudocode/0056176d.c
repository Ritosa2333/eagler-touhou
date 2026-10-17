/* Entry: 0x0056176d; symbol: FUN_0056176d; body bytes: 596 */

void FUN_0056176d(ushort *param_1)

{
  short *psVar1;
  short sVar2;
  uint *puVar3;
  uint uVar4;
  bool bVar5;
  int iVar6;
  int iVar7;
  LCID Locale;
  LCTYPE LVar8;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  short *psVar9;
  uint extraout_EDX;
  WCHAR local_f8 [120];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar6 = FUN_005579c2();
  iVar7 = FUN_005579c2();
  puVar3 = *(uint **)(iVar7 + 0x34c);
  Locale = FUN_00561a92(param_1);
  LVar8 = 0x1002;
  if (*(int *)(iVar6 + 100) != 0) {
    LVar8 = 7;
  }
  iVar7 = GetLocaleInfoW(Locale,LVar8,local_f8,0x78);
  if (iVar7 == 0) {
LAB_005619ab:
    *puVar3 = 0;
    goto LAB_005619b0;
  }
  iVar7 = FUN_0055df10(*(ushort **)(iVar6 + 0x54),(ushort *)local_f8);
  if (iVar7 == 0) {
    LVar8 = 0x1001;
    if (*(int *)(iVar6 + 0x60) != 0) {
      LVar8 = 3;
    }
    iVar7 = GetLocaleInfoW(Locale,LVar8,local_f8,0x78);
    if (iVar7 == 0) goto LAB_005619ab;
    iVar7 = FUN_0055df10(*(ushort **)(iVar6 + 0x50),(ushort *)local_f8);
    if (iVar7 == 0) {
      puVar3[1] = Locale;
      *puVar3 = *puVar3 | 0x304;
LAB_005618a3:
      puVar3[2] = Locale;
    }
    else if ((*puVar3 & 2) == 0) {
      if ((*(int *)(iVar6 + 0x5c) == 0) ||
         (iVar7 = FUN_00565d40(*(ushort **)(iVar6 + 0x50),(ushort *)local_f8,*(int *)(iVar6 + 0x5c))
         , iVar7 != 0)) {
        if (((*puVar3 & 1) == 0) && (iVar7 = _TestDefaultCountry((short)Locale), iVar7 != 0)) {
          *puVar3 = extraout_EDX | 1;
          goto LAB_005618a3;
        }
      }
      else {
        *puVar3 = *puVar3 | 2;
        puVar3[2] = Locale;
        psVar9 = *(short **)(iVar6 + 0x50);
        psVar1 = psVar9 + 1;
        do {
          sVar2 = *psVar9;
          psVar9 = psVar9 + 1;
        } while (sVar2 != 0);
        if ((int)psVar9 - (int)psVar1 >> 1 == *(int *)(iVar6 + 0x5c)) {
          puVar3[1] = Locale;
        }
      }
    }
  }
  if ((*puVar3 & 0x300) == 0x300) goto LAB_005619b0;
  LVar8 = 0x1001;
  if (*(int *)(iVar6 + 0x60) != 0) {
    LVar8 = 3;
  }
  iVar7 = GetLocaleInfoW(Locale,LVar8,local_f8,0x78);
  if (iVar7 == 0) goto LAB_005619ab;
  iVar7 = FUN_0055df10(*(ushort **)(iVar6 + 0x50),(ushort *)local_f8);
  if (iVar7 == 0) {
    uVar4 = *puVar3;
    *puVar3 = uVar4 | 0x200;
    if ((*(int *)(iVar6 + 0x60) == 0) && (*(int *)(iVar6 + 0x5c) != 0)) {
      psVar9 = *(short **)(iVar6 + 0x50);
      psVar1 = psVar9 + 1;
      do {
        sVar2 = *psVar9;
        psVar9 = psVar9 + 1;
      } while (sVar2 != 0);
      if ((int)psVar9 - (int)psVar1 >> 1 != *(int *)(iVar6 + 0x5c)) goto LAB_00561904;
      bVar5 = FUN_00561bec(Locale,1);
      if (CONCAT31(extraout_var,bVar5) == 0) goto LAB_005619b0;
      *puVar3 = *puVar3 | 0x100;
    }
    else {
LAB_00561904:
      *puVar3 = uVar4 | 0x300;
    }
    uVar4 = puVar3[1];
  }
  else {
    if ((((*(int *)(iVar6 + 0x60) != 0) || (*(int *)(iVar6 + 0x5c) == 0)) ||
        (iVar6 = FUN_0055df10(*(ushort **)(iVar6 + 0x50),(ushort *)local_f8), iVar6 != 0)) ||
       (bVar5 = FUN_00561bec(Locale,0), CONCAT31(extraout_var_00,bVar5) == 0)) goto LAB_005619b0;
    *puVar3 = *puVar3 | 0x100;
    uVar4 = puVar3[1];
  }
  if (uVar4 == 0) {
    puVar3[1] = Locale;
  }
LAB_005619b0:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

