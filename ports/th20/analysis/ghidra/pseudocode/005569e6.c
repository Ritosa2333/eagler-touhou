/* Entry: 0x005569e6; symbol: FUN_005569e6; body bytes: 944 */

/* WARNING: Type propagation algorithm not settling */

void __cdecl FUN_005569e6(int param_1,int param_2,wchar_t *param_3)

{
  ushort uVar1;
  undefined2 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  UINT UVar5;
  undefined4 uVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  undefined4 *puVar10;
  int iVar11;
  ushort *puVar12;
  uint uVar13;
  undefined4 *puVar14;
  int iVar15;
  ushort *puVar16;
  wchar_t *pwVar17;
  bool bVar18;
  UINT local_2c8;
  undefined4 local_2c4;
  wchar_t *local_2c0;
  wchar_t local_2bc [86];
  uint local_210 [64];
  ushort local_110 [132];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar9 = FUN_005579c2();
  puVar10 = (undefined4 *)(iVar9 + 0x278);
  iVar11 = FUN_005560bf(param_3,(short *)local_110,0x83,local_2bc,0x55,&local_2c8);
  if (iVar11 != 0) {
    iVar11 = param_2 + 2;
    puVar12 = local_110;
    puVar16 = *(ushort **)(param_1 + iVar11 * 0x10);
    do {
      local_2c0 = (wchar_t *)0x0;
      bVar18 = *puVar12 < *puVar16;
      if (*puVar12 != *puVar16) {
LAB_00556aae:
        uVar13 = -(uint)bVar18 | 1;
        goto LAB_00556ab3;
      }
      if (*puVar12 == 0) break;
      uVar1 = puVar12[1];
      bVar18 = uVar1 < puVar16[1];
      local_2c4 = CONCAT22(uVar1,(undefined2)local_2c4);
      if (uVar1 != puVar16[1]) goto LAB_00556aae;
      puVar12 = puVar12 + 2;
      puVar16 = puVar16 + 2;
    } while (uVar1 != 0);
    uVar13 = 0;
LAB_00556ab3:
    if (uVar13 != 0) {
      puVar12 = local_110;
      do {
        uVar1 = *puVar12;
        puVar12 = puVar12 + 1;
      } while (uVar1 != 0);
      local_2c4 = ((int)puVar12 - (int)(local_110 + 1) >> 1) + 1;
      puVar14 = (undefined4 *)FUN_005584c0(local_2c4 * 2 + 4);
      if (puVar14 != (undefined4 *)0x0) {
        puVar2 = *(undefined2 **)(param_1 + iVar11 * 0x10);
        uVar3 = *(undefined4 *)(param_1 + 0xa0 + param_2 * 4);
        uVar4 = *(undefined4 *)(param_1 + 8);
        iVar15 = FUN_00548100((short *)(puVar14 + 1),local_2c4,(int)local_110);
        if (iVar15 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson(local_2c0,local_2c0,local_2c0,(uint)local_2c0,(uintptr_t)local_2c0);
        }
        *(undefined4 **)(param_1 + iVar11 * 0x10) = puVar14 + 1;
        if ((local_110[0] != 0x43) || (pwVar17 = local_2c0, local_110[1] != 0)) {
          pwVar17 = ___acrt_copy_locale_name(local_2bc);
        }
        *(wchar_t **)(param_1 + 0xa0 + param_2 * 4) = pwVar17;
        if (param_2 == 2) {
          *(UINT *)(param_1 + 8) = local_2c8;
          pwVar17 = local_2c0;
          local_2c8 = *(UINT *)(iVar9 + 0x29c);
          local_2c4 = *(int *)(iVar9 + 0x298);
          do {
            if (*(int *)(param_1 + 8) == puVar10[(int)pwVar17 * 2]) {
              if (pwVar17 != (wchar_t *)0x0) {
                uVar6 = *(undefined4 *)(iVar9 + 0x27c + (int)pwVar17 * 8);
                *puVar10 = puVar10[(int)pwVar17 * 2];
                *(undefined4 *)(iVar9 + 0x27c) = uVar6;
                puVar10[(int)pwVar17 * 2] = local_2c4;
                *(UINT *)(iVar9 + 0x27c + (int)pwVar17 * 8) = local_2c8;
              }
              break;
            }
            iVar11 = puVar10[(int)pwVar17 * 2];
            UVar5 = *(UINT *)(iVar9 + 0x27c + (int)pwVar17 * 8);
            puVar10[(int)pwVar17 * 2] = local_2c4;
            *(UINT *)(iVar9 + 0x27c + (int)pwVar17 * 8) = local_2c8;
            pwVar17 = (wchar_t *)((int)pwVar17 + 1);
            local_2c8 = UVar5;
            local_2c4 = iVar11;
          } while ((int)pwVar17 < 5);
          if (pwVar17 == (wchar_t *)0x5) {
            iVar11 = FUN_0055d108((int *)local_2c0,1,
                                  "\x01\x02\x03\x04\x05\x06\a\b\t\n\v\f\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
                                  ,0x7f,(LPWORD)local_210,*(uint *)(param_1 + 8),1);
            pwVar17 = local_2c0;
            if (iVar11 != 0) {
              do {
                puVar12 = (ushort *)((int)local_210 + (int)pwVar17 * 2);
                *puVar12 = *puVar12 & 0x1ff;
                pwVar17 = (wchar_t *)((int)pwVar17 + 1);
              } while (pwVar17 < (wchar_t *)0x7f);
              iVar11 = FUN_00544d77(local_210,(uint *)PTR_DAT_005b2780,0xfe);
              pwVar17 = (wchar_t *)(uint)(iVar11 == 0);
            }
            *(wchar_t **)(iVar9 + 0x27c) = pwVar17;
            *puVar10 = *(undefined4 *)(param_1 + 8);
          }
          *(undefined4 *)(param_1 + 0x18) = *(undefined4 *)(iVar9 + 0x27c);
        }
        else if (param_2 == 1) {
          *(UINT *)(param_1 + 0x10) = local_2c8;
        }
        else if (param_2 == 5) {
          *(UINT *)(param_1 + 0x14) = local_2c8;
        }
        pcVar7 = (code *)(&PTR_FUN_005996c8)[param_2 * 3];
        (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
        iVar9 = (*pcVar7)();
        if (iVar9 == 0) {
          if (puVar2 != &DAT_005b2848) {
            piVar8 = *(int **)(param_1 + 0x28 + param_2 * 0x10);
            LOCK();
            iVar9 = *piVar8 + -1;
            *piVar8 = iVar9;
            UNLOCK();
            if (iVar9 == 0) {
              FUN_00557ba0(*(LPVOID *)(param_1 + 0x28 + param_2 * 0x10));
              FUN_00557ba0(*(LPVOID *)(param_1 + 0x24 + param_2 * 0x10));
              FUN_00557ba0(*(LPVOID *)(param_1 + 0xa0 + param_2 * 4));
              *(wchar_t **)(param_1 + (param_2 + 2) * 0x10) = local_2c0;
              *(wchar_t **)(param_1 + 0xa0 + param_2 * 4) = local_2c0;
            }
          }
          *puVar14 = 1;
          *(undefined4 **)(param_1 + 0x28 + param_2 * 0x10) = puVar14;
        }
        else {
          *(undefined2 **)(param_1 + (param_2 + 2) * 0x10) = puVar2;
          FUN_00557ba0(*(LPVOID *)(param_1 + 0xa0 + param_2 * 4));
          *(undefined4 *)(param_1 + 0xa0 + param_2 * 4) = uVar3;
          FUN_00557ba0(puVar14);
          *(undefined4 *)(param_1 + 8) = uVar4;
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

