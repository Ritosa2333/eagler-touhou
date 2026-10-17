/* Entry: 0x00560e06; symbol: FUN_00560e06; body bytes: 735 */

void FUN_00560e06(ushort *param_1)

{
  short *psVar1;
  ushort uVar2;
  short sVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  LCTYPE LVar7;
  void *extraout_ECX;
  void *pvVar8;
  ushort *puVar9;
  short *psVar10;
  WCHAR local_88 [64];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar5 = FUN_005579c2();
  pvVar8 = (void *)0x1002;
  if (*(int *)(iVar5 + 100) != 0) {
    pvVar8 = (void *)0x7;
  }
  iVar6 = ___acrt_GetLocaleInfoEx_16(pvVar8,param_1,(LCTYPE)pvVar8,local_88,0x40);
  if (iVar6 == 0) {
    *(undefined4 *)(iVar5 + 0x58) = 0;
    goto LAB_0056109c;
  }
  iVar6 = FUN_0055df10(*(ushort **)(iVar5 + 0x54),(ushort *)local_88);
  if (iVar6 != 0) goto LAB_00560f6a;
  LVar7 = 0x1001;
  if (*(int *)(iVar5 + 0x60) != 0) {
    LVar7 = 3;
  }
  iVar6 = ___acrt_GetLocaleInfoEx_16((void *)0x0,param_1,LVar7,local_88,0x40);
  if (iVar6 != 0) {
    iVar6 = FUN_0055df10(*(ushort **)(iVar5 + 0x50),(ushort *)local_88);
    pvVar8 = *(void **)(iVar5 + 0x58);
    puVar9 = param_1;
    if (iVar6 == 0) {
      *(uint *)(iVar5 + 0x58) = (uint)pvVar8 | 0x304;
      do {
        uVar2 = *puVar9;
        puVar9 = puVar9 + 1;
      } while (uVar2 != 0);
LAB_00560f45:
      iVar6 = FUN_0055f860((short *)(iVar5 + 0x2a0),0x55,(int)param_1,
                           ((int)puVar9 - (int)(param_1 + 1) >> 1) + 1);
      if (iVar6 != 0) goto LAB_005610d9;
    }
    else if (((uint)pvVar8 & 2) == 0) {
      if ((*(int *)(iVar5 + 0x5c) == 0) ||
         (iVar6 = FUN_00565d40(*(ushort **)(iVar5 + 0x50),(ushort *)local_88,*(int *)(iVar5 + 0x5c))
         , pvVar8 = extraout_ECX, iVar6 != 0)) {
        if (((*(byte *)(iVar5 + 0x58) & 1) != 0) ||
           (iVar6 = FUN_00561290(pvVar8,param_1), iVar6 == 0)) goto LAB_00560f6a;
        *(uint *)(iVar5 + 0x58) = *(uint *)(iVar5 + 0x58) | 1;
        do {
          uVar2 = *puVar9;
          puVar9 = puVar9 + 1;
        } while (uVar2 != 0);
      }
      else {
        *(uint *)(iVar5 + 0x58) = *(uint *)(iVar5 + 0x58) | 2;
        do {
          uVar2 = *puVar9;
          puVar9 = puVar9 + 1;
        } while (uVar2 != 0);
      }
      goto LAB_00560f45;
    }
LAB_00560f6a:
    if ((*(uint *)(iVar5 + 0x58) & 0x300) == 0x300) goto LAB_0056109c;
    LVar7 = 0x1001;
    if (*(int *)(iVar5 + 0x60) != 0) {
      LVar7 = 3;
    }
    iVar6 = ___acrt_GetLocaleInfoEx_16((void *)0x300,param_1,LVar7,local_88,0x40);
    if (iVar6 != 0) {
      iVar6 = FUN_0055df10(*(ushort **)(iVar5 + 0x50),(ushort *)local_88);
      if (iVar6 != 0) goto LAB_0056109c;
      uVar4 = *(uint *)(iVar5 + 0x58);
      *(void **)(iVar5 + 0x58) = (void *)(uVar4 | 0x200);
      puVar9 = param_1;
      if (*(int *)(iVar5 + 0x60) == 0) {
        if (*(int *)(iVar5 + 0x5c) != 0) {
          psVar10 = *(short **)(iVar5 + 0x50);
          psVar1 = psVar10 + 1;
          do {
            sVar3 = *psVar10;
            psVar10 = psVar10 + 1;
          } while (sVar3 != 0);
          if ((int)psVar10 - (int)psVar1 >> 1 == *(int *)(iVar5 + 0x5c)) {
            iVar6 = FUN_00561290((void *)(uVar4 | 0x200),param_1);
            if (iVar6 == 0) {
              psVar10 = *(short **)(iVar5 + 0x50);
              psVar1 = psVar10 + 1;
              do {
                sVar3 = *psVar10;
                psVar10 = psVar10 + 1;
              } while (sVar3 != 0);
              iVar6 = _GetPrimaryLen(*(ushort **)(iVar5 + 0x50));
              if (iVar6 == (int)psVar10 - (int)psVar1 >> 1) goto LAB_0056109c;
            }
            *(uint *)(iVar5 + 0x58) = *(uint *)(iVar5 + 0x58) | 0x100;
            if (*(short *)(iVar5 + 0x2a0) != 0) goto LAB_0056109c;
            do {
              uVar2 = *puVar9;
              puVar9 = puVar9 + 1;
            } while (uVar2 != 0);
            goto LAB_00561079;
          }
        }
        *(uint *)(iVar5 + 0x58) = uVar4 | 0x300;
        if (*(short *)(iVar5 + 0x2a0) != 0) goto LAB_0056109c;
        do {
          uVar2 = *puVar9;
          puVar9 = puVar9 + 1;
        } while (uVar2 != 0);
      }
      else {
        *(uint *)(iVar5 + 0x58) = uVar4 | 0x300;
        if (*(short *)(iVar5 + 0x2a0) != 0) goto LAB_0056109c;
        do {
          uVar2 = *puVar9;
          puVar9 = puVar9 + 1;
        } while (uVar2 != 0);
      }
LAB_00561079:
      iVar5 = FUN_0055f860((short *)(iVar5 + 0x2a0),0x55,(int)param_1,
                           ((int)puVar9 - (int)(param_1 + 1) >> 1) + 1);
      if (iVar5 != 0) {
LAB_005610d9:
                    /* WARNING: Subroutine does not return */
        __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      goto LAB_0056109c;
    }
  }
  *(undefined4 *)(iVar5 + 0x58) = 0;
LAB_0056109c:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

