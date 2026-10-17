/* Entry: 0x005600d4; symbol: FUN_005600d4; body bytes: 541 */

undefined4 __cdecl FUN_005600d4(int param_1)

{
  byte *pbVar1;
  byte bVar2;
  ushort *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  byte *pbVar9;
  undefined **ppuVar10;
  int *piVar11;
  byte *pbVar12;
  undefined4 *puVar13;
  undefined **ppuVar14;
  int local_1c [3];
  undefined4 *local_10;
  undefined4 *local_c;
  undefined4 *local_8;
  
  local_1c[0] = param_1;
  local_1c[1] = 0;
  if ((*(int *)(param_1 + 0xb0) == 0) && (*(int *)(param_1 + 0xac) == 0)) {
    local_8 = (undefined4 *)0x0;
    ppuVar10 = &PTR_DAT_005b2660;
  }
  else {
    local_1c[2] = 1;
    ppuVar10 = (undefined **)FUN_00557b40(1,0x50);
    if (ppuVar10 == (undefined **)0x0) {
      return 1;
    }
    local_c = (undefined4 *)(param_1 + 0x88);
    piVar11 = (int *)*local_c;
    ppuVar14 = ppuVar10;
    for (iVar8 = 0x14; iVar8 != 0; iVar8 = iVar8 + -1) {
      *ppuVar14 = (undefined *)*piVar11;
      piVar11 = piVar11 + 1;
      ppuVar14 = ppuVar14 + 1;
    }
    local_8 = (undefined4 *)FUN_005584c0(4);
    FUN_00557ba0((LPVOID)0x0);
    if (local_8 == (undefined4 *)0x0) {
      FUN_00557ba0(ppuVar10);
      return 1;
    }
    *local_8 = 0;
    if (*(int *)(param_1 + 0xb0) != 0) {
      local_10 = (undefined4 *)FUN_005584c0(4);
      FUN_00557ba0((LPVOID)0x0);
      if (local_10 == (undefined4 *)0x0) {
        FUN_00557ba0(ppuVar10);
LAB_00560222:
        FUN_00557ba0(local_8);
        return local_1c[2];
      }
      *local_10 = 0;
      puVar3 = *(ushort **)(param_1 + 0xb0);
      iVar8 = ___acrt_GetLocaleInfoA(local_1c,1,puVar3,0xe,(int *)ppuVar10);
      iVar4 = ___acrt_GetLocaleInfoA(local_1c,1,puVar3,0xf,(int *)(ppuVar10 + 1));
      iVar5 = ___acrt_GetLocaleInfoA(local_1c,1,puVar3,0x10,(int *)(ppuVar10 + 2));
      iVar6 = ___acrt_GetLocaleInfoA(local_1c,2,puVar3,0xe,(int *)(ppuVar10 + 0xc));
      iVar7 = ___acrt_GetLocaleInfoA(local_1c,2,puVar3,0xf,(int *)(ppuVar10 + 0xd));
      if (iVar7 != 0 || (((iVar8 != 0 || iVar4 != 0) || iVar5 != 0) || iVar6 != 0)) {
        FID_conflict____free_lconv_num(ppuVar10);
        FUN_00557ba0(ppuVar10);
        FUN_00557ba0(local_10);
        local_1c[2] = 0xffffffff;
        goto LAB_00560222;
      }
      pbVar9 = ppuVar10[2];
      while (bVar2 = *pbVar9, bVar2 != 0) {
        if ((byte)(bVar2 - 0x30) < 10) {
          *pbVar9 = bVar2 - 0x30;
LAB_00560243:
          pbVar9 = pbVar9 + 1;
        }
        else {
          pbVar12 = pbVar9;
          if (bVar2 != 0x3b) goto LAB_00560243;
          do {
            pbVar1 = pbVar12 + 1;
            *pbVar12 = *pbVar1;
            pbVar12 = pbVar12 + 1;
          } while (*pbVar1 != 0);
        }
      }
      *local_8 = 1;
      *local_10 = 1;
      puVar13 = local_10;
      goto LAB_005602a8;
    }
    *ppuVar10 = PTR_DAT_005b2660;
    ppuVar10[1] = PTR_DAT_005b2664;
    ppuVar10[2] = PTR_DAT_005b2668;
    ppuVar10[0xc] = PTR_DAT_005b2690;
    ppuVar10[0xd] = PTR_DAT_005b2694;
    *local_8 = 1;
  }
  local_c = (undefined4 *)(param_1 + 0x88);
  puVar13 = (undefined4 *)0x0;
LAB_005602a8:
  piVar11 = *(int **)(param_1 + 0x80);
  if (piVar11 != (int *)0x0) {
    LOCK();
    *piVar11 = *piVar11 + -1;
    UNLOCK();
  }
  piVar11 = *(int **)(param_1 + 0x7c);
  if (piVar11 != (int *)0x0) {
    LOCK();
    iVar8 = *piVar11;
    *piVar11 = iVar8 + -1;
    UNLOCK();
    if (iVar8 + -1 == 0) {
      FUN_00557ba0(*(LPVOID *)(param_1 + 0x7c));
      FUN_00557ba0((LPVOID)*local_c);
    }
  }
  *(undefined4 **)(param_1 + 0x7c) = local_8;
  *(undefined4 **)(param_1 + 0x80) = puVar13;
  *local_c = ppuVar10;
  return 0;
}

