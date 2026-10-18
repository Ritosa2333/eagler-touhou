/* Entry: 0x005566f6; symbol: __wsetlocale_nolock; body bytes: 751 */

/* Library Function - Single Match
    __wsetlocale_nolock
   
   Library: Visual Studio 2019 Release */

void __cdecl __wsetlocale_nolock(int param_1,int param_2,wchar_t *param_3)

{
  short *psVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  wchar_t *pwVar5;
  uint uVar6;
  short *psVar7;
  wchar_t *pwVar8;
  wchar_t wVar9;
  undefined4 *puVar10;
  undefined **ppuVar11;
  ushort *puVar12;
  bool bVar13;
  ushort *local_1d0;
  int local_1cc;
  int local_1c8;
  undefined4 local_1c4;
  undefined4 *local_1c0;
  wchar_t local_1bc [86];
  wchar_t local_110 [132];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1cc = param_1;
  if (param_2 != 0) {
    if (param_3 != (wchar_t *)0x0) {
      FUN_005569e6(param_1,param_2,param_3);
    }
    goto LAB_005569c5;
  }
  local_1c8 = 1;
  puVar10 = (undefined4 *)0x0;
  local_1c0 = (undefined4 *)0x0;
  if (param_3 != (wchar_t *)0x0) {
    if (((*param_3 == L'L') && (param_3[1] == L'C')) && (param_3[2] == L'_')) {
      do {
        local_1d0 = FUN_00560880((ushort *)param_3,(ushort *)&DAT_00599784);
        if (((local_1d0 == (ushort *)0x0) ||
            (iVar3 = (int)local_1d0 - (int)param_3 >> 1, local_1c8 = iVar3, iVar3 == 0)) ||
           (*local_1d0 == 0x3b)) goto LAB_005569c5;
        ppuVar11 = &PTR_u_LC_COLLATE_005996cc;
        local_1c4 = 1;
        do {
          iVar4 = FUN_00557290((ushort *)*ppuVar11,(ushort *)param_3,iVar3);
          if (iVar4 == 0) {
            psVar7 = (short *)*ppuVar11;
            psVar1 = psVar7 + 1;
            do {
              sVar2 = *psVar7;
              psVar7 = psVar7 + 1;
            } while (sVar2 != 0);
            if (iVar3 == (int)psVar7 - (int)psVar1 >> 1) break;
          }
          local_1c4 = local_1c4 + 1;
          ppuVar11 = ppuVar11 + 3;
        } while ((int)ppuVar11 < 0x5996fd);
        puVar12 = local_1d0 + 1;
        iVar3 = FUN_00560830(puVar12,(ushort *)&DAT_0059978c);
        param_1 = local_1cc;
        if ((iVar3 == 0) && (*puVar12 != 0x3b)) goto LAB_005569c5;
        if (local_1c4 < 6) {
          iVar4 = FUN_0055f860(local_110,0x83,(int)puVar12,iVar3);
          if (iVar4 != 0) {
LAB_005569d9:
                    /* WARNING: Subroutine does not return */
            __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          if (0x105 < (uint)(iVar3 * 2)) {
            ___report_rangecheckfailure();
            goto LAB_005569d9;
          }
          local_110[iVar3] = L'\0';
          iVar4 = FUN_005569e6(param_1,local_1c4,local_110);
          if (iVar4 != 0) {
            local_1c0 = (undefined4 *)((int)local_1c0 + 1);
          }
        }
        param_3 = (wchar_t *)(puVar12 + iVar3);
        wVar9 = *param_3;
        if (wVar9 != L'\0') {
          param_3 = param_3 + 1;
          wVar9 = *param_3;
        }
        puVar10 = local_1c0;
      } while (wVar9 != L'\0');
    }
    else {
      iVar3 = FUN_005560bf(param_3,local_110,0x83,local_1bc,0x55,(UINT *)&local_1d0);
      if (iVar3 == 0) goto LAB_005569c5;
      local_1c0 = (undefined4 *)(param_1 + 0x20);
      iVar3 = 0;
      do {
        if (iVar3 != 0) {
          pwVar8 = (wchar_t *)*local_1c0;
          pwVar5 = local_110;
          do {
            bVar13 = (ushort)*pwVar5 < (ushort)*pwVar8;
            if (*pwVar5 != *pwVar8) {
LAB_00556961:
              uVar6 = -(uint)bVar13 | 1;
              goto LAB_00556968;
            }
            if (*pwVar5 == L'\0') break;
            wVar9 = pwVar5[1];
            bVar13 = (ushort)wVar9 < (ushort)pwVar8[1];
            local_1c4 = CONCAT22(wVar9,(undefined2)local_1c4);
            if (wVar9 != pwVar8[1]) goto LAB_00556961;
            pwVar5 = pwVar5 + 2;
            pwVar8 = pwVar8 + 2;
          } while (wVar9 != L'\0');
          uVar6 = 0;
LAB_00556968:
          param_1 = local_1cc;
          if (uVar6 == 0) {
            puVar10 = (undefined4 *)((int)puVar10 + 1);
          }
          else {
            iVar4 = FUN_005569e6(local_1cc,iVar3,local_110);
            if (iVar4 == 0) {
              local_1c8 = 0;
            }
            else {
              puVar10 = (undefined4 *)((int)puVar10 + 1);
            }
          }
        }
        iVar3 = iVar3 + 1;
        local_1c0 = local_1c0 + 4;
      } while (iVar3 < 6);
      if (local_1c8 != 0) goto LAB_005569be;
    }
    if (puVar10 == (undefined4 *)0x0) goto LAB_005569c5;
  }
LAB_005569be:
  FUN_00556559(param_1);
LAB_005569c5:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

