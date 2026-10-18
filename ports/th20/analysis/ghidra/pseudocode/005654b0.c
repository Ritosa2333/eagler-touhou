/* Entry: 0x005654b0; symbol: FUN_005654b0; body bytes: 1084 */

void __cdecl FUN_005654b0(undefined1 *param_1,uint param_2,uint param_3,undefined *param_4)

{
  undefined1 *puVar1;
  undefined1 uVar2;
  __acrt_ptd *p_Var3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined1 *puVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  undefined1 *puVar10;
  int local_118;
  undefined1 *local_110;
  undefined1 *local_108;
  undefined1 *local_104;
  undefined4 auStack_f8 [30];
  undefined4 auStack_80 [30];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_104 = param_1;
  if ((((param_1 == (undefined1 *)0x0) && (param_2 != 0)) || (param_3 == 0)) ||
     (param_4 == (undefined *)0x0)) {
    p_Var3 = FUN_005516c1();
    *(undefined4 *)p_Var3 = 0x16;
    FUN_005480bc();
  }
  else {
    guard_check_icall();
    local_118 = 0;
    if (1 < param_2) {
      puVar7 = param_1 + (param_2 - 1) * param_3;
LAB_00565541:
      while (uVar4 = (uint)((int)puVar7 - (int)local_104) / param_3 + 1, 8 < uVar4) {
        iVar5 = (uVar4 >> 1) * param_3;
        puVar10 = local_104 + iVar5;
        iVar6 = (*(code *)param_4)(local_104,puVar10);
        if ((0 < iVar6) && (puVar8 = puVar10, uVar4 = param_3, local_104 != puVar10)) {
          do {
            uVar2 = puVar8[-iVar5];
            puVar8[-iVar5] = *puVar8;
            *puVar8 = uVar2;
            uVar4 = uVar4 - 1;
            puVar8 = puVar8 + 1;
          } while (uVar4 != 0);
        }
        iVar5 = (*(code *)param_4)(local_104,puVar7);
        if ((0 < iVar5) && (puVar8 = puVar7, uVar4 = param_3, local_104 != puVar7)) {
          do {
            puVar9 = puVar8 + 1;
            uVar2 = puVar9[(int)(local_104 + (-1 - (int)puVar7))];
            puVar9[(int)(local_104 + (-1 - (int)puVar7))] = *puVar8;
            *puVar8 = uVar2;
            uVar4 = uVar4 - 1;
            puVar8 = puVar9;
          } while (uVar4 != 0);
        }
        iVar5 = (*(code *)param_4)(puVar10,puVar7);
        local_110 = local_104;
        local_108 = puVar7;
        if ((0 < iVar5) && (puVar8 = puVar7, uVar4 = param_3, puVar10 != puVar7)) {
          do {
            puVar9 = puVar8 + 1;
            uVar2 = puVar9[(int)(puVar10 + (-1 - (int)puVar7))];
            puVar9[(int)(puVar10 + (-1 - (int)puVar7))] = *puVar8;
            *puVar8 = uVar2;
            uVar4 = uVar4 - 1;
            puVar8 = puVar9;
          } while (uVar4 != 0);
        }
LAB_00565700:
        puVar8 = puVar10;
        if (local_110 < puVar8) {
          do {
            local_110 = local_110 + param_3;
            if (puVar8 <= local_110) goto LAB_00565730;
            iVar5 = (*(code *)param_4)(local_110,puVar8);
            puVar9 = local_108;
          } while (iVar5 < 1);
        }
        else {
LAB_00565730:
          do {
            local_110 = local_110 + param_3;
            puVar9 = local_108;
            if (puVar7 < local_110) break;
            iVar5 = (*(code *)param_4)(local_110,puVar8);
          } while (iVar5 < 1);
        }
        do {
          local_108 = puVar9;
          puVar9 = local_108 + -param_3;
          if (puVar9 <= puVar8) break;
          iVar5 = (*(code *)param_4)(puVar9,puVar8);
        } while (0 < iVar5);
        if (local_110 <= puVar9) {
          puVar10 = puVar9;
          uVar4 = param_3;
          if (puVar9 != local_110) {
            do {
              puVar1 = puVar10 + 1;
              uVar2 = puVar1[(int)(local_110 + (-1 - (int)puVar9))];
              puVar1[(int)(local_110 + (-1 - (int)puVar9))] = *puVar10;
              *puVar10 = uVar2;
              uVar4 = uVar4 - 1;
              puVar10 = puVar1;
            } while (uVar4 != 0);
          }
          puVar10 = local_110;
          local_108 = puVar9;
          if (puVar8 != puVar9) {
            puVar10 = puVar8;
          }
          goto LAB_00565700;
        }
        if (puVar8 < local_108) {
          do {
            local_108 = local_108 + -param_3;
            if (local_108 <= puVar8) goto LAB_00565820;
            iVar5 = (*(code *)param_4)(local_108,puVar8);
          } while (iVar5 == 0);
        }
        else {
LAB_00565820:
          do {
            local_108 = local_108 + -param_3;
            if (local_108 <= local_104) break;
            iVar5 = (*(code *)param_4)(local_108,puVar8);
          } while (iVar5 == 0);
        }
        if ((int)local_108 - (int)local_104 < (int)puVar7 - (int)local_110) goto LAB_005658aa;
        if (local_104 < local_108) {
          auStack_80[local_118] = local_104;
          auStack_f8[local_118] = local_108;
          local_118 = local_118 + 1;
        }
        local_104 = local_110;
        if (puVar7 <= local_110) goto LAB_005655c9;
      }
      for (; puVar8 = local_104, puVar10 = local_104, local_104 < puVar7; puVar7 = puVar7 + -param_3
          ) {
        while (puVar9 = puVar8, puVar10 = puVar10 + param_3, puVar10 <= puVar7) {
          iVar5 = (*(code *)param_4)(puVar10,puVar9);
          puVar8 = puVar10;
          if (iVar5 < 1) {
            puVar8 = puVar9;
          }
        }
        puVar10 = puVar7;
        uVar4 = param_3;
        if (puVar9 != puVar7) {
          do {
            puVar8 = puVar10 + 1;
            uVar2 = puVar8[(int)(puVar9 + (-1 - (int)puVar7))];
            puVar8[(int)(puVar9 + (-1 - (int)puVar7))] = *puVar10;
            *puVar10 = uVar2;
            uVar4 = uVar4 - 1;
            puVar10 = puVar8;
          } while (uVar4 != 0);
        }
      }
      goto LAB_005655c9;
    }
  }
LAB_005654fe:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
LAB_005658aa:
  if (local_110 < puVar7) {
    auStack_80[local_118] = local_110;
    auStack_f8[local_118] = puVar7;
    local_118 = local_118 + 1;
  }
  puVar7 = local_108;
  if (local_108 <= local_104) {
LAB_005655c9:
    local_118 = local_118 + -1;
    if (-1 < local_118) {
      local_104 = (undefined1 *)auStack_80[local_118];
      puVar7 = (undefined1 *)auStack_f8[local_118];
      goto LAB_00565541;
    }
    goto LAB_005654fe;
  }
  goto LAB_00565541;
}

