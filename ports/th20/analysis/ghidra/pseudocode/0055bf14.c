/* Entry: 0x0055bf14; symbol: FUN_0055bf14; body bytes: 331 */

undefined4 __cdecl FUN_0055bf14(int *param_1,uint *param_2)

{
  int iVar1;
  longlong lVar2;
  uint uVar3;
  __acrt_ptd *p_Var4;
  uint uVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  uint uVar10;
  int *piVar11;
  undefined8 uVar12;
  uint local_10;
  uint local_c;
  char local_5;
  
  if (param_1 == (int *)0x0) {
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0x16;
    FUN_005480bc();
  }
  else {
    piVar11 = param_1;
    for (iVar7 = 9; iVar7 != 0; iVar7 = iVar7 + -1) {
      *piVar11 = -1;
      piVar11 = piVar11 + 1;
    }
    if (param_2 == (uint *)0x0) {
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
    }
    else {
      local_c = param_2[1];
      local_10 = *param_2;
      if (((local_c < 0x80000000) || ((-2 < (int)local_c && (0xffff573f < local_10)))) &&
         (((int)local_c < 7 || (((int)local_c < 8 && (local_10 < 0x9358efe0)))))) {
        local_5 = '\0';
        uVar5 = FUN_0055c05f(&local_10,&local_5);
        uVar3 = local_10;
        param_1[5] = uVar5;
        uVar12 = __alldiv(local_10,local_c,0x15180,0);
        iVar7 = (int)uVar12;
        uVar5 = (uint)((longlong)iVar7 * 0x15180);
        param_1[7] = iVar7;
        uVar9 = uVar3 - uVar5;
        local_c = (local_c - (int)((ulonglong)((longlong)iVar7 * 0x15180) >> 0x20)) -
                  (uint)(uVar3 < uVar5);
        puVar8 = (undefined4 *)&DAT_005a2614;
        if (local_5 == '\0') {
          puVar8 = &DAT_005a25e0;
        }
        iVar6 = 1;
        iVar1 = puVar8[1];
        while (iVar1 < iVar7) {
          iVar6 = iVar6 + 1;
          iVar1 = puVar8[iVar6];
        }
        param_1[4] = iVar6 + -1;
        param_1[3] = iVar7 - puVar8[iVar6 + -1];
        uVar12 = __alldiv(*param_2,param_2[1],0x15180,0);
        uVar3 = local_c;
        param_1[6] = ((int)uVar12 + 4) % 7;
        uVar12 = __alldiv(uVar9,local_c,0xe10,0);
        param_1[2] = (int)uVar12;
        lVar2 = (longlong)(int)uVar12 * 0xe10;
        uVar5 = (uint)lVar2;
        uVar10 = uVar9 - uVar5;
        uVar12 = __alldiv(uVar10,(uVar3 - (int)((ulonglong)lVar2 >> 0x20)) - (uint)(uVar9 < uVar5),
                          0x3c,0);
        param_1[8] = 0;
        param_1[1] = (int)uVar12;
        *param_1 = uVar10 + (int)uVar12 * -0x3c;
        return 0;
      }
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
    }
  }
  return 0x16;
}

