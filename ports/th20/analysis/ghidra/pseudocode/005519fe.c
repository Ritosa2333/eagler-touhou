/* Entry: 0x005519fe; symbol: FUN_005519fe; body bytes: 1124 */

void __cdecl FUN_005519fe(int *param_1)

{
  short *psVar1;
  byte bVar2;
  int *piVar3;
  char cVar4;
  int iVar5;
  BOOL BVar6;
  char *pcVar7;
  uint uVar8;
  BYTE *pBVar9;
  undefined2 *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  int *local_4c;
  undefined4 local_48;
  uint local_44;
  uint local_40;
  undefined4 *local_3c;
  int *local_38;
  undefined4 *local_34;
  short *local_30;
  char *local_2c;
  LPCSTR local_28;
  undefined4 *local_24;
  undefined4 *local_20;
  _cpinfo local_1c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_38 = param_1;
  puVar11 = (undefined4 *)0x0;
  local_20 = (undefined4 *)0x0;
  local_24 = (undefined4 *)0x0;
  local_28 = (LPCSTR)0x0;
  local_4c = param_1;
  local_48 = 0;
  if ((ushort *)param_1[0x2a] == (ushort *)0x0) {
    piVar3 = (int *)param_1[0x23];
    if (piVar3 != (int *)0x0) {
      LOCK();
      *piVar3 = *piVar3 + -1;
      UNLOCK();
    }
    param_1[0x23] = 0;
    param_1[0x24] = 0;
    *param_1 = (int)&DAT_00598c40;
    param_1[0x25] = (int)&DAT_00598ec0;
    param_1[0x26] = (int)&DAT_00599040;
    param_1[1] = 1;
    goto LAB_00551e54;
  }
  local_34 = (undefined4 *)0x0;
  if ((param_1[2] == 0) &&
     (iVar5 = ___acrt_GetLocaleInfoA((int *)&local_4c,0,(ushort *)param_1[0x2a],0x1004,param_1 + 2),
     iVar5 != 0)) {
LAB_00551de6:
    FUN_00557ba0(local_34);
    FUN_00557ba0(local_20);
    FUN_00557ba0(local_24);
    FUN_00557ba0(puVar11);
  }
  else {
    local_34 = (undefined4 *)FUN_00557b40(1,4);
    FUN_00557ba0((LPVOID)0x0);
    local_20 = (undefined4 *)FUN_00557b40(0x180,2);
    FUN_00557ba0((LPVOID)0x0);
    local_24 = (undefined4 *)FUN_00557b40(0x180,1);
    FUN_00557ba0((LPVOID)0x0);
    puVar11 = (undefined4 *)FUN_00557b40(0x180,1);
    local_3c = puVar11;
    FUN_00557ba0((LPVOID)0x0);
    local_28 = (LPCSTR)FUN_00557b40(0x101,1);
    FUN_00557ba0((LPVOID)0x0);
    if (((local_34 == (undefined4 *)0x0) ||
        (((local_20 == (undefined4 *)0x0 || (local_28 == (LPCSTR)0x0)) ||
         (local_24 == (undefined4 *)0x0)))) || (puVar11 == (undefined4 *)0x0)) goto LAB_00551de6;
    iVar5 = 0;
    do {
      local_28[iVar5] = (CHAR)iVar5;
      iVar5 = iVar5 + 1;
    } while (iVar5 < 0x100);
    BVar6 = GetCPInfo(param_1[2],&local_1c);
    if ((BVar6 == 0) || (5 < local_1c.MaxCharSize)) goto LAB_00551de6;
    local_44 = local_1c.MaxCharSize & 0xffff;
    if (1 < local_44) {
      if (param_1[2] == 0xfde9) {
        _memset(local_28 + 0x80,0x20,0x80);
      }
      else {
        pBVar9 = local_1c.LeadByte;
        bVar2 = local_1c.LeadByte[0];
        while ((bVar2 != 0 && (param_1 = local_38, pBVar9[1] != 0))) {
          uVar8 = (uint)*pBVar9;
          if (uVar8 <= pBVar9[1]) {
            do {
              local_28[uVar8] = ' ';
              uVar8 = uVar8 + 1;
            } while ((int)uVar8 <= (int)(uint)pBVar9[1]);
          }
          pBVar9 = pBVar9 + 2;
          bVar2 = *pBVar9;
        }
      }
    }
    local_2c = local_28 + 1;
    iVar5 = ___acrt_LCMapStringA
                      ((int *)0x0,(ushort *)param_1[0x2a],0x100,local_2c,0xff,
                       (LPWSTR)((int)local_24 + 0x81),0xff,param_1[2],0);
    if (((iVar5 == 0) ||
        (iVar5 = ___acrt_LCMapStringA
                           ((int *)0x0,(ushort *)param_1[0x2a],0x200,local_2c,0xff,
                            (LPWSTR)((int)puVar11 + 0x81),0xff,param_1[2],0), iVar5 == 0)) ||
       (iVar5 = FUN_0055d108((int *)0x0,1,local_28,0x100,(LPWORD)(local_20 + 0x40),param_1[2],0),
       iVar5 == 0)) goto LAB_00551de6;
    *(undefined2 *)((int)local_20 + 0xfe) = 0;
    *(undefined1 *)((int)local_24 + 0x7f) = 0;
    *(undefined1 *)((int)local_3c + 0x7f) = 0;
    *(undefined1 *)(local_24 + 0x20) = 0;
    *(undefined1 *)(local_3c + 0x20) = 0;
    if (1 < local_44) {
      if (param_1[2] == 0xfde9) {
        pcVar7 = (char *)(local_3c + 0x40);
        local_2c = (char *)(-0x142 - (int)local_3c);
        local_40 = -(int)local_3c - 0x80;
        local_30 = (short *)(local_20 + 0x80);
        do {
          psVar1 = local_30 + 1;
          *local_30 = (-(ushort)((char *)0x32 < local_2c + (int)pcVar7) & 0x8000) + 0x8000;
          cVar4 = (char)pcVar7 + (char)local_40;
          pcVar7[(int)local_24 - (int)local_3c] = cVar4;
          *pcVar7 = cVar4;
          pcVar7 = pcVar7 + 1;
          param_1 = local_38;
          local_30 = psVar1;
        } while ((int)(pcVar7 + local_40) < 0x100);
      }
      else if (local_1c.LeadByte[0] != 0) {
        pBVar9 = local_1c.LeadByte;
        do {
          param_1 = local_38;
          if (pBVar9[1] == 0) break;
          local_40 = (uint)*pBVar9;
          if (local_40 <= pBVar9[1]) {
            local_2c = (char *)((int)local_3c + local_40 + 0x80);
            local_30 = (short *)((int)local_24 - (int)local_3c);
            puVar10 = (undefined2 *)((int)local_20 + local_40 * 2 + 0x100);
            uVar8 = local_40;
            pcVar7 = local_2c;
            do {
              *puVar10 = 0x8000;
              puVar10 = puVar10 + 1;
              pcVar7[(int)local_30] = (char)uVar8;
              *pcVar7 = (char)uVar8;
              uVar8 = uVar8 + 1;
              pcVar7 = pcVar7 + 1;
            } while ((int)uVar8 <= (int)(uint)pBVar9[1]);
          }
          pBVar9 = pBVar9 + 2;
        } while (*pBVar9 != 0);
      }
    }
    puVar11 = local_20 + 0x80;
    puVar12 = local_20;
    for (iVar5 = 0x3f; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar12 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar12 = puVar12 + 1;
    }
    *(undefined2 *)puVar12 = *(undefined2 *)puVar11;
    puVar11 = local_24 + 0x40;
    puVar12 = local_24;
    for (iVar5 = 0x1f; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar12 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar12 = puVar12 + 1;
    }
    *(undefined2 *)puVar12 = *(undefined2 *)puVar11;
    *(undefined1 *)((int)puVar12 + 2) = *(undefined1 *)((int)puVar11 + 2);
    puVar11 = local_3c + 0x40;
    puVar12 = local_3c;
    for (iVar5 = 0x1f; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar12 = *puVar11;
      puVar11 = puVar11 + 1;
      puVar12 = puVar12 + 1;
    }
    *(undefined2 *)puVar12 = *(undefined2 *)puVar11;
    *(undefined1 *)((int)puVar12 + 2) = *(undefined1 *)((int)puVar11 + 2);
    piVar3 = (int *)param_1[0x23];
    if (piVar3 != (int *)0x0) {
      LOCK();
      iVar5 = *piVar3;
      *piVar3 = iVar5 + -1;
      UNLOCK();
      if (iVar5 + -1 == 0) {
        FUN_00557ba0((LPVOID)(param_1[0x24] + -0xfe));
        FUN_00557ba0((LPVOID)(param_1[0x25] + -0x80));
        FUN_00557ba0((LPVOID)(param_1[0x26] + -0x80));
        FUN_00557ba0((LPVOID)param_1[0x23]);
      }
    }
    *local_34 = 1;
    param_1[0x23] = (int)local_34;
    *param_1 = (int)(local_20 + 0x40);
    param_1[0x24] = (int)((int)local_20 + 0xfe);
    param_1[0x25] = (int)(local_24 + 0x20);
    param_1[0x26] = (int)(local_3c + 0x20);
    param_1[1] = local_44;
  }
  FUN_00557ba0(local_28);
LAB_00551e54:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

