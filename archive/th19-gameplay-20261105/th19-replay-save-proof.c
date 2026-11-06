// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 5512bc

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */

undefined4 FUN_00551130(undefined4 param_1,char *param_2)

{
  int *piVar1;
  char cVar2;
  float fVar3;
  int iVar4;
  undefined4 uVar5;
  void *_Dst;
  undefined4 *puVar6;
  int iVar7;
  int iVar8;
  char *pcVar9;
  int *piVar10;
  uint uVar11;
  int iVar12;
  undefined4 *puVar13;
  undefined4 *puVar14;
  int local_154;
  int local_14c;
  int local_148;
  int local_140;
  int local_13c;
  undefined1 local_138 [4];
  undefined4 local_134;
  undefined4 local_130;
  undefined4 local_12c;
  int local_128;
  int local_124;
  undefined1 local_114 [256];
  uint local_14;
  void *local_10;
  undefined1 *puStack_c;
  undefined4 local_8;

  iVar4 = DAT_005d1bfc;
  local_8 = 0xffffffff;
  puStack_c = &LAB_0057876d;
  local_10 = ExceptionList;
  local_14 = DAT_005c3d80 ^ (uint)&stack0xfffffffc;
  ExceptionList = &local_10;
  local_14c = 0;
  local_154 = 0;
  FUN_0046d433(*(undefined4 *)(DAT_005d1bfc + 0x1c),9,param_2,local_14);
  pcVar9 = param_2 + 1;
  do {
    cVar2 = *param_2;
    param_2 = param_2 + 1;
  } while (cVar2 != '\0');
  for (iVar12 = (int)param_2 - (int)pcVar9; iVar12 < 8; iVar12 = iVar12 + 1) {
    *(undefined1 *)(iVar12 + *(int *)(iVar4 + 0x1c)) = 0x20;
  }
  if ((*(byte *)(iVar4 + 0x1e0) & 1) == 0) {
    iVar12 = **(int **)(iVar4 + 0xac);
    **(undefined2 **)(iVar12 + 0x1518) = 0xffff;
    *(undefined2 *)(*(int *)(iVar12 + 0x1518) + 2) = 0xffff;
    *(undefined2 *)(*(int *)(iVar12 + 0x1518) + 4) = 0xffff;
    *(int *)(iVar12 + 0x1518) = *(int *)(iVar12 + 0x1518) + 6;
    if (0x1517 < *(int *)(iVar12 + 0x1518) - iVar12) {
      iVar12 = *(int *)(iVar4 + 0x1dc);
      local_8 = 0;
      _Dst = (void *)FUN_004d6b50(0x18b4,4);
      _memset(_Dst,0,0x18b4);
      puVar6 = (undefined4 *)((int)_Dst + 0x18a4);
      *(void **)((int)_Dst + 0x1518) = _Dst;
      *puVar6 = _Dst;
      *(int *)((int)_Dst + 0x18a0) = (int)_Dst + 0x151c;
      *(undefined4 *)((int)_Dst + 0x18ac) = 0;
      *(undefined4 *)((int)_Dst + 0x18a8) = 0;
      *(undefined4 *)((int)_Dst + 0x18b0) = 0;
      iVar12 = iVar4 + 0x3c + iVar12 * 0x10;
      local_8 = 0xffffffff;
      for (iVar7 = *(int *)(iVar12 + 4); iVar7 != 0; iVar7 = *(int *)(iVar7 + 4)) {
        iVar12 = iVar7;
      }
      if (*(int *)(iVar12 + 4) != 0) {
        *(int *)((int)_Dst + 0x18a8) = *(int *)(iVar12 + 4);
        *(undefined4 **)(*(int *)(iVar12 + 4) + 8) = puVar6;
      }
      *(undefined4 **)(iVar12 + 4) = puVar6;
      *(int *)((int)_Dst + 0x18ac) = iVar12;
      *(int *)(iVar4 + 0xb0) = *(int *)(iVar4 + 0xb0) + 1;
      *(undefined4 **)(iVar4 + 0xac) = puVar6;
    }
  }
  FUN_00401ac0(local_114,0x100,"replay/%s");
  local_140 = 0;
  local_13c = 0;
  do {
    iVar12 = *(int *)(iVar4 + 0x20 + local_13c * 4);
    if (iVar12 != 0) {
      iVar7 = local_13c;
      if (local_14c != 0) {
        iVar7 = local_14c;
      }
      if ((*(byte *)(iVar4 + 0x1e0) & 1) == 0) {
        *(undefined4 *)(iVar12 + 0xc) = 0;
      }
      for (piVar10 = *(int **)(iVar4 + (local_13c + 4) * 0x10); piVar10 != (int *)0x0;
          piVar10 = (int *)piVar10[1]) {
        iVar12 = *piVar10;
        if ((*(byte *)(iVar4 + 0x1e0) & 1) == 0) {
          piVar1 = (int *)(*(int *)(iVar4 + 0x20 + local_13c * 4) + 0xc);
          *piVar1 = *piVar1 + (*(int *)(iVar12 + 0x18a0) - iVar12) +
                              ((*(int *)(iVar12 + 0x1518) - iVar12) / 6) * 6 + -0x151c;
          piVar1 = (int *)(*(int *)(iVar4 + 0x20 + local_13c * 4) + 8);
          *piVar1 = *piVar1 + (*(int *)(*piVar10 + 0x1518) - *piVar10) / 6;
        }
      }
      local_140 = local_140 + 1;
      local_154 = local_13c;
      local_14c = iVar7;
    }
    fVar3 = DAT_005aa390;
    local_13c = local_13c + 1;
  } while (local_13c < 7);
  *(int *)(*(int *)(iVar4 + 0x1c) + 0x124) = local_140;
  uVar5 = DAT_0062b0c4;
  iVar12 = *(int *)(iVar4 + 0x1c);
  *(undefined4 *)(iVar12 + 0x18) = DAT_0062b0c0;
  *(undefined4 *)(iVar12 + 0x1c) = uVar5;
  *(float *)(*(int *)(iVar4 + 0x1c) + 0x120) =
       fVar3 - (float)(*(double *)(DAT_005d1a4c + 0x28) / *(double *)(DAT_005d1a4c + 0x30)) * fVar3;
  iVar12 = FUN_0046230c();
  if (iVar12 == 0) {
    if (DAT_0062ad74 == 0x7fffffff) {
      DAT_0062ad74 = 0x7ffffffe;
    }
    else {
      puVar6 = (undefined4 *)FUN_0047a4d0();
      FUN_0046231d();
      if (puVar6 == (undefined4 *)0x0) {
        ExceptionList = local_10;
        return 0;
      }
      puVar13 = *(undefined4 **)(iVar4 + 0x1c);
      puVar14 = puVar6;
      for (iVar12 = 0x50; iVar12 != 0; iVar12 = iVar12 + -1) {
        *puVar14 = *puVar13;
        puVar13 = puVar13 + 1;
        puVar14 = puVar14 + 1;
      }
      puVar13 = (undefined4 *)(iVar4 + 0x40);
      local_148 = 7;
      piVar10 = (int *)(iVar4 + 0x20);
      iVar12 = 0x140;
      do {
        if (*piVar10 != 0) {
          FUN_00464df0((int)puVar6 + iVar12,*piVar10);
          iVar12 = iVar12 + 0x2558;
          for (piVar1 = (int *)*puVar13; piVar1 != (int *)0x0; piVar1 = (int *)piVar1[1]) {
            iVar7 = *piVar1;
            FUN_00464df0((int)puVar6 + iVar12,iVar7);
            iVar12 = iVar12 + ((*(int *)(iVar7 + 0x1518) - iVar7) / 6) * 6;
          }
          for (piVar1 = (int *)*puVar13; piVar1 != (int *)0x0; piVar1 = (int *)piVar1[1]) {
            iVar7 = *piVar1;
            FUN_00464df0((int)puVar6 + iVar12,iVar7 + 0x151c);
            iVar12 = iVar12 + -0x151c + (*(int *)(iVar7 + 0x18a0) - iVar7);
          }
        }
        puVar13 = puVar13 + 4;
        piVar10 = piVar10 + 1;
        local_148 = local_148 + -1;
      } while (local_148 != 0);
      iVar7 = FUN_0055d2d0();
      iVar8 = FUN_0046230c();
      if (iVar8 != 0) goto LAB_00551a78;
      if (DAT_0062ad74 == 0x7fffffff) {
        DAT_0062ad74 = 0x7ffffffe;
      }
      else {
        FUN_0047a4db();
        FUN_0046231d();
        FUN_004d5750(0x7d,0x3a,0x100);
        FUN_004d5750(0x5c,0xe1,0x400,piVar10);
        *(int *)(*(int *)(iVar4 + 0x18) + 0x20) = iVar12;
        *(int **)(*(int *)(iVar4 + 0x18) + 0x1c) = piVar10;
        *(int *)(*(int *)(iVar4 + 0x18) + 0xc) = *(int *)(*(int *)(iVar4 + 0x18) + 0x1c) + 0x24;
        FUN_0046dbd9(&DAT_0062ce79);
        FUN_004d5620();
        FUN_004d5460();
        FUN_004d5460();
        if (iVar7 != 0) {
          iVar12 = FUN_0046230c();
          if (iVar12 != 0) goto LAB_00551a78;
          if (DAT_0062ad74 == 0x7fffffff) {
            DAT_0062ad74 = 0x7ffffffe;
            goto LAB_00551a89;
          }
          FUN_0047a4db();
          FUN_0046231d();
        }
        iVar12 = FUN_0046230c();
        if (iVar12 != 0) goto LAB_00551a78;
        if (DAT_0062ad74 == 0x7fffffff) {
          DAT_0062ad74 = 0x7ffffffe;
        }
        else {
          puVar6 = (undefined4 *)FUN_0047a4d0();
          FUN_0046231d();
          _memset(puVar6,0,0xffff);
          *puVar6 = 0x52455355;
          iVar12 = FUN_00401ac0(puVar6 + 3,0x100,&DAT_005a05c8,&DAT_005a05bc);
          iVar12 = (int)(puVar6 + 3) + iVar12;
          iVar7 = FUN_00401ac0(iVar12,0x100,"Version %s\r\n","1.10c");
          iVar12 = iVar12 + iVar7;
          iVar7 = FUN_00401ac0(iVar12,0x100,"Name %s\r\n",*(undefined4 *)(iVar4 + 0x1c));
          iVar12 = iVar12 + iVar7;
          FUN_0047bd08(local_138,*(int *)(iVar4 + 0x1c) + 0x10);
          iVar7 = FUN_00401ac0(iVar12,0x100,"Date %.2d/%.2d/%.2d %.2d:%.2d\r\n",local_124 % 100,
                               local_128 + 1,local_12c,local_130,local_134);
          iVar12 = iVar12 + iVar7;
          iVar7 = FUN_00401ac0(iVar12,0x100,"Chara %s\r\n",
                               (&PTR_s_Reimu_0059d330)
                               [*(int *)(*(int *)(iVar4 + 0x1c) + 300) +
                                *(int *)(*(int *)(iVar4 + 0x1c) + 0x128)]);
          iVar12 = iVar12 + iVar7;
          iVar7 = FUN_00401ac0(iVar12,0x100,"Rank %s\r\n",
                               (&PTR_s_Very_Easy_0059d310)[*(int *)(*(int *)(iVar4 + 0x1c) + 0x130)]
                              );
          iVar12 = iVar12 + iVar7;
          if (*(int *)(*(int *)(iVar4 + 0x1c) + 0x134) < 8) {
            if (local_14c == local_154) {
              if (local_14c == 7) {
                iVar7 = FUN_00401ac0(iVar12,0x100,"Extra Stage\r\n");
              }
              else {
                iVar7 = FUN_00401ac0(iVar12,0x100,"Stage %d\r\n",local_14c);
              }
            }
            else {
              iVar7 = FUN_00401ac0(iVar12,0x100,&DAT_005a0684,local_14c,local_154);
            }
          }
          else {
            pcVar9 = "Stage All Clear\r\n";
            if (local_14c == 7) {
              pcVar9 = "Extra Stage Clear\r\n";
            }
            iVar7 = FUN_00401ac0(iVar12,0x100,pcVar9);
          }
          iVar8 = FUN_00401ac0(iVar12 + iVar7,0x100,"Score %llu\r\n",
                               *(undefined4 *)(*(int *)(iVar4 + 0x1c) + 0x18),
                               *(undefined4 *)(*(int *)(iVar4 + 0x1c) + 0x1c));
          iVar8 = iVar12 + iVar7 + iVar8;
          iVar12 = FUN_00401ac0(iVar8,0x100,"Slow Rate %2.2f\r\n",
                                (double)*(float *)(*(int *)(iVar4 + 0x1c) + 0x120));
          iVar12 = iVar8 + 1 + iVar12;
          uVar11 = iVar12 - (int)puVar6 & 0x80000003;
          if ((int)uVar11 < 0) {
            uVar11 = (uVar11 - 1 | 0xfffffffc) + 1;
          }
          if (uVar11 != 0) {
            iVar12 = iVar12 + (4 - uVar11);
          }
          puVar6[1] = iVar12 - (int)puVar6;
          FUN_004d5460();
          _memset(puVar6,0,0xffff);
          *puVar6 = 0x52455355;
          *(undefined1 *)(puVar6 + 2) = 1;
          iVar12 = FUN_00401ac0(puVar6 + 3,0x100);
          iVar12 = (int)puVar6 + iVar12 + 0xd;
          uVar11 = iVar12 - (int)puVar6 & 0x80000003;
          if ((int)uVar11 < 0) {
            uVar11 = (uVar11 - 1 | 0xfffffffc) + 1;
          }
          if (uVar11 != 0) {
            iVar12 = iVar12 + (4 - uVar11);
          }
          puVar6[1] = iVar12 - (int)puVar6;
          FUN_004d5460();
          iVar12 = FUN_0046230c();
          if (iVar12 != 0) goto LAB_00551a78;
          if (DAT_0062ad74 == 0x7fffffff) {
            DAT_0062ad74 = 0x7ffffffe;
          }
          else {
            FUN_0047a4db();
            FUN_0046231d();
            iVar12 = FUN_0046230c();
            if (iVar12 != 0) goto LAB_00551a78;
            if (DAT_0062ada4 != 0x7fffffff) {
              if (DAT_005c6414 != (HANDLE)0xffffffff) {
                CloseHandle(DAT_005c6414);
              }
              FUN_0046231d();
              FUN_0046dbd9();
              *(uint *)(iVar4 + 0x1e0) = *(uint *)(iVar4 + 0x1e0) | 1;
              ExceptionList = local_10;
              return 0;
            }
            DAT_0062ada4 = 0x7ffffffe;
          }
        }
      }
    }
LAB_00551a89:
                    /* WARNING: Subroutine does not return */
    FUN_004627e2();
  }
LAB_00551a78:
                    /* WARNING: Subroutine does not return */
  FUN_004627e2();
}


// Address 5505b0

undefined4 __fastcall FUN_005505b0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;

  iVar1 = DAT_005d1a34;
  if (((DAT_005d1a54 != 0) && (*(int *)(param_1 + 0x10) != 0)) && (*(int *)(param_1 + 0x10) == 1)) {
    local_10 = 0x43bf8000;
    local_c = 0x43e10000;
    local_8 = 0;
    if (DAT_005aa32c <= (float)*(byte *)(param_1 + 0x1d0)) {
      uVar2 = 0xffffffff;
      if ((float)*(byte *)(param_1 + 0x1d0) < DAT_005aa354) {
        uVar2 = 0xffa0a0ff;
      }
    }
    else {
      uVar2 = 0xff5050ff;
    }
    *(undefined4 *)(DAT_005d1a34 + 0x1977c) = uVar2;
    FUN_004e7610(iVar1,&local_10,&DAT_005a0534,*(undefined1 *)(param_1 + 0x1d0));
    *(undefined4 *)(DAT_005d1a34 + 0x1977c) = 0xffffffff;
  }
  return 1;
}


// Address 550670

char __fastcall FUN_00550670(int param_1)

{
  uint uVar1;
  int *piVar2;
  int *piVar3;
  int local_10 [3];

  if ((DAT_005d1a54 != 0) && (*(int *)(param_1 + 0x10) == 1)) {
    if ((DAT_005d19a0 != (uint *)0x0) &&
       (((*DAT_005d19a0 & 0x200) != 0 || ((*DAT_005d19a0 & 1) != 0)))) {
      *(undefined4 *)(param_1 + 0x14) = 1;
      uVar1 = *(uint *)(param_1 + 0x1d4) & 0x80000007;
      if ((int)uVar1 < 0) {
        uVar1 = (uVar1 - 1 | 0xfffffff8) + 1;
      }
      return (-(uVar1 != 0) & 5U) + 1;
    }
    if (*(int *)(param_1 + 0x14) != 0) {
      piVar3 = local_10;
      local_10[1] = 999999999;
      local_10[0] = 0;
      if (-1 < DAT_0062ba7c) {
        piVar3 = &DAT_0062ba7c;
      }
      piVar2 = local_10 + 1;
      if (DAT_0062ba7c < 1000000000) {
        piVar2 = piVar3;
      }
      DAT_0062ba7c = *piVar2;
      FUN_004d7510();
    }
    *(undefined4 *)(param_1 + 0x14) = 0;
  }
  return '\x01';
}


// Address 5510b0

undefined4 * __fastcall FUN_005510b0(undefined4 param_1)

{
  undefined4 *puVar1;
  int iVar2;

  puVar1 = (undefined4 *)FUN_004300b0(param_1);
  puVar1[4] = 2;
  iVar2 = FUN_00550250(param_1);
  if (iVar2 == 0) {
    return puVar1;
  }
  (**(code **)*puVar1)(0);
  iVar2 = FUN_0046230c(&DAT_0062ad48);
  if (iVar2 == 0) {
    if (DAT_0062ad74 != 0x7fffffff) {
      thunk_FUN_0047a4db(puVar1);
      FUN_0046231d(&DAT_0062ad48);
      return (undefined4 *)0x0;
    }
    DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
    FUN_004627e2(6);
  }
                    /* WARNING: Subroutine does not return */
  FUN_004627e2(5);
}


// Reference 005386f9
// Reference 005389d4
// Address 5386e1

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00537d40(int param_1)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 extraout_ECX_14;
  undefined4 extraout_ECX_15;
  undefined4 extraout_ECX_16;
  undefined4 extraout_ECX_17;
  int extraout_ECX_18;
  int extraout_ECX_19;
  int extraout_ECX_20;
  int extraout_ECX_21;
  int extraout_ECX_22;
  undefined4 extraout_ECX_23;
  undefined4 extraout_ECX_24;
  undefined4 *puVar9;
  byte *pbVar10;
  int *piVar11;
  int *piVar12;
  bool bVar13;
  undefined4 in_XMM0_Da;
  float fVar14;
  undefined4 in_XMM0_Db;
  undefined4 uVar15;
  int local_6c;
  undefined1 local_68 [4];
  float local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  int local_54;
  undefined1 local_50 [68];
  uint local_c;

  local_c = DAT_005c3d80 ^ (uint)&local_6c;
  iVar8 = *(int *)(param_1 + 0x400);
  iVar7 = *(int *)(DAT_005d19b0 + 0x2e30 + iVar8 * 4);
  switch(*(undefined4 *)(param_1 + 0x1fc)) {
  case 0:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    FUN_00416bd0(10);
    iVar8 = DAT_005d1a54;
    *(undefined4 *)(param_1 + 0x40) = 5;
    if (*(int *)(iVar8 + 0x160) != 0) {
      FUN_00416b50(3);
    }
    iVar8 = FUN_00412af0();
    if (0 < iVar8) {
      FUN_004cb7a0(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x42,extraout_ECX);
      FUN_004cb7a0(*puVar9,5);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4f,extraout_ECX_00);
      FUN_004cb7a0(*puVar9,5);
    }
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x404) + 0x20) + 0x2c) == 0) {
      FUN_00416b50(2);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3c,extraout_ECX_01);
      FUN_004cb7a0(*puVar9,5);
    }
    cVar2 = FUN_00412310();
    if (cVar2 != '\0') {
      FUN_00416b50(4);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3e,extraout_ECX_02);
      FUN_004cb7a0(*puVar9,5);
      FUN_00416b50(3);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3d,extraout_ECX_03);
      FUN_004cb7a0(*puVar9,5);
    }
    *(undefined4 *)(param_1 + 0x108) = 1;
    FUN_0040a670(0);
    goto LAB_00537eac;
  case 1:
    if (9 < *(int *)(param_1 + 0x14)) {
      FUN_00416bd0(6);
      *(undefined4 *)(param_1 + 0x40) = 5;
      FUN_00416b50(3);
      FUN_00416b50(0);
      FUN_00416b50(2);
      FUN_00416b50(4);
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      *(undefined4 *)(param_1 + 0x20c) = 1;
      return;
    }
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
LAB_00538a88:
    FUN_00416bd0(6);
    *(undefined4 *)(param_1 + 0x40) = 5;
    *(undefined4 *)(param_1 + 0x108) = 1;
    FUN_00416b50(2);
    FUN_00416b50(3);
    FUN_00416b50(4);
    FUN_0040a670(0);
    *(undefined4 *)(param_1 + 0x108) = 1;
LAB_00537eac:
    FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
    *(undefined4 *)(param_1 + 0x20c) = 0;
    return;
  case 6:
    goto switchD_00537d7a_caseD_6;
  case 7:
    if (*(int *)(param_1 + 0x14) < 0x3c) {
      return;
    }
    if (iVar7 < 0) {
      return;
    }
    if ((&DAT_005d19a0)[iVar7] == 0) {
      return;
    }
    if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0) {
      return;
    }
    goto LAB_00537fa2;
  case 8:
  case 0xb:
    if (*(int *)(param_1 + 0x14) < 0x14) {
      return;
    }
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004d5090();
      *(undefined4 *)(param_1 + 0x40) = 2;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),0xe);
      iVar8 = extraout_ECX_18;
    }
    if (*(int *)(param_1 + 0x14) < 0x1e) {
      return;
    }
    if (*(int *)(param_1 + 0x14) == 0x1e) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 0xf));
      iVar8 = extraout_ECX_19;
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      iVar8 = 0;
      if (((&DAT_005d19a0)[iVar7] != 0) &&
         (iVar3 = FUN_00401a60(0x20), iVar8 = extraout_ECX_20, iVar3 != 0)) {
        FUN_004d4fa0(1);
        iVar8 = extraout_ECX_21;
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),
                   (int)(short)((short)*(int *)(param_1 + 0x38) + 0xf));
      FUN_004bb8d0(10,0);
      iVar8 = extraout_ECX_22;
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] != 0) && ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) != 0)
       ) {
      if (*(int *)(param_1 + 0x38) == 0) {
        piVar12 = (int *)FUN_004cafa0(local_68,0x54,iVar8);
        if (*piVar12 != 0) {
          puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x54,extraout_ECX_23);
          FUN_004cb830(*puVar9,6);
        }
        uVar15 = 0xc;
        if (*(int *)(param_1 + 0x1fc) != 0xb) {
          uVar15 = 9;
        }
        FUN_00416bd0(uVar15);
        uVar15 = 7;
      }
      else {
        if (*(int *)(param_1 + 0x38) != 1) goto LAB_00538550;
        puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x55,iVar8);
        FUN_004cb830(*puVar9,6);
        FUN_00416bd0(9);
        uVar15 = 9;
      }
      FUN_004bb8d0(uVar15,0);
    }
LAB_00538550:
    if ((&DAT_005d19a0)[iVar7] == 0) {
      if (iVar7 < 0) {
        return;
      }
    }
    else if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) != 0) {
      FUN_004bb8d0(9,0);
      if (*(int *)(param_1 + 0x38) == 0) {
        FUN_0040a670(1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 0xf)
                    );
      }
      else if (*(int *)(param_1 + 0x38) == 1) {
        puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x55,extraout_ECX_24);
        FUN_004cb830(*puVar9,6);
        FUN_00416bd0(9);
      }
    }
    if ((&DAT_005d19a0)[iVar7] == 0) {
      return;
    }
    if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x100) == 0) {
      return;
    }
    if (*(int *)(param_1 + 500) != 1) {
      return;
    }
    goto LAB_005385fa;
  case 9:
    if (*(int *)(param_1 + 0x14) < 0x14) {
      return;
    }
    if (*(int *)(param_1 + 0x38) != 0) {
      if (*(int *)(param_1 + 0x38) != 1) {
        return;
      }
      FUN_004d5040();
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      uVar15 = 10;
      goto LAB_00537fae;
    }
    FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
    FUN_004d5040();
    goto LAB_00537fac;
  case 10:
    FUN_00416bd0(6);
    if (*(int *)(DAT_005d1a54 + 0x160) != 0) {
      FUN_00416b50(3);
    }
    iVar8 = FUN_00412af0();
    if (0 < iVar8) {
      FUN_004cb7a0(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x42,extraout_ECX_04);
      FUN_004cb7a0(*puVar9,5);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x4f,extraout_ECX_05);
      FUN_004cb7a0(*puVar9,5);
    }
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x404) + 0x20) + 0x2c) == 0) {
      FUN_00416b50(2);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3c,extraout_ECX_06);
      FUN_004cb7a0(*puVar9,5);
    }
    cVar2 = FUN_00412310();
    if (cVar2 != '\0') {
      FUN_00416b50(4);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3e,extraout_ECX_07);
      FUN_004cb7a0(*puVar9,5);
      FUN_00416b50(3);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3d,extraout_ECX_08);
      FUN_004cb7a0(*puVar9,5);
    }
switchD_00537d7a_caseD_6:
    if (((*(int *)(param_1 + 0x14) < 2) && (*(int *)(param_1 + 500) == 2)) && (DAT_0062b0b4 < 1)) {
      FUN_00416b50(0);
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x20), iVar8 != 0)) {
        FUN_004d4fa0(1);
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),
                   (int)(short)((short)*(int *)(param_1 + 0x38) + 7));
      FUN_004bb8d0(10,0);
    }
    if (((-1 < iVar7) && ((&DAT_005d19a0)[iVar7] != 0)) &&
       ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) != 0)) {
      FUN_004bb8d0(7,0);
      switch(*(undefined4 *)(param_1 + 0x38)) {
      case 0:
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        uVar15 = 0x14;
        break;
      case 1:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3b,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x41,extraout_ECX_10);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x48,extraout_ECX_11);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4c,extraout_ECX_12);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4e,extraout_ECX_13);
        FUN_004cb830(*puVar9,6);
        if ((*(int *)(DAT_005d1a54 + 0x160) != 0) || (uVar15 = 8, *(int *)(param_1 + 500) != 1)) {
          uVar15 = 0x14;
        }
        break;
      case 2:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3c,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x43,extraout_ECX_16);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x13;
        break;
      case 3:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3d,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x44,extraout_ECX_14);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x10;
        break;
      case 4:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3e,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x45,extraout_ECX_15);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x12;
        break;
      default:
        goto switchD_0053819e_default;
      }
      FUN_00416bd0(uVar15);
switchD_0053819e_default:
      FUN_00402650(0);
    }
    if (*(int *)(param_1 + 0x20c) == 0) {
      if (iVar7 < 0) {
        return;
      }
      if ((&DAT_005d19a0)[iVar7] == 0) goto LAB_0053837b;
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x100) == 0) goto LAB_00538373;
LAB_005385fa:
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
      uVar15 = 0;
    }
    else {
LAB_00538373:
      if (iVar7 < 0) {
        return;
      }
LAB_0053837b:
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x10000) == 0) {
        return;
      }
      FUN_004bb8d0(7,0);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3b,extraout_ECX_17);
      FUN_004cb830(*puVar9,6);
LAB_00537fa2:
      uVar15 = 1;
    }
    FUN_0040a670(uVar15);
LAB_00537fac:
    uVar15 = 0x14;
LAB_00537fae:
    FUN_00416bd0(uVar15);
    return;
  case 0xc:
    if (0x13 < *(int *)(param_1 + 0x14)) {
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
      FUN_00416bd0(0xd);
      FUN_004cb480();
      FUN_004d5090();
      *(undefined4 *)(param_1 + 0x40) = 0x19;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(0);
      iVar8 = 1;
      puVar9 = (undefined4 *)(param_1 + 0x214);
      do {
        FUN_00401ac0(local_50,0x40,"th19_%.2d.rpy",iVar8);
        uVar15 = FUN_005510b0();
        iVar8 = iVar8 + 1;
        *puVar9 = uVar15;
        puVar9 = puVar9 + 1;
      } while (iVar8 < 0x1a);
      return;
    }
    break;
  case 0xd:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x20), iVar8 != 0)) {
        FUN_004d4fa0(1);
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004bb8d0(10,0);
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] == 0) || ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0)
       ) {
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) == 0) {
        return;
      }
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffc;
      FUN_004d5040();
      *(undefined4 *)(param_1 + 0x40) = 5;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      piVar12 = (int *)(param_1 + 0x214);
      local_6c = 0;
      do {
        puVar9 = (undefined4 *)*piVar12;
        if (puVar9 != (undefined4 *)0x0) {
          (**(code **)*puVar9)(0);
          iVar8 = FUN_0046230c(&DAT_0062ad48);
          if (iVar8 != 0) {
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(5);
          }
          if (DAT_0062ad74 == 0x7fffffff) {
            DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(6);
          }
          thunk_FUN_0047a4db(puVar9);
          FUN_0046231d(&DAT_0062ad48);
        }
        local_6c = local_6c + 1;
        *piVar12 = 0;
        piVar12 = piVar12 + 1;
        if (0x18 < local_6c) {
          if (*(int *)(param_1 + 500) == 1) {
            FUN_00416bd0(0x14);
            FUN_0040a670(1);
            FUN_004bb8d0(9,0);
            return;
          }
          FUN_00416bd0(10);
          FUN_004cb4c0();
          if ((DAT_0062b0ac & 0x60) != 0) {
            FUN_00416b50(0);
            FUN_00416b50(3);
          }
          FUN_004bb8d0(9,0);
          return;
        }
      } while( true );
    }
    *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffe | 2;
    FUN_00416bd0(0xe);
    FUN_0040a670(0);
    *(undefined4 *)(param_1 + 0x118) = 0x5b;
    *(undefined4 *)(param_1 + 0x1e0) = 1;
    if ((*(int *)(param_1 + 0x204) == 0) || ((DAT_0062b0ac & 0x60) != 0)) {
      uVar15 = 0;
    }
    else {
      uVar15 = 1;
    }
    FUN_00551aa0(uVar15);
    pbVar10 = (byte *)(param_1 + 0x2dc);
    FUN_0046d433(pbVar10,9,DAT_005d1c00 + 0x2e874);
    *(undefined4 *)(param_1 + 0x200) = 0;
    pbVar4 = &DAT_0059d29c;
    do {
      bVar1 = *pbVar10;
      bVar13 = bVar1 < *pbVar4;
      if (bVar1 != *pbVar4) {
LAB_00538c88:
        uVar5 = -(uint)bVar13 | 1;
        goto LAB_00538c8d;
      }
      if (bVar1 == 0) break;
      bVar1 = pbVar10[1];
      bVar13 = bVar1 < pbVar4[1];
      if (bVar1 != pbVar4[1]) goto LAB_00538c88;
      pbVar10 = pbVar10 + 2;
      pbVar4 = pbVar4 + 2;
    } while (bVar1 != 0);
    uVar5 = 0;
LAB_00538c8d:
    if (uVar5 != 0) {
      FUN_004d4fa0(0xffffffff);
    }
    iVar8 = 8;
    do {
      if (*(char *)(iVar8 + 0x2db + param_1) != ' ') break;
      iVar8 = iVar8 + -1;
    } while (0 < iVar8);
    *(int *)(param_1 + 0x200) = iVar8;
    goto LAB_00538cb8;
  case 0xe:
  case 0x11:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    *(undefined4 *)(param_1 + 0x114) = *(undefined4 *)(param_1 + 0x110);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xfffffff3);
      }
      if ((&DAT_005d19a0)[iVar7] == 0) {
        if (iVar7 < 0) goto LAB_00538801;
      }
      else {
        iVar8 = FUN_00401a60(0x20);
        if (iVar8 != 0) {
          FUN_004d4fa0(0xd);
        }
      }
      if ((&DAT_005d19a0)[iVar7] == 0) {
        if (iVar7 < 0) goto LAB_00538801;
      }
      else {
        iVar8 = FUN_00401a60(0x40);
        if (iVar8 != 0) {
          if (*(int *)(param_1 + 0x110) % 0xd == 0) {
            FUN_004d4fa0(0xc);
          }
          else {
            FUN_004d4fa0(0xffffffff);
          }
        }
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x80), iVar8 != 0)) {
        if (*(int *)(param_1 + 0x110) % 0xd == 0xc) {
          uVar15 = 0xfffffff4;
        }
        else {
          uVar15 = 1;
        }
        FUN_004d4fa0(uVar15);
      }
    }
LAB_00538801:
    if (*(int *)(param_1 + 0x114) != *(int *)(param_1 + 0x110)) {
      FUN_004bb8d0(10,0);
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] == 0) || ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0)
       ) {
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) == 0) {
        return;
      }
      FUN_004bb8d0(9,0);
      if (*(int *)(param_1 + 0x200) != 0) {
        iVar8 = *(int *)(param_1 + 0x200) + -1;
        *(int *)(param_1 + 0x200) = iVar8;
        *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
        return;
      }
      if (*(int *)(param_1 + 0x1fc) != 0xe) {
        return;
      }
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
      uVar15 = 0xd;
      goto LAB_00537fae;
    }
    iVar8 = *(int *)(param_1 + 0x110);
    if (iVar8 < 0x58) {
      iVar7 = *(int *)(param_1 + 0x200);
      if (7 < iVar7) {
        *(char *)(param_1 + 0x2db + iVar7) =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   "
             [iVar8];
        FUN_004bb8d0(7,0);
        return;
      }
      *(char *)(param_1 + 0x2dc + iVar7) =
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   "
           [iVar8];
    }
    else {
      if (iVar8 != 0x58) {
        if (iVar8 == 0x59) {
          if (*(int *)(param_1 + 0x200) == 0) {
            return;
          }
          iVar8 = *(int *)(param_1 + 0x200) + -1;
          *(int *)(param_1 + 0x200) = iVar8;
          *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
          FUN_004bb8d0(9,0);
          return;
        }
        if (iVar8 != 0x5a) goto LAB_00538cb8;
        if (*(int *)(param_1 + 0x1fc) == 0xe) {
          *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
          FUN_004bb8d0(0x11,0);
          FUN_00401ac0(local_50,0x40,"th19_%.2d.rpy",*(int *)(param_1 + 0x38) + 1);
          FUN_00551b70();
          FUN_00551130(local_50,param_1 + 0x2dc,0,1);
          uVar15 = FUN_005510b0();
          *(undefined4 *)(param_1 + 0x214 + *(int *)(param_1 + 0x38) * 4) = uVar15;
          FUN_00416bd0(0xd);
          FUN_0046d433(DAT_005d1c00 + 0x2e874,9,param_1 + 0x2dc);
          FUN_004bb8d0(7,0);
          return;
        }
        FUN_004bb8d0(7,0);
        iVar8 = FUN_0040b870();
        iVar7 = iVar8 * 0x2538 + DAT_005d1c00;
        iVar8 = FUN_0040e510();
        FUN_0046d433(*(int *)(param_1 + 0x38) * 0x28 + 0x22 + iVar8 * 400 + iVar7,9,param_1 + 0x2dc)
        ;
        FUN_0046d433(DAT_005d1c00 + 0x2e874,9,param_1 + 0x2dc);
        goto LAB_00538a88;
      }
      iVar8 = *(int *)(param_1 + 0x200);
      if (7 < iVar8) {
        *(undefined1 *)(param_1 + 0x2db + iVar8) = 0x20;
        FUN_004bb8d0(7,0);
        return;
      }
      *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
    }
    iVar8 = *(int *)(param_1 + 0x200) + 1;
    *(int *)(param_1 + 0x200) = iVar8;
    if (7 < iVar8) {
      FUN_0040a670(0x5a);
      FUN_004bb8d0(7,0);
      return;
    }
LAB_00538cb8:
    FUN_004bb8d0(7,0);
    return;
  case 0x10:
    iVar8 = DAT_005d1bdc;
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004cb480();
      FUN_0051e0f0();
      iVar8 = DAT_005d1bdc;
      *(undefined4 *)(DAT_005d1bdc + 0x13c) = 0x43500000;
    }
    if ((iVar8 != 0) && (*(int *)(iVar8 + 0x138) != 0)) {
      FUN_0051e090();
      FUN_00416bd0(10);
      FUN_004cb4c0();
      return;
    }
    break;
  case 0x12:
    iVar8 = DAT_005d1bf4;
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004cb480();
      local_64 = 156.0;
      local_60 = 0x42c80000;
      local_5c = 0;
      FUN_005372f0();
      iVar8 = DAT_005d1bf4;
      *(undefined4 *)(DAT_005d1bf4 + 0x134) = 1;
    }
    if (*(int *)(param_1 + 0x14) < 0x15) {
      return;
    }
    goto LAB_00538ee1;
  case 0x13:
    iVar8 = *(int *)(param_1 + 0x14);
    bVar13 = false;
    if (iVar8 == 0x14) {
      FUN_004cb480();
      fVar14 = DAT_005aa3c0;
      if (*(int *)(param_1 + 0x400) == 0) {
        fVar14 = DAT_005aa538;
      }
      local_60 = 0x42c80000;
      local_5c = 0;
      local_64 = fVar14 * DAT_005aa280 + DAT_005aa464;
      FUN_004e37a0(0);
      iVar8 = *(int *)(param_1 + 0x14);
      bVar13 = iVar8 == 0x14;
    }
    if (bVar13 || iVar8 < 0x14) {
      return;
    }
    iVar8 = (&DAT_005d1a28)[*(int *)(param_1 + 0x400)];
LAB_00538ee1:
    if (iVar8 == 0) {
      FUN_00416bd0(10);
      FUN_004cb4c0();
      return;
    }
    break;
  case 0x14:
    if (0xb < *(int *)(param_1 + 0x14)) {
      iVar8 = *(int *)(param_1 + 500);
      if (iVar8 == 1) {
        if (*(int *)(DAT_005d1a54 + 0x160) == 0) {
          FUN_004b5920();
          _DAT_0062bb30 = CONCAT44(in_XMM0_Db,in_XMM0_Da);
        }
        iVar7 = DAT_005d1a54;
        local_54 = 2;
        local_58 = 0;
        iVar8 = *(int *)((&DAT_005d1a8c)[*(int *)(param_1 + 0x400) * 0xf] + 8);
        piVar11 = (int *)((&DAT_005d1a8c)[*(int *)(param_1 + 0x400) * 0xf] + 8);
        piVar12 = &local_58;
        if (-1 < iVar8) {
          piVar12 = piVar11;
        }
        piVar6 = &local_54;
        if (iVar8 < 3) {
          piVar6 = piVar12;
        }
        iVar8 = *piVar6;
        *piVar11 = iVar8;
        if (iVar8 == 1) {
          (&DAT_006240fc)[*(int *)(param_1 + 0x400) * 0xb0] = 1;
        }
        *(uint *)(iVar7 + 0x140) = *(uint *)(iVar7 + 0x140) & 0xffffffef;
        DAT_005c6418 = *(undefined4 *)(param_1 + 0x2e8);
        if (*(int *)(DAT_005d1a50 + 0x310) != 0) {
          FUN_0050fac0();
        }
        iVar8 = DAT_005d1a50;
        DAT_0062eef0 = *(undefined4 *)(param_1 + 0x210);
        iVar7 = FUN_004cb8b0(*(undefined4 *)(DAT_005d1a50 + 0x5c));
        if (iVar7 != 0) {
          *(undefined1 *)(iVar7 + 0x4b5) = 1;
          for (iVar7 = *(int *)(iVar7 + 0x540); iVar7 != 0; iVar7 = *(int *)(iVar7 + 4)) {
            iVar7 = FUN_004cb450();
          }
        }
        iVar8 = FUN_004cb8b0(*(undefined4 *)(iVar8 + 200));
        if (iVar8 != 0) {
          *(undefined1 *)(iVar8 + 0x4b5) = 1;
          for (iVar8 = *(int *)(iVar8 + 0x540); iVar8 != 0; iVar8 = *(int *)(iVar8 + 4)) {
            iVar8 = FUN_004cb450();
          }
        }
      }
      else if ((iVar8 == 2) || (iVar8 == 3)) {
        if (*(int *)(DAT_005d1a54 + 0x160) == 0) {
          FUN_004b5920();
          _DAT_0062bb30 = CONCAT44(in_XMM0_Db,in_XMM0_Da);
        }
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        DAT_005c6418 = *(undefined4 *)(param_1 + 0x2e8);
      }
      if (*(int *)(param_1 + 0x38) == 0) {
        if (*(int *)(param_1 + 500) == 1) {
          piVar12 = &DAT_00626ecc;
          do {
            piVar11 = (int *)piVar12[-2];
            if ((piVar11 != (int *)0x0) && (piVar12[3] != 0)) {
              (**(code **)(*piVar11 + 0x30))(piVar11,0,0,*(undefined4 *)(*piVar12 + 0xc));
            }
            piVar12 = piVar12 + 6;
          } while ((int)piVar12 < 0x62773c);
          FUN_004babe0(7,0,"UnPause");
        }
        else if (*(int *)(param_1 + 500) == 2) {
          DAT_0062c450 = (uint)(*(int *)(param_1 + 0x204) == 0) * 4 + 10;
        }
      }
      else if (*(int *)(param_1 + 0x38) == 1) {
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffffe) + 4;
      }
      FUN_00416ca0(0);
    }
  }
  return;
}


// Address 538992

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00537d40(int param_1)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  int *piVar6;
  int iVar7;
  int iVar8;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined4 extraout_ECX_02;
  undefined4 extraout_ECX_03;
  undefined4 extraout_ECX_04;
  undefined4 extraout_ECX_05;
  undefined4 extraout_ECX_06;
  undefined4 extraout_ECX_07;
  undefined4 extraout_ECX_08;
  undefined4 extraout_ECX_09;
  undefined4 extraout_ECX_10;
  undefined4 extraout_ECX_11;
  undefined4 extraout_ECX_12;
  undefined4 extraout_ECX_13;
  undefined4 extraout_ECX_14;
  undefined4 extraout_ECX_15;
  undefined4 extraout_ECX_16;
  undefined4 extraout_ECX_17;
  int extraout_ECX_18;
  int extraout_ECX_19;
  int extraout_ECX_20;
  int extraout_ECX_21;
  int extraout_ECX_22;
  undefined4 extraout_ECX_23;
  undefined4 extraout_ECX_24;
  undefined4 *puVar9;
  byte *pbVar10;
  int *piVar11;
  int *piVar12;
  bool bVar13;
  undefined4 in_XMM0_Da;
  float fVar14;
  undefined4 in_XMM0_Db;
  undefined4 uVar15;
  int local_6c;
  undefined1 local_68 [4];
  float local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  int local_54;
  undefined1 local_50 [68];
  uint local_c;

  local_c = DAT_005c3d80 ^ (uint)&local_6c;
  iVar8 = *(int *)(param_1 + 0x400);
  iVar7 = *(int *)(DAT_005d19b0 + 0x2e30 + iVar8 * 4);
  switch(*(undefined4 *)(param_1 + 0x1fc)) {
  case 0:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    FUN_00416bd0(10);
    iVar8 = DAT_005d1a54;
    *(undefined4 *)(param_1 + 0x40) = 5;
    if (*(int *)(iVar8 + 0x160) != 0) {
      FUN_00416b50(3);
    }
    iVar8 = FUN_00412af0();
    if (0 < iVar8) {
      FUN_004cb7a0(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x42,extraout_ECX);
      FUN_004cb7a0(*puVar9,5);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4f,extraout_ECX_00);
      FUN_004cb7a0(*puVar9,5);
    }
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x404) + 0x20) + 0x2c) == 0) {
      FUN_00416b50(2);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3c,extraout_ECX_01);
      FUN_004cb7a0(*puVar9,5);
    }
    cVar2 = FUN_00412310();
    if (cVar2 != '\0') {
      FUN_00416b50(4);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3e,extraout_ECX_02);
      FUN_004cb7a0(*puVar9,5);
      FUN_00416b50(3);
      puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3d,extraout_ECX_03);
      FUN_004cb7a0(*puVar9,5);
    }
    *(undefined4 *)(param_1 + 0x108) = 1;
    FUN_0040a670(0);
    goto LAB_00537eac;
  case 1:
    if (9 < *(int *)(param_1 + 0x14)) {
      FUN_00416bd0(6);
      *(undefined4 *)(param_1 + 0x40) = 5;
      FUN_00416b50(3);
      FUN_00416b50(0);
      FUN_00416b50(2);
      FUN_00416b50(4);
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      *(undefined4 *)(param_1 + 0x20c) = 1;
      return;
    }
    break;
  case 2:
  case 3:
  case 4:
  case 5:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
LAB_00538a88:
    FUN_00416bd0(6);
    *(undefined4 *)(param_1 + 0x40) = 5;
    *(undefined4 *)(param_1 + 0x108) = 1;
    FUN_00416b50(2);
    FUN_00416b50(3);
    FUN_00416b50(4);
    FUN_0040a670(0);
    *(undefined4 *)(param_1 + 0x108) = 1;
LAB_00537eac:
    FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
    *(undefined4 *)(param_1 + 0x20c) = 0;
    return;
  case 6:
    goto switchD_00537d7a_caseD_6;
  case 7:
    if (*(int *)(param_1 + 0x14) < 0x3c) {
      return;
    }
    if (iVar7 < 0) {
      return;
    }
    if ((&DAT_005d19a0)[iVar7] == 0) {
      return;
    }
    if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0) {
      return;
    }
    goto LAB_00537fa2;
  case 8:
  case 0xb:
    if (*(int *)(param_1 + 0x14) < 0x14) {
      return;
    }
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004d5090();
      *(undefined4 *)(param_1 + 0x40) = 2;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),0xe);
      iVar8 = extraout_ECX_18;
    }
    if (*(int *)(param_1 + 0x14) < 0x1e) {
      return;
    }
    if (*(int *)(param_1 + 0x14) == 0x1e) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 0xf));
      iVar8 = extraout_ECX_19;
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      iVar8 = 0;
      if (((&DAT_005d19a0)[iVar7] != 0) &&
         (iVar3 = FUN_00401a60(0x20), iVar8 = extraout_ECX_20, iVar3 != 0)) {
        FUN_004d4fa0(1);
        iVar8 = extraout_ECX_21;
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),
                   (int)(short)((short)*(int *)(param_1 + 0x38) + 0xf));
      FUN_004bb8d0(10,0);
      iVar8 = extraout_ECX_22;
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] != 0) && ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) != 0)
       ) {
      if (*(int *)(param_1 + 0x38) == 0) {
        piVar12 = (int *)FUN_004cafa0(local_68,0x54,iVar8);
        if (*piVar12 != 0) {
          puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x54,extraout_ECX_23);
          FUN_004cb830(*puVar9,6);
        }
        uVar15 = 0xc;
        if (*(int *)(param_1 + 0x1fc) != 0xb) {
          uVar15 = 9;
        }
        FUN_00416bd0(uVar15);
        uVar15 = 7;
      }
      else {
        if (*(int *)(param_1 + 0x38) != 1) goto LAB_00538550;
        puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x55,iVar8);
        FUN_004cb830(*puVar9,6);
        FUN_00416bd0(9);
        uVar15 = 9;
      }
      FUN_004bb8d0(uVar15,0);
    }
LAB_00538550:
    if ((&DAT_005d19a0)[iVar7] == 0) {
      if (iVar7 < 0) {
        return;
      }
    }
    else if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) != 0) {
      FUN_004bb8d0(9,0);
      if (*(int *)(param_1 + 0x38) == 0) {
        FUN_0040a670(1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 0xf)
                    );
      }
      else if (*(int *)(param_1 + 0x38) == 1) {
        puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x55,extraout_ECX_24);
        FUN_004cb830(*puVar9,6);
        FUN_00416bd0(9);
      }
    }
    if ((&DAT_005d19a0)[iVar7] == 0) {
      return;
    }
    if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x100) == 0) {
      return;
    }
    if (*(int *)(param_1 + 500) != 1) {
      return;
    }
    goto LAB_005385fa;
  case 9:
    if (*(int *)(param_1 + 0x14) < 0x14) {
      return;
    }
    if (*(int *)(param_1 + 0x38) != 0) {
      if (*(int *)(param_1 + 0x38) != 1) {
        return;
      }
      FUN_004d5040();
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      uVar15 = 10;
      goto LAB_00537fae;
    }
    FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
    FUN_004d5040();
    goto LAB_00537fac;
  case 10:
    FUN_00416bd0(6);
    if (*(int *)(DAT_005d1a54 + 0x160) != 0) {
      FUN_00416b50(3);
    }
    iVar8 = FUN_00412af0();
    if (0 < iVar8) {
      FUN_004cb7a0(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x42,extraout_ECX_04);
      FUN_004cb7a0(*puVar9,5);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x4f,extraout_ECX_05);
      FUN_004cb7a0(*puVar9,5);
    }
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x404) + 0x20) + 0x2c) == 0) {
      FUN_00416b50(2);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3c,extraout_ECX_06);
      FUN_004cb7a0(*puVar9,5);
    }
    cVar2 = FUN_00412310();
    if (cVar2 != '\0') {
      FUN_00416b50(4);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3e,extraout_ECX_07);
      FUN_004cb7a0(*puVar9,5);
      FUN_00416b50(3);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3d,extraout_ECX_08);
      FUN_004cb7a0(*puVar9,5);
    }
switchD_00537d7a_caseD_6:
    if (((*(int *)(param_1 + 0x14) < 2) && (*(int *)(param_1 + 500) == 2)) && (DAT_0062b0b4 < 1)) {
      FUN_00416b50(0);
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x20), iVar8 != 0)) {
        FUN_004d4fa0(1);
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),
                   (int)(short)((short)*(int *)(param_1 + 0x38) + 7));
      FUN_004bb8d0(10,0);
    }
    if (((-1 < iVar7) && ((&DAT_005d19a0)[iVar7] != 0)) &&
       ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) != 0)) {
      FUN_004bb8d0(7,0);
      switch(*(undefined4 *)(param_1 + 0x38)) {
      case 0:
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        uVar15 = 0x14;
        break;
      case 1:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3b,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x41,extraout_ECX_10);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x48,extraout_ECX_11);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4c,extraout_ECX_12);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x4e,extraout_ECX_13);
        FUN_004cb830(*puVar9,6);
        if ((*(int *)(DAT_005d1a54 + 0x160) != 0) || (uVar15 = 8, *(int *)(param_1 + 500) != 1)) {
          uVar15 = 0x14;
        }
        break;
      case 2:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3c,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x43,extraout_ECX_16);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x13;
        break;
      case 3:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3d,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x44,extraout_ECX_14);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x10;
        break;
      case 4:
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x3e,extraout_ECX_09);
        FUN_004cb830(*puVar9,6);
        puVar9 = (undefined4 *)FUN_004cafa0(&local_6c,0x45,extraout_ECX_15);
        FUN_004cb830(*puVar9,6);
        uVar15 = 0x12;
        break;
      default:
        goto switchD_0053819e_default;
      }
      FUN_00416bd0(uVar15);
switchD_0053819e_default:
      FUN_00402650(0);
    }
    if (*(int *)(param_1 + 0x20c) == 0) {
      if (iVar7 < 0) {
        return;
      }
      if ((&DAT_005d19a0)[iVar7] == 0) goto LAB_0053837b;
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x100) == 0) goto LAB_00538373;
LAB_005385fa:
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
      uVar15 = 0;
    }
    else {
LAB_00538373:
      if (iVar7 < 0) {
        return;
      }
LAB_0053837b:
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x10000) == 0) {
        return;
      }
      FUN_004bb8d0(7,0);
      puVar9 = (undefined4 *)FUN_004cafa0(local_68,0x3b,extraout_ECX_17);
      FUN_004cb830(*puVar9,6);
LAB_00537fa2:
      uVar15 = 1;
    }
    FUN_0040a670(uVar15);
LAB_00537fac:
    uVar15 = 0x14;
LAB_00537fae:
    FUN_00416bd0(uVar15);
    return;
  case 0xc:
    if (0x13 < *(int *)(param_1 + 0x14)) {
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
      FUN_00416bd0(0xd);
      FUN_004cb480();
      FUN_004d5090();
      *(undefined4 *)(param_1 + 0x40) = 0x19;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_0040a670(0);
      iVar8 = 1;
      puVar9 = (undefined4 *)(param_1 + 0x214);
      do {
        FUN_00401ac0(local_50,0x40,"th19_%.2d.rpy",iVar8);
        uVar15 = FUN_005510b0();
        iVar8 = iVar8 + 1;
        *puVar9 = uVar15;
        puVar9 = puVar9 + 1;
      } while (iVar8 < 0x1a);
      return;
    }
    break;
  case 0xd:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    *(undefined4 *)(param_1 + 0x3c) = *(undefined4 *)(param_1 + 0x38);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xffffffff);
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x20), iVar8 != 0)) {
        FUN_004d4fa0(1);
      }
    }
    if (*(int *)(param_1 + 0x3c) != *(int *)(param_1 + 0x38)) {
      FUN_004bb8d0(10,0);
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] == 0) || ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0)
       ) {
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) == 0) {
        return;
      }
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffc;
      FUN_004d5040();
      *(undefined4 *)(param_1 + 0x40) = 5;
      *(undefined4 *)(param_1 + 0x108) = 1;
      FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),(int)(short)(*(short *)(param_1 + 0x38) + 7));
      piVar12 = (int *)(param_1 + 0x214);
      local_6c = 0;
      do {
        puVar9 = (undefined4 *)*piVar12;
        if (puVar9 != (undefined4 *)0x0) {
          (**(code **)*puVar9)(0);
          iVar8 = FUN_0046230c(&DAT_0062ad48);
          if (iVar8 != 0) {
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(5);
          }
          if (DAT_0062ad74 == 0x7fffffff) {
            DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(6);
          }
          thunk_FUN_0047a4db(puVar9);
          FUN_0046231d(&DAT_0062ad48);
        }
        local_6c = local_6c + 1;
        *piVar12 = 0;
        piVar12 = piVar12 + 1;
        if (0x18 < local_6c) {
          if (*(int *)(param_1 + 500) == 1) {
            FUN_00416bd0(0x14);
            FUN_0040a670(1);
            FUN_004bb8d0(9,0);
            return;
          }
          FUN_00416bd0(10);
          FUN_004cb4c0();
          if ((DAT_0062b0ac & 0x60) != 0) {
            FUN_00416b50(0);
            FUN_00416b50(3);
          }
          FUN_004bb8d0(9,0);
          return;
        }
      } while( true );
    }
    *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffe | 2;
    FUN_00416bd0(0xe);
    FUN_0040a670(0);
    *(undefined4 *)(param_1 + 0x118) = 0x5b;
    *(undefined4 *)(param_1 + 0x1e0) = 1;
    if ((*(int *)(param_1 + 0x204) == 0) || ((DAT_0062b0ac & 0x60) != 0)) {
      uVar15 = 0;
    }
    else {
      uVar15 = 1;
    }
    FUN_00551aa0(uVar15);
    pbVar10 = (byte *)(param_1 + 0x2dc);
    FUN_0046d433(pbVar10,9,DAT_005d1c00 + 0x2e874);
    *(undefined4 *)(param_1 + 0x200) = 0;
    pbVar4 = &DAT_0059d29c;
    do {
      bVar1 = *pbVar10;
      bVar13 = bVar1 < *pbVar4;
      if (bVar1 != *pbVar4) {
LAB_00538c88:
        uVar5 = -(uint)bVar13 | 1;
        goto LAB_00538c8d;
      }
      if (bVar1 == 0) break;
      bVar1 = pbVar10[1];
      bVar13 = bVar1 < pbVar4[1];
      if (bVar1 != pbVar4[1]) goto LAB_00538c88;
      pbVar10 = pbVar10 + 2;
      pbVar4 = pbVar4 + 2;
    } while (bVar1 != 0);
    uVar5 = 0;
LAB_00538c8d:
    if (uVar5 != 0) {
      FUN_004d4fa0(0xffffffff);
    }
    iVar8 = 8;
    do {
      if (*(char *)(iVar8 + 0x2db + param_1) != ' ') break;
      iVar8 = iVar8 + -1;
    } while (0 < iVar8);
    *(int *)(param_1 + 0x200) = iVar8;
    goto LAB_00538cb8;
  case 0xe:
  case 0x11:
    if (*(int *)(param_1 + 0x14) < 10) {
      return;
    }
    *(undefined4 *)(param_1 + 0x114) = *(undefined4 *)(param_1 + 0x110);
    if (-1 < iVar7) {
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x10), iVar8 != 0)) {
        FUN_004d4fa0(0xfffffff3);
      }
      if ((&DAT_005d19a0)[iVar7] == 0) {
        if (iVar7 < 0) goto LAB_00538801;
      }
      else {
        iVar8 = FUN_00401a60(0x20);
        if (iVar8 != 0) {
          FUN_004d4fa0(0xd);
        }
      }
      if ((&DAT_005d19a0)[iVar7] == 0) {
        if (iVar7 < 0) goto LAB_00538801;
      }
      else {
        iVar8 = FUN_00401a60(0x40);
        if (iVar8 != 0) {
          if (*(int *)(param_1 + 0x110) % 0xd == 0) {
            FUN_004d4fa0(0xc);
          }
          else {
            FUN_004d4fa0(0xffffffff);
          }
        }
      }
      if (((&DAT_005d19a0)[iVar7] != 0) && (iVar8 = FUN_00401a60(0x80), iVar8 != 0)) {
        if (*(int *)(param_1 + 0x110) % 0xd == 0xc) {
          uVar15 = 0xfffffff4;
        }
        else {
          uVar15 = 1;
        }
        FUN_004d4fa0(uVar15);
      }
    }
LAB_00538801:
    if (*(int *)(param_1 + 0x114) != *(int *)(param_1 + 0x110)) {
      FUN_004bb8d0(10,0);
    }
    if (iVar7 < 0) {
      return;
    }
    if (((&DAT_005d19a0)[iVar7] == 0) || ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x80001) == 0)
       ) {
      if ((&DAT_005d19a0)[iVar7] == 0) {
        return;
      }
      if ((*(uint *)((&DAT_005d19a0)[iVar7] + 0x10) & 0x106) == 0) {
        return;
      }
      FUN_004bb8d0(9,0);
      if (*(int *)(param_1 + 0x200) != 0) {
        iVar8 = *(int *)(param_1 + 0x200) + -1;
        *(int *)(param_1 + 0x200) = iVar8;
        *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
        return;
      }
      if (*(int *)(param_1 + 0x1fc) != 0xe) {
        return;
      }
      *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
      uVar15 = 0xd;
      goto LAB_00537fae;
    }
    iVar8 = *(int *)(param_1 + 0x110);
    if (iVar8 < 0x58) {
      iVar7 = *(int *)(param_1 + 0x200);
      if (7 < iVar7) {
        *(char *)(param_1 + 0x2db + iVar7) =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   "
             [iVar8];
        FUN_004bb8d0(7,0);
        return;
      }
      *(char *)(param_1 + 0x2dc + iVar7) =
           "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   "
           [iVar8];
    }
    else {
      if (iVar8 != 0x58) {
        if (iVar8 == 0x59) {
          if (*(int *)(param_1 + 0x200) == 0) {
            return;
          }
          iVar8 = *(int *)(param_1 + 0x200) + -1;
          *(int *)(param_1 + 0x200) = iVar8;
          *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
          FUN_004bb8d0(9,0);
          return;
        }
        if (iVar8 != 0x5a) goto LAB_00538cb8;
        if (*(int *)(param_1 + 0x1fc) == 0xe) {
          *(uint *)(param_1 + 0x3f8) = *(uint *)(param_1 + 0x3f8) & 0xfffffffd | 1;
          FUN_004bb8d0(0x11,0);
          FUN_00401ac0(local_50,0x40,"th19_%.2d.rpy",*(int *)(param_1 + 0x38) + 1);
          FUN_00551b70();
          FUN_00551130(local_50,param_1 + 0x2dc,0,1);
          uVar15 = FUN_005510b0();
          *(undefined4 *)(param_1 + 0x214 + *(int *)(param_1 + 0x38) * 4) = uVar15;
          FUN_00416bd0(0xd);
          FUN_0046d433(DAT_005d1c00 + 0x2e874,9,param_1 + 0x2dc);
          FUN_004bb8d0(7,0);
          return;
        }
        FUN_004bb8d0(7,0);
        iVar8 = FUN_0040b870();
        iVar7 = iVar8 * 0x2538 + DAT_005d1c00;
        iVar8 = FUN_0040e510();
        FUN_0046d433(*(int *)(param_1 + 0x38) * 0x28 + 0x22 + iVar8 * 400 + iVar7,9,param_1 + 0x2dc)
        ;
        FUN_0046d433(DAT_005d1c00 + 0x2e874,9,param_1 + 0x2dc);
        goto LAB_00538a88;
      }
      iVar8 = *(int *)(param_1 + 0x200);
      if (7 < iVar8) {
        *(undefined1 *)(param_1 + 0x2db + iVar8) = 0x20;
        FUN_004bb8d0(7,0);
        return;
      }
      *(undefined1 *)(param_1 + 0x2dc + iVar8) = 0x20;
    }
    iVar8 = *(int *)(param_1 + 0x200) + 1;
    *(int *)(param_1 + 0x200) = iVar8;
    if (7 < iVar8) {
      FUN_0040a670(0x5a);
      FUN_004bb8d0(7,0);
      return;
    }
LAB_00538cb8:
    FUN_004bb8d0(7,0);
    return;
  case 0x10:
    iVar8 = DAT_005d1bdc;
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004cb480();
      FUN_0051e0f0();
      iVar8 = DAT_005d1bdc;
      *(undefined4 *)(DAT_005d1bdc + 0x13c) = 0x43500000;
    }
    if ((iVar8 != 0) && (*(int *)(iVar8 + 0x138) != 0)) {
      FUN_0051e090();
      FUN_00416bd0(10);
      FUN_004cb4c0();
      return;
    }
    break;
  case 0x12:
    iVar8 = DAT_005d1bf4;
    if (*(int *)(param_1 + 0x14) == 0x14) {
      FUN_004cb480();
      local_64 = 156.0;
      local_60 = 0x42c80000;
      local_5c = 0;
      FUN_005372f0();
      iVar8 = DAT_005d1bf4;
      *(undefined4 *)(DAT_005d1bf4 + 0x134) = 1;
    }
    if (*(int *)(param_1 + 0x14) < 0x15) {
      return;
    }
    goto LAB_00538ee1;
  case 0x13:
    iVar8 = *(int *)(param_1 + 0x14);
    bVar13 = false;
    if (iVar8 == 0x14) {
      FUN_004cb480();
      fVar14 = DAT_005aa3c0;
      if (*(int *)(param_1 + 0x400) == 0) {
        fVar14 = DAT_005aa538;
      }
      local_60 = 0x42c80000;
      local_5c = 0;
      local_64 = fVar14 * DAT_005aa280 + DAT_005aa464;
      FUN_004e37a0(0);
      iVar8 = *(int *)(param_1 + 0x14);
      bVar13 = iVar8 == 0x14;
    }
    if (bVar13 || iVar8 < 0x14) {
      return;
    }
    iVar8 = (&DAT_005d1a28)[*(int *)(param_1 + 0x400)];
LAB_00538ee1:
    if (iVar8 == 0) {
      FUN_00416bd0(10);
      FUN_004cb4c0();
      return;
    }
    break;
  case 0x14:
    if (0xb < *(int *)(param_1 + 0x14)) {
      iVar8 = *(int *)(param_1 + 500);
      if (iVar8 == 1) {
        if (*(int *)(DAT_005d1a54 + 0x160) == 0) {
          FUN_004b5920();
          _DAT_0062bb30 = CONCAT44(in_XMM0_Db,in_XMM0_Da);
        }
        iVar7 = DAT_005d1a54;
        local_54 = 2;
        local_58 = 0;
        iVar8 = *(int *)((&DAT_005d1a8c)[*(int *)(param_1 + 0x400) * 0xf] + 8);
        piVar11 = (int *)((&DAT_005d1a8c)[*(int *)(param_1 + 0x400) * 0xf] + 8);
        piVar12 = &local_58;
        if (-1 < iVar8) {
          piVar12 = piVar11;
        }
        piVar6 = &local_54;
        if (iVar8 < 3) {
          piVar6 = piVar12;
        }
        iVar8 = *piVar6;
        *piVar11 = iVar8;
        if (iVar8 == 1) {
          (&DAT_006240fc)[*(int *)(param_1 + 0x400) * 0xb0] = 1;
        }
        *(uint *)(iVar7 + 0x140) = *(uint *)(iVar7 + 0x140) & 0xffffffef;
        DAT_005c6418 = *(undefined4 *)(param_1 + 0x2e8);
        if (*(int *)(DAT_005d1a50 + 0x310) != 0) {
          FUN_0050fac0();
        }
        iVar8 = DAT_005d1a50;
        DAT_0062eef0 = *(undefined4 *)(param_1 + 0x210);
        iVar7 = FUN_004cb8b0(*(undefined4 *)(DAT_005d1a50 + 0x5c));
        if (iVar7 != 0) {
          *(undefined1 *)(iVar7 + 0x4b5) = 1;
          for (iVar7 = *(int *)(iVar7 + 0x540); iVar7 != 0; iVar7 = *(int *)(iVar7 + 4)) {
            iVar7 = FUN_004cb450();
          }
        }
        iVar8 = FUN_004cb8b0(*(undefined4 *)(iVar8 + 200));
        if (iVar8 != 0) {
          *(undefined1 *)(iVar8 + 0x4b5) = 1;
          for (iVar8 = *(int *)(iVar8 + 0x540); iVar8 != 0; iVar8 = *(int *)(iVar8 + 4)) {
            iVar8 = FUN_004cb450();
          }
        }
      }
      else if ((iVar8 == 2) || (iVar8 == 3)) {
        if (*(int *)(DAT_005d1a54 + 0x160) == 0) {
          FUN_004b5920();
          _DAT_0062bb30 = CONCAT44(in_XMM0_Db,in_XMM0_Da);
        }
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        DAT_005c6418 = *(undefined4 *)(param_1 + 0x2e8);
      }
      if (*(int *)(param_1 + 0x38) == 0) {
        if (*(int *)(param_1 + 500) == 1) {
          piVar12 = &DAT_00626ecc;
          do {
            piVar11 = (int *)piVar12[-2];
            if ((piVar11 != (int *)0x0) && (piVar12[3] != 0)) {
              (**(code **)(*piVar11 + 0x30))(piVar11,0,0,*(undefined4 *)(*piVar12 + 0xc));
            }
            piVar12 = piVar12 + 6;
          } while ((int)piVar12 < 0x62773c);
          FUN_004babe0(7,0,"UnPause");
        }
        else if (*(int *)(param_1 + 500) == 2) {
          DAT_0062c450 = (uint)(*(int *)(param_1 + 0x204) == 0) * 4 + 10;
        }
      }
      else if (*(int *)(param_1 + 0x38) == 1) {
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1ec),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1f0),1);
        FUN_004cb830(*(undefined4 *)(param_1 + 0x1e8),1);
        DAT_0062c450 = (-(uint)((DAT_0062c48c & 0x200) != 0) & 0xfffffffe) + 4;
      }
      FUN_00416ca0(0);
    }
  }
  return;
}
