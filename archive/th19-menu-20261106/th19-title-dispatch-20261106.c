// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 55ac60

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0055ac60(undefined4 *param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 *puVar5;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  undefined4 *extraout_ECX_02;
  undefined4 *extraout_ECX_03;
  undefined4 *extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 *extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 *extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 *extraout_ECX_14;
  undefined4 *extraout_ECX_15;
  undefined4 *extraout_ECX_16;
  undefined4 *extraout_ECX_17;
  undefined4 *extraout_ECX_18;
  undefined4 *extraout_ECX_19;
  undefined4 *extraout_ECX_20;
  undefined4 *extraout_ECX_21;
  undefined4 *extraout_ECX_22;
  undefined4 *extraout_ECX_23;
  undefined4 *extraout_ECX_24;
  int *piVar6;
  bool bVar7;
  float fVar8;
  undefined *local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined **local_14;
  void *local_10;
  undefined1 *puStack_c;
  int local_8;

  local_8 = 0xffffffff;
  puStack_c = &LAB_005789ab;
  local_10 = ExceptionList;
  uVar2 = DAT_005c3d80 ^ (uint)&stack0xfffffffc;
  if (DAT_005d1a40 != 0) {
    if ((*(byte *)(DAT_005d1a40 + 0x20) & 8) == 0) {
      return 1;
    }
    ExceptionList = &local_10;
    FUN_00507c40();
    FUN_005347a0(extraout_ECX,PTR_s_th19_01_005c5b64);
    FUN_005349e0(0,DAT_005c5b60);
    ExceptionList = local_10;
    return 1;
  }
  if (*(int *)(DAT_005d1a80 + 0xea8) == 0) {
    return 1;
  }
  puVar5 = param_1;
  ExceptionList = &local_10;
  if ((*(byte *)(param_1 + 0x181b) & 1) != 0) {
    iVar3 = param_1[0x181a];
    bVar7 = DAT_005d1c14 != 1;
    param_1[0x181a] = iVar3 + 1;
    if ((bVar7) || (-1 < iVar3 + 1)) {
      FUN_005346b0(uVar2);
      DAT_00627974 = 0;
      FUN_005347a0(extraout_ECX_00,PTR_s_th19_01_005c5b64);
      FUN_005349e0(0,DAT_005c5b60);
      param_1[0x181b] = param_1[0x181b] & 0xfffffffe;
      param_1[0x181a] = 0;
      puVar5 = extraout_ECX_01;
    }
  }
  switch(param_1[6]) {
  case 0:
    FUN_0040eee0(param_1[0x1867]);
    iVar3 = DAT_005d19bc;
    param_1[0x1867] = 0;
    FUN_004cb550(*(undefined4 *)(iVar3 + 0x64c0794));
    FUN_004cb550(*(undefined4 *)(iVar3 + 0x64c079c));
    FUN_004cb550(*(undefined4 *)(iVar3 + 0x64c0788));
    FUN_004cb550(*(undefined4 *)(iVar3 + 0x64c0780));
    local_28 = &DAT_0062aec8;
    iVar3 = FUN_0046230c(&DAT_0062aec8);
    if (iVar3 != 0) {
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(5);
    }
    if (DAT_0062aef4 == 0x7fffffff) {
      DAT_0062aef4 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
      FUN_004627e2(6);
    }
    iVar1 = param_1[0x1867];
    local_8 = iVar3;
    if (iVar1 != 0) {
      FUN_0040c910();
      iVar3 = FUN_0046230c(&DAT_0062ad48);
      if (iVar3 != 0) {
                    /* WARNING: Subroutine does not return */
        FUN_004627e2(5);
      }
      if (DAT_0062ad74 == 0x7fffffff) {
        DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
        FUN_004627e2(6);
      }
      thunk_FUN_0047a4db(iVar1);
      FUN_0046231d(&DAT_0062ad48);
    }
    param_1[0x1867] = 0;
    local_18 = 0;
    local_1c = 1;
    local_20 = 0x20;
    local_24 = 0x2a;
    local_14 = debug_memory_resource::vftable;
    local_8._0_1_ = 1;
    uVar4 = FUN_004d6b50(0x24,4);
    local_8 = CONCAT31(local_8._1_3_,2);
    FUN_0040c450(uVar4,&local_24,&local_20,&local_1c,&local_18);
    param_1[0x1867] = uVar4;
    FUN_005059e0((float)DAT_0062ee94);
    param_1[0x1868] = 0;
    iVar3 = 0;
    piVar6 = (int *)param_1[0x1867];
    if (*piVar6 != 1 && -1 < *piVar6 + -1) {
      do {
        iVar1 = iVar3 * 4;
        iVar3 = iVar3 + 1;
        *(undefined1 *)(*(int *)(piVar6[4] + iVar1) + 0x4bb) = 1;
        piVar6 = (int *)param_1[0x1867];
      } while (iVar3 < *piVar6 + -1);
    }
    local_8 = 0xffffffff;
    FUN_0046231d(&DAT_0062aec8);
    FUN_004107f0();
    *(undefined4 *)(DAT_005d1a80 + 0xea4) = 0;
    FUN_004de2f0(1,0);
    *(undefined4 *)(DAT_005d1abc + 0xea4) = 0;
    FUN_004de2f0(1,0);
    if ((_DAT_0062b0ac & 0x80) == 0) {
      param_1[0x181a] = 0;
      param_1[0x181b] = param_1[0x181b] | 1;
    }
    else {
      param_1[0x181b] = param_1[0x181b] & 0xfffffffe;
      FUN_00430300(DAT_0062bb28);
    }
    iVar3 = DAT_005d1c14;
    _DAT_0062b0ac = _DAT_0062b0ac & 0xffffff7f;
    if (DAT_005d1c14 == 0) {
      param_1[0x181b] = param_1[0x181b] | 2;
      FUN_004316b0(1);
      DAT_005d1c14 = 1;
LAB_0055af88:
      DAT_0062b0d4 = 0;
      DAT_0062b19c = 0;
      goto switchD_0055ad36_caseD_1;
    }
    uVar2 = param_1[0x181b] & 0xfffffffd;
    param_1[0x181b] = uVar2;
    if (iVar3 == 1) {
      iVar3 = FUN_0040e510();
      if (iVar3 == 4) {
        FUN_0040a670(1);
      }
      FUN_004316b0(1);
      *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
      FUN_0055a110();
      puVar5 = extraout_ECX_03;
      break;
    }
    if (iVar3 == 2) {
      FUN_004302d0(0);
      param_1[0xd] = 10;
      FUN_0040a670(4);
      FUN_004d5090();
      FUN_004316b0(0xf);
      DAT_005d1c14 = 1;
      *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
      puVar5 = extraout_ECX_04;
      break;
    }
    if (iVar3 == 8) {
      FUN_004302d0(0);
      param_1[0xd] = 10;
      FUN_0040a670(5);
      FUN_004d5090();
      param_1[0xd] = 10;
      FUN_0040a670(2);
      FUN_004d5090();
      FUN_004b9f00(0,0,extraout_ECX_05,0x67,2);
      FUN_004cb960(&local_24,extraout_ECX_06,0,0,0,0xffffffff,2,0);
      param_1[0x18a] = local_24;
      FUN_004cb7a0(local_24,3);
      FUN_004316b0(0x16);
      DAT_005d1c14 = 1;
      *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
      goto switchD_0055ad36_caseD_16;
    }
    if (iVar3 == 6) {
      FUN_004b9f00(0,0,uVar2,0x67,2);
      FUN_004cb960(&local_24,extraout_ECX_10,0,0,0,0xffffffff,2,0);
      param_1[0x18a] = local_24;
      FUN_004cb960(&local_24,extraout_ECX_11,1,0,0,0xffffffff,2,0);
      param_1[0x18b] = local_24;
      FUN_004cb7a0(param_1[0x18a],3);
      FUN_004316b0(0xb);
      *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
    }
    else {
      if (iVar3 != 3) goto LAB_0055af88;
      FUN_004cb960(&local_24,uVar2,0,0,0,0xffffffff,2,0);
      param_1[0x18a] = local_24;
      FUN_004cb960(&local_24,extraout_ECX_13,1,0,0,0xffffffff,2,0);
      param_1[0x18b] = local_24;
      FUN_004cb7a0(param_1[0x18a],3);
      FUN_004cb7a0(param_1[0x18b],3);
      FUN_004316b0(0xb);
      FUN_004d5090();
      iVar3 = DAT_005d1c28;
      if (DAT_005d1c28 != 0) {
        *(uint *)(DAT_005d1c28 + 0x70) = *(uint *)(DAT_005d1c28 + 0x70) & 0xffffff1f;
        *(undefined4 *)(iVar3 + 0x8c) = 0;
        *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
        FUN_00556340();
        puVar5 = extraout_ECX_14;
        break;
      }
    }
  case 0xb:
    *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
    FUN_00556340();
    puVar5 = extraout_ECX_12;
    break;
  case 1:
switchD_0055ad36_caseD_1:
    *(uint *)(param_1[3] + 4) = *(uint *)(param_1[3] + 4) | 2;
    FUN_0055a110();
    puVar5 = extraout_ECX_02;
    break;
  case 2:
    DAT_0062c450 = ~(DAT_0062c48c >> 9) & 1 | 2;
  case 0xc:
  case 0x10:
    FUN_005346b0(uVar2);
    puVar5 = extraout_ECX_19;
    break;
  case 3:
    if (param_1[8] != 0) {
      if ((param_1[8] == 1) && (DAT_005d1bf4 == 0)) {
        FUN_004316b0(1);
        FUN_004cb7a0(param_1[0x18a],2);
        puVar5 = extraout_ECX_15;
      }
      break;
    }
    FUN_005372f0();
    goto LAB_0055b3ee;
  case 4:
    if (param_1[8] != 0) {
      if ((param_1[8] == 1) && (DAT_005d1be0 == 0)) {
        FUN_004316b0(9);
        puVar5 = extraout_ECX_16;
      }
      break;
    }
    FUN_005238a0();
    goto LAB_0055b3ee;
  case 5:
  case 6:
    if (param_1[8] != 0) {
      if ((param_1[8] == 1) && (DAT_005d1a5c == 0)) {
        FUN_004316b0(9);
        puVar5 = extraout_ECX_17;
      }
      break;
    }
    goto LAB_0055b4d5;
  case 7:
    if (param_1[8] != 0) {
      if ((param_1[8] == 1) && (DAT_005d1a5c == 0)) {
        FUN_004316b0(8);
        puVar5 = extraout_ECX_18;
      }
      break;
    }
LAB_0055b4d5:
    FUN_0051a740(&DAT_0062be08,&DAT_0062be0c);
LAB_0055b3ee:
    param_1[8] = 1;
    puVar5 = param_1 + 0x187;
    if ((param_1[0x189] & 1) == 0) {
      param_1[0x186] = 0;
      *puVar5 = 0;
      param_1[0x188] = 0;
      param_1[0x189] = param_1[0x189] | 1;
    }
    param_1[0x186] = 0;
    *puVar5 = 0;
    param_1[0x185] = 0xffffffff;
    break;
  case 8:
    FUN_005596a0();
    puVar5 = extraout_ECX_20;
    break;
  case 9:
    FUN_005593b0();
    puVar5 = extraout_ECX_21;
    break;
  case 10:
    FUN_00558680();
    puVar5 = extraout_ECX_22;
    break;
  case 0x11:
    FUN_00555200();
    puVar5 = extraout_ECX_23;
    break;
  case 0x14:
    if (param_1[8] == 0) {
      puVar5 = (undefined4 *)FUN_00405530(&local_28,puVar5,0xcf,0xffffffff,puVar5);
      param_1[0x259] = *puVar5;
      FUN_0051e0f0();
      param_1[8] = 1;
      puVar5 = param_1 + 0x187;
      if ((param_1[0x189] & 1) == 0) {
        param_1[0x186] = 0;
        *puVar5 = 0;
        param_1[0x188] = 0;
        param_1[0x189] = param_1[0x189] | 1;
      }
      iVar3 = DAT_005d1bdc;
      param_1[0x186] = 0;
      *puVar5 = 0;
      param_1[0x185] = 0xffffffff;
      *(undefined4 *)(iVar3 + 0x13c) = 0x43500000;
    }
    else if ((param_1[8] == 1) && (*(int *)(DAT_005d1bdc + 0x138) != 0)) {
      FUN_004cb830(param_1[0x280],1);
      param_1[0x280] = 0;
      FUN_004cb830(param_1[0x259],1);
      param_1[0x259] = 0;
      FUN_004316b0(1);
      FUN_0051e090();
      puVar5 = extraout_ECX_24;
    }
    break;
  case 0x16:
switchD_0055ad36_caseD_16:
    FUN_00554850();
    puVar5 = extraout_ECX_07;
  }
  FUN_004026c0(puVar5);
  uVar4 = extraout_ECX_08;
  if (0 < (int)param_1[0x185e]) {
    FUN_00404810(extraout_ECX_08);
    uVar4 = extraout_ECX_09;
  }
  if (0 < (int)param_1[0x1863]) {
    FUN_00404810(uVar4);
  }
  FUN_00554130();
  iVar3 = param_1[0x1875];
  if (iVar3 < 0x28) {
    fVar8 = (float)iVar3 / DAT_005aa344;
    param_1[0x186b] =
         (int)(((float)(int)param_1[0x1873] - (float)(int)param_1[0x186f]) * fVar8 +
              (float)(int)param_1[0x186f]) & 0xff;
    param_1[0x186a] =
         (int)(((float)(int)param_1[0x1872] - (float)(int)param_1[0x186e]) * fVar8 +
              (float)(int)param_1[0x186e]) & 0xff;
    param_1[0x1869] =
         (int)(((float)(int)param_1[0x1871] - (float)(int)param_1[0x186d]) * fVar8 +
              (float)(int)param_1[0x186d]) & 0xff;
    param_1[0x186c] =
         (int)(((float)(int)param_1[0x1874] - (float)(int)param_1[0x1870]) * fVar8 +
              (float)(int)param_1[0x1870]) & 0xff;
    param_1[0x1875] = iVar3 + 1;
  }
  ExceptionList = local_10;
  return 1;
}


// Reference 0055aa67
// Address 559b80

undefined4 __fastcall FUN_00559b80(undefined4 param_1,uint param_2)

{
  bool bVar1;

  if (DAT_005d1c28 == 0) {
    if (DAT_005d19a8 == 0) {
      return 0;
    }
    if ((*(uint *)(DAT_005d19a8 + 0x10) & param_2) != 0) {
      return 1;
    }
    bVar1 = (*(uint *)(DAT_005d19a8 + 8) & param_2) == 0;
  }
  else {
    if (DAT_005d19a0 != 0) {
      if (((*(uint *)(DAT_005d19a0 + 0x10) & param_2) == 0) &&
         ((*(uint *)(DAT_005d19a0 + 8) & param_2) == 0)) {
        bVar1 = false;
      }
      else {
        bVar1 = true;
      }
      if (bVar1) {
        return 1;
      }
    }
    if (DAT_005d19a4 == 0) {
      return 0;
    }
    if (((*(uint *)(DAT_005d19a4 + 0x10) & param_2) == 0) &&
       ((*(uint *)(DAT_005d19a4 + 8) & param_2) == 0)) {
      bVar1 = true;
    }
    else {
      bVar1 = false;
    }
  }
  if (bVar1) {
    return 0;
  }
  return 1;
}


// Reference 005598fd
// Reference 00559909
// Reference 00559921
// Reference 0055992f
