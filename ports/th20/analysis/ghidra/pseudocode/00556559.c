/* Entry: 0x00556559; symbol: FUN_00556559; body bytes: 412 */

undefined4 * __cdecl FUN_00556559(int param_1)

{
  short *psVar1;
  ushort uVar2;
  int *piVar3;
  bool bVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  int iVar7;
  ushort *puVar8;
  ushort *puVar9;
  uint uVar10;
  bool bVar11;
  undefined4 *local_10;
  undefined **local_8;
  
  puVar5 = (undefined4 *)FUN_005584c0(0x6a6);
  puVar6 = (undefined4 *)0x0;
  if (puVar5 != (undefined4 *)0x0) {
    *puVar5 = 1;
    psVar1 = (short *)(puVar5 + 1);
    *psVar1 = 0;
    __wcscats(psVar1,0x351,3);
    local_8 = &PTR_u_LC_COLLATE_005996cc;
    local_10 = (undefined4 *)(param_1 + 0x30);
    bVar4 = true;
    do {
      iVar7 = FUN_005517b0(psVar1,0x351,0x59978c);
      if (iVar7 != 0) {
                    /* WARNING: Subroutine does not return */
        __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      puVar8 = (ushort *)*local_10;
      puVar9 = (ushort *)local_10[4];
      do {
        uVar2 = *puVar8;
        bVar11 = uVar2 < *puVar9;
        if (uVar2 != *puVar9) {
LAB_00556608:
          uVar10 = -(uint)bVar11 | 1;
          goto LAB_0055660d;
        }
        if (uVar2 == 0) break;
        uVar2 = puVar8[1];
        bVar11 = uVar2 < puVar9[1];
        if (uVar2 != puVar9[1]) goto LAB_00556608;
        puVar8 = puVar8 + 2;
        puVar9 = puVar9 + 2;
      } while (uVar2 != 0);
      uVar10 = 0;
LAB_0055660d:
      bVar11 = false;
      if (uVar10 == 0) {
        bVar11 = bVar4;
      }
      local_8 = local_8 + 3;
      __wcscats(psVar1,0x351,3);
      local_10 = local_10 + 4;
      bVar4 = bVar11;
    } while ((int)local_8 < 0x5996fc);
    if (bVar11) {
      FUN_00557ba0(puVar5);
      piVar3 = *(int **)(param_1 + 0x28);
      if (piVar3 != (int *)0x0) {
        LOCK();
        iVar7 = *piVar3;
        *piVar3 = iVar7 + -1;
        UNLOCK();
        if (iVar7 + -1 == 0) {
          FUN_00557ba0(*(LPVOID *)(param_1 + 0x28));
        }
      }
      piVar3 = *(int **)(param_1 + 0x24);
      if (piVar3 != (int *)0x0) {
        LOCK();
        iVar7 = *piVar3;
        *piVar3 = *piVar3 + -1;
        UNLOCK();
        if (iVar7 == 1) {
          FUN_00557ba0(*(LPVOID *)(param_1 + 0x24));
        }
      }
      *(undefined4 *)(param_1 + 0x24) = 0;
      *(undefined4 *)(param_1 + 0x1c) = 0;
      *(undefined4 *)(param_1 + 0x28) = 0;
      *(undefined4 *)(param_1 + 0x20) = 0;
      puVar6 = *(undefined4 **)(param_1 + 0x40);
    }
    else {
      piVar3 = *(int **)(param_1 + 0x28);
      if (piVar3 != (int *)0x0) {
        LOCK();
        iVar7 = *piVar3;
        *piVar3 = iVar7 + -1;
        UNLOCK();
        if (iVar7 + -1 == 0) {
          FUN_00557ba0(*(LPVOID *)(param_1 + 0x28));
        }
      }
      piVar3 = *(int **)(param_1 + 0x24);
      if (piVar3 != (int *)0x0) {
        LOCK();
        iVar7 = *piVar3;
        *piVar3 = *piVar3 + -1;
        UNLOCK();
        if (iVar7 == 1) {
          FUN_00557ba0(*(LPVOID *)(param_1 + 0x24));
        }
      }
      *(undefined4 *)(param_1 + 0x24) = 0;
      puVar6 = puVar5 + 1;
      *(undefined4 *)(param_1 + 0x1c) = 0;
      *(undefined4 **)(param_1 + 0x28) = puVar5;
      *(undefined4 **)(param_1 + 0x20) = puVar6;
    }
  }
  return puVar6;
}

