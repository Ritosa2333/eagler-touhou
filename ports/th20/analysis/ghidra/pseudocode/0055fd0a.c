/* Entry: 0x0055fd0a; symbol: FUN_0055fd0a; body bytes: 865 */

undefined4 __cdecl FUN_0055fd0a(int param_1)

{
  byte *pbVar1;
  byte bVar2;
  ushort *puVar3;
  int *piVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  int iVar19;
  int iVar20;
  int iVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  int iVar26;
  undefined4 uVar27;
  byte *pbVar28;
  undefined **ppuVar29;
  byte *pbVar30;
  undefined **ppuVar31;
  undefined **ppuVar32;
  int local_14 [2];
  undefined4 *local_c;
  undefined4 *local_8;
  
  local_14[0] = param_1;
  local_8 = (undefined4 *)0x0;
  local_14[1] = 0;
  if ((*(int *)(param_1 + 0xac) == 0) && (*(int *)(param_1 + 0xb0) == 0)) {
    local_8 = (undefined4 *)0x0;
    ppuVar29 = &PTR_DAT_005b2660;
    local_c = (undefined4 *)0x0;
LAB_00560025:
    piVar4 = *(int **)(param_1 + 0x84);
    if (piVar4 != (int *)0x0) {
      LOCK();
      *piVar4 = *piVar4 + -1;
      UNLOCK();
    }
    piVar4 = *(int **)(param_1 + 0x7c);
    if (piVar4 != (int *)0x0) {
      LOCK();
      iVar6 = *piVar4;
      *piVar4 = iVar6 + -1;
      UNLOCK();
      if (iVar6 + -1 == 0) {
        FUN_00557ba0(*(LPVOID *)(param_1 + 0x88));
        FUN_00557ba0(*(LPVOID *)(param_1 + 0x7c));
      }
    }
    *(undefined4 **)(param_1 + 0x84) = local_8;
    *(undefined4 **)(param_1 + 0x7c) = local_c;
    uVar27 = 0;
    *(undefined ***)(param_1 + 0x88) = ppuVar29;
  }
  else {
    ppuVar29 = (undefined **)FUN_00557b40(1,0x50);
    FUN_00557ba0((LPVOID)0x0);
    if (ppuVar29 != (undefined **)0x0) {
      puVar5 = (undefined4 *)FUN_00557b40(1,4);
      local_c = puVar5;
      FUN_00557ba0((LPVOID)0x0);
      if (puVar5 == (undefined4 *)0x0) {
        FUN_00557ba0(ppuVar29);
      }
      else {
        if (*(int *)(param_1 + 0xac) == 0) {
          ppuVar31 = &PTR_DAT_005b2660;
          ppuVar32 = ppuVar29;
          for (iVar6 = 0x14; iVar6 != 0; iVar6 = iVar6 + -1) {
            *ppuVar32 = *ppuVar31;
            ppuVar31 = ppuVar31 + 1;
            ppuVar32 = ppuVar32 + 1;
          }
LAB_0055ffe5:
          piVar4 = (int *)(param_1 + 0x88);
          *ppuVar29 = *(undefined **)*piVar4;
          ppuVar29[1] = *(undefined **)(*piVar4 + 4);
          ppuVar29[2] = *(undefined **)(*piVar4 + 8);
          ppuVar29[0xc] = *(undefined **)(*piVar4 + 0x30);
          ppuVar29[0xd] = *(undefined **)(*piVar4 + 0x34);
          *local_c = 1;
          if (local_8 != (undefined4 *)0x0) {
            *local_8 = 1;
          }
          goto LAB_00560025;
        }
        puVar5 = (undefined4 *)FUN_00557b40(1,4);
        local_8 = puVar5;
        FUN_00557ba0((LPVOID)0x0);
        if (puVar5 == (undefined4 *)0x0) {
          FUN_00557ba0(ppuVar29);
          FUN_00557ba0(local_c);
        }
        else {
          puVar3 = *(ushort **)(param_1 + 0xac);
          iVar6 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x15,(int *)(ppuVar29 + 3));
          iVar7 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x14,(int *)(ppuVar29 + 4));
          iVar8 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x16,(int *)(ppuVar29 + 5));
          iVar9 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x17,(int *)(ppuVar29 + 6));
          iVar10 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x18,(int *)(ppuVar29 + 7));
          iVar11 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x50,(int *)(ppuVar29 + 8));
          iVar12 = ___acrt_GetLocaleInfoA(local_14,1,puVar3,0x51,(int *)(ppuVar29 + 9));
          iVar13 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x1a,(int *)(ppuVar29 + 10));
          iVar14 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x19,(int *)((int)ppuVar29 + 0x29));
          iVar15 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x54,(int *)((int)ppuVar29 + 0x2a));
          iVar16 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x55,(int *)((int)ppuVar29 + 0x2b));
          iVar17 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x56,(int *)(ppuVar29 + 0xb));
          iVar18 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x57,(int *)((int)ppuVar29 + 0x2d));
          iVar19 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x52,(int *)((int)ppuVar29 + 0x2e));
          iVar20 = ___acrt_GetLocaleInfoA(local_14,0,puVar3,0x53,(int *)((int)ppuVar29 + 0x2f));
          iVar21 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x15,(int *)(ppuVar29 + 0xe));
          iVar22 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x14,(int *)(ppuVar29 + 0xf));
          iVar23 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x16,(int *)(ppuVar29 + 0x10));
          iVar24 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x17,(int *)(ppuVar29 + 0x11));
          iVar25 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x50,(int *)(ppuVar29 + 0x12));
          iVar26 = ___acrt_GetLocaleInfoA(local_14,2,puVar3,0x51,(int *)(ppuVar29 + 0x13));
          if (iVar26 == 0 &&
              (((((((((((((((((((iVar6 == 0 && iVar7 == 0) && iVar8 == 0) && iVar9 == 0) &&
                             iVar10 == 0) && iVar11 == 0) && iVar12 == 0) && iVar13 == 0) &&
                         iVar14 == 0) && iVar15 == 0) && iVar16 == 0) && iVar17 == 0) && iVar18 == 0
                     ) && iVar19 == 0) && iVar20 == 0) && iVar21 == 0) && iVar22 == 0) &&
                iVar23 == 0) && iVar24 == 0) && iVar25 == 0)) {
            pbVar28 = ppuVar29[7];
            while (bVar2 = *pbVar28, bVar2 != 0) {
              if ((byte)(bVar2 - 0x30) < 10) {
                *pbVar28 = bVar2 - 0x30;
LAB_0055ffbc:
                pbVar28 = pbVar28 + 1;
              }
              else {
                pbVar30 = pbVar28;
                if (bVar2 != 0x3b) goto LAB_0055ffbc;
                do {
                  pbVar1 = pbVar30 + 1;
                  *pbVar30 = *pbVar1;
                  pbVar30 = pbVar30 + 1;
                } while (*pbVar1 != 0);
              }
            }
            goto LAB_0055ffe5;
          }
          FID_conflict____acrt_locale_free_monetary((int)ppuVar29);
          FUN_00557ba0(ppuVar29);
          FUN_00557ba0(local_c);
          FUN_00557ba0(local_8);
        }
      }
    }
    uVar27 = 1;
  }
  return uVar27;
}

