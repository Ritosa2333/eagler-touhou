/* Entry: 0x00549a8e; symbol: FUN_00549a8e; body bytes: 1082 */

void __fastcall FUN_00549a8e(__uint64 *param_1)

{
  int *piVar1;
  int iVar2;
  bool bVar3;
  bool bVar4;
  ulonglong uVar5;
  char cVar6;
  undefined1 uVar7;
  undefined4 uVar8;
  uint uVar9;
  __uint64 *p_Var10;
  WCHAR *pWVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int *unaff_FS_OFFSET;
  byte bVar15;
  undefined8 local_2c;
  undefined4 local_24;
  __uint64 *local_20;
  __uint64 *local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056b28f;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  cVar6 = *(char *)((int)param_1 + 0x2d);
  local_1c[0] = param_1;
  if (cVar6 < 'e') {
    if (cVar6 == 'd') {
LAB_00549b62:
      *(uint *)((int)param_1 + 0x1c) = *(uint *)((int)param_1 + 0x1c) | 0x10;
LAB_00549b66:
      uVar9 = FUN_00548e24(param_1,0);
      cVar6 = (char)uVar9;
    }
    else if (cVar6 < 'T') {
      if (cVar6 == 'S') {
LAB_00549b8f:
        uVar8 = FUN_0054a28f((undefined4 *)param_1);
        cVar6 = (char)uVar8;
      }
      else {
        if (cVar6 != 'A') {
          if (cVar6 == 'C') {
LAB_00549b1c:
            uVar8 = FUN_0054a14b((undefined6 *)param_1);
            cVar6 = (char)uVar8;
            goto LAB_00549b9f;
          }
          if (((cVar6 != 'E') && (cVar6 != 'F')) && (cVar6 != 'G')) goto LAB_00549eae;
        }
LAB_00549afa:
        uVar8 = FUN_00549f94((uint *)param_1);
        cVar6 = (char)uVar8;
      }
    }
    else if (cVar6 == 'X') {
      bVar15 = 1;
LAB_00549b31:
      uVar9 = FUN_00548fb1(param_1,bVar15);
      cVar6 = (char)uVar9;
    }
    else {
      if (cVar6 != 'Z') {
        if (cVar6 != 'a') {
          if (cVar6 != 'c') goto LAB_00549eae;
          goto LAB_00549b1c;
        }
        goto LAB_00549afa;
      }
      uVar8 = FUN_00549f21(param_1);
      cVar6 = (char)uVar8;
    }
  }
  else if (cVar6 < 'p') {
    if (cVar6 == 'o') {
      cVar6 = FUN_0054a25c(param_1);
    }
    else {
      if (((cVar6 == 'e') || (cVar6 == 'f')) || (cVar6 == 'g')) goto LAB_00549afa;
      if (cVar6 == 'i') goto LAB_00549b62;
      if (cVar6 != 'n') goto LAB_00549eae;
      uVar9 = FUN_0054a1d7((int)param_1);
      cVar6 = (char)uVar9;
    }
  }
  else {
    if (cVar6 != 'p') {
      if (cVar6 == 's') goto LAB_00549b8f;
      if (cVar6 != 'u') {
        if (cVar6 != 'x') goto LAB_00549eae;
        bVar15 = 0;
        goto LAB_00549b31;
      }
      goto LAB_00549b66;
    }
    cVar6 = FUN_0054a279(param_1);
  }
LAB_00549b9f:
  if ((cVar6 == '\0') || (*(char *)((int)param_1 + 0x2c) != '\0')) goto LAB_00549eae;
  p_Var10 = (__uint64 *)0x0;
  local_24 = (WCHAR *)((uint)local_24 & 0xff000000);
  local_20 = (__uint64 *)0x0;
  if ((*(uint *)((int)param_1 + 0x1c) >> 4 & 1) != 0) {
    if ((*(uint *)((int)param_1 + 0x1c) >> 6 & 1) == 0) {
      if ((*(byte *)((int)param_1 + 0x1c) & 1) == 0) {
        if ((*(uint *)((int)param_1 + 0x1c) >> 1 & 1) != 0) {
          local_24 = (WCHAR *)CONCAT31(local_24._1_3_,0x20);
          p_Var10 = (__uint64 *)0x1;
          local_20 = (__uint64 *)0x1;
        }
        goto LAB_00549c00;
      }
      local_24 = (WCHAR *)CONCAT31(local_24._1_3_,0x2b);
    }
    else {
      local_24 = (WCHAR *)CONCAT31(local_24._1_3_,0x2d);
    }
    local_20 = (__uint64 *)0x1;
    p_Var10 = (__uint64 *)0x1;
  }
LAB_00549c00:
  cVar6 = *(char *)((int)param_1 + 0x2d);
  if (((cVar6 != 'x') && (cVar6 != 'X')) ||
     (bVar3 = true, (*(uint *)((int)param_1 + 0x1c) >> 5 & 1) == 0)) {
    bVar3 = false;
  }
  if ((cVar6 == 'a') || (bVar4 = false, cVar6 == 'A')) {
    bVar4 = true;
  }
  if ((bVar3) || (bVar4)) {
    *(undefined1 *)((int)&local_24 + (int)p_Var10) = 0x30;
    if ((cVar6 == 'X') || (cVar6 == 'A')) {
      uVar7 = 0x58;
    }
    else {
      uVar7 = 0x78;
    }
    *(undefined1 *)((int)&local_24 + (int)p_Var10 + 1U) = uVar7;
    p_Var10 = (__uint64 *)((int)p_Var10 + 2);
    local_20 = p_Var10;
  }
  iVar12 = ((int)param_1[4] - *(int *)((int)param_1 + 0x34)) - (int)p_Var10;
  uVar5 = local_2c;
  if (((*(byte *)((int)param_1 + 0x1c) & 0xc) == 0) &&
     (local_2c = local_2c & 0xffffffff, uVar5 = local_2c, 0 < iVar12)) {
    iVar13 = *(int *)((int)param_1 + 0x14);
    local_2c._4_4_ = 0;
    do {
      iVar14 = (int)param_1[0x89];
      if (*(int *)(iVar14 + 8) == *(int *)(iVar14 + 4)) {
        if (*(char *)(iVar14 + 0xc) == '\0') {
          iVar13 = -1;
        }
        else {
          iVar13 = iVar13 + 1;
        }
        *(int *)((int)param_1 + 0x14) = iVar13;
      }
      else {
        *(int *)((int)param_1 + 0x14) = iVar13 + 1;
        *(int *)(iVar14 + 8) = *(int *)(iVar14 + 8) + 1;
        *(undefined1 *)**(undefined4 **)(param_1 + 0x89) = 0x20;
        **(int **)(param_1 + 0x89) = **(int **)(param_1 + 0x89) + 1;
      }
      iVar13 = *(int *)((int)param_1 + 0x14);
      p_Var10 = local_20;
    } while ((iVar13 != -1) && (local_2c._4_4_ = local_2c._4_4_ + 1, local_2c._4_4_ < iVar12));
  }
  local_2c = uVar5;
  iVar13 = 0;
  piVar1 = (int *)((int)param_1 + 0x14);
  local_20 = param_1 + 0x89;
  FUN_0054a407(local_20,&local_24,(uint)p_Var10,piVar1);
  if (((*(uint *)((int)param_1 + 0x1c) >> 3 & 1) != 0) &&
     (((*(uint *)((int)param_1 + 0x1c) >> 2 & 1) == 0 &&
      (local_2c = local_2c & 0xffffffff, 0 < iVar12)))) {
    iVar14 = *piVar1;
    do {
      iVar2 = (int)*local_20;
      if (*(int *)(iVar2 + 8) == *(int *)(iVar2 + 4)) {
        if (*(char *)(iVar2 + 0xc) == '\0') {
          iVar14 = -1;
        }
        else {
          iVar14 = iVar14 + 1;
        }
        *piVar1 = iVar14;
      }
      else {
        *piVar1 = iVar14 + 1;
        *(int *)(iVar2 + 8) = *(int *)(iVar2 + 8) + 1;
        *(undefined1 *)**(undefined4 **)local_20 = 0x30;
        **(int **)local_20 = **(int **)local_20 + 1;
      }
      iVar14 = *piVar1;
      param_1 = local_1c[0];
    } while ((iVar14 != -1) && (iVar13 = iVar13 + 1, iVar13 < iVar12));
  }
  iVar13 = 0;
  local_8 = 0;
  if (((char)param_1[7] == '\0') || (*(int *)((int)param_1 + 0x34) < 1)) {
    FUN_0054a407(param_1 + 0x89,*(uint **)(param_1 + 6),*(uint *)((int)param_1 + 0x34),piVar1);
  }
  else {
    local_1c[0] = *(__uint64 **)(param_1 + 1);
    if (*(char *)((int)local_1c[0] + 0x14) == '\0') {
      FUN_0054a360((int *)local_1c[0]);
    }
    if (*(int *)(*(int *)((int)local_1c[0] + 0xc) + 8) == 0xfde9) {
      local_20 = (__uint64 *)0x0;
      local_2c = 0;
      pWVar11 = *(WCHAR **)(param_1 + 6);
      if (*(int *)((int)param_1 + 0x34) != 0) {
        do {
          local_24 = pWVar11 + 1;
          uVar9 = FUN_005595c4((byte *)local_1c,(uint)(ushort)*pWVar11,(int *)&local_2c,
                               (int)param_1[1]);
          if (uVar9 == 0xffffffff) goto LAB_00549dd7;
          FUN_0054a407(param_1 + 0x89,(uint *)local_1c,uVar9,piVar1);
          local_20 = (__uint64 *)((int)local_20 + 1);
          pWVar11 = local_24;
        } while (local_20 != *(__uint64 **)((int)param_1 + 0x34));
      }
    }
    else {
      local_2c = local_2c & 0xffffffff;
      pWVar11 = *(WCHAR **)(param_1 + 6);
      if (*(int *)((int)param_1 + 0x34) != 0) {
        do {
          local_24 = pWVar11 + 1;
          local_20 = (__uint64 *)0x0;
          iVar14 = FUN_005590fc(local_24,(int *)&local_20,(byte *)local_1c,6,*pWVar11,
                                *(int **)(param_1 + 1));
          if ((iVar14 != 0) || (local_20 == (__uint64 *)0x0)) goto LAB_00549dd7;
          FUN_0054a407(param_1 + 0x89,(uint *)local_1c,(uint)local_20,piVar1);
          iVar14 = local_2c._4_4_ + 1;
          local_2c = CONCAT44(iVar14,(int)local_2c);
          pWVar11 = local_24;
        } while (iVar14 != *(int *)((int)param_1 + 0x34));
      }
    }
  }
  goto LAB_00549e4a;
LAB_00549dd7:
  *piVar1 = -1;
LAB_00549e4a:
  iVar14 = *(int *)((int)param_1 + 0x14);
  if (((-1 < iVar14) && ((*(uint *)((int)param_1 + 0x1c) >> 2 & 1) != 0)) &&
     (piVar1 = (int *)((int)param_1 + 0x14), 0 < iVar12)) {
    do {
      iVar2 = (int)param_1[0x89];
      if (*(int *)(iVar2 + 8) == *(int *)(iVar2 + 4)) {
        if (*(char *)(iVar2 + 0xc) == '\0') {
          iVar14 = -1;
        }
        else {
          iVar14 = iVar14 + 1;
        }
        *piVar1 = iVar14;
      }
      else {
        *piVar1 = iVar14 + 1;
        *(int *)(iVar2 + 8) = *(int *)(iVar2 + 8) + 1;
        *(undefined1 *)**(undefined4 **)(param_1 + 0x89) = 0x20;
        **(int **)(param_1 + 0x89) = **(int **)(param_1 + 0x89) + 1;
      }
      iVar14 = *piVar1;
    } while ((iVar14 != -1) && (iVar13 = iVar13 + 1, iVar13 < iVar12));
  }
LAB_00549eae:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

