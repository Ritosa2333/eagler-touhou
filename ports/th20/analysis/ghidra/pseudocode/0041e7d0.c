/* Entry: 0x0041e7d0; symbol: FUN_0041e7d0; body bytes: 2535 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0041e7d0(HINSTANCE param_1)

{
  char **cx;
  undefined4 *puVar1;
  int iVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  BOOL BVar7;
  int unaff_ESI;
  float10 fVar8;
  undefined4 *local_1038;
  int local_1030;
  int local_102c;
  tagMSG local_1028;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_100c [4];
  char local_1008 [4096];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_102c = 0;
  _DAT_005b6764 = param_1;
  _DAT_005b67c4 = 0;
  timeBeginPeriod(1);
  FUN_0054a4e0(0);
  FUN_0041ccc0(0x5c0240);
  puVar1 = (undefined4 *)operator_new(8);
  if (puVar1 == (undefined4 *)0x0) {
    local_1038 = (undefined4 *)0x0;
  }
  else {
    local_1038 = FUN_0041f5b0(puVar1);
  }
  DAT_005b8894 = local_1038;
  FUN_00418db0(local_100c);
  FUN_0041e770(local_100c);
  FUN_00454150(&DAT_005c0678,
               L"貓ﮕ꺓\xec8d䲋庘ⴠⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭⴭഠ\n"
              );
  iVar2 = FUN_0041c020();
  if (iVar2 != -1) {
    FUN_004117a0(&DAT_005c4d40,param_1);
    iVar2 = FUN_0041b4f0();
    if (iVar2 == 0) {
      iVar2 = FUN_004dc1c0("th20.cfg");
      if (iVar2 == 0) {
        FUN_00420f80();
        FUN_00421040();
        FUN_0041ae70();
        if ((DAT_005b87e8 >> 3 & 3) == 0) {
          _DAT_005b87e4 = DAT_005c4f80;
          eof(unaff_ESI);
          FUN_00416d20();
          goto LAB_0041e93d;
        }
      }
    }
    else {
      FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056c988);
    }
  }
  do {
    do {
      FUN_00426170((undefined4 *)&DAT_005ba830);
      FUN_00418730(DAT_005c0028);
      DAT_005c0028 = (undefined1 *)0x0;
      if (DAT_005c4edc != (int *)0x0) {
        (**(code **)(*DAT_005c4edc + 8))(DAT_005c4edc);
        DAT_005c4edc = (int *)0x0;
      }
      if (DAT_005c4ee0 != (int *)0x0) {
        (**(code **)(*DAT_005c4ee0 + 8))(DAT_005c4ee0);
        DAT_005c4ee0 = (int *)0x0;
      }
      if (DAT_005c4ee4 != (int *)0x0) {
        (**(code **)(*DAT_005c4ee4 + 8))(DAT_005c4ee4);
        DAT_005c4ee4 = (int *)0x0;
      }
      CDocObjectServer::ReleaseDocSite((CDocObjectServer *)&DAT_005c4d40);
      CDocObjectServer::ReleaseDocSite((CDocObjectServer *)&DAT_005c4d40);
      if (DAT_005b6758 != (HWND)0x0) {
        ShowWindow(DAT_005b6758,0);
        DestroyWindow(DAT_005b6758);
        DAT_005b6758 = (HWND)0x0;
      }
      do {
        iVar2 = ShowCursor(1);
      } while (iVar2 < 0);
      if (local_102c != 2) {
        FUN_0041aa70(&DAT_005c4f08);
        timeEndPeriod(1);
        FUN_00548610(local_1008,0x1000,0x5b67e1);
        FUN_005486a0(local_1008,0x1000,0x56ca74);
        FUN_00453220(&DAT_005c0678);
        FUN_0041b490();
        if (DAT_005b8894 != (undefined4 *)0x0) {
          FUN_00419320(DAT_005b8894,1);
        }
        FUN_0041ca30(0x5c0240);
        FUN_00418e90((undefined4 *)local_100c);
        FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
        return;
      }
      FUN_00419be0(&DAT_005c0678);
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056ca3c);
      iVar2 = FUN_00415800(0x5c4d40);
      if (iVar2 == 0) {
        WINNLSEnableIME(0,1);
      }
      for (local_1030 = 0; local_1030 < 0x3c; local_1030 = local_1030 + 1) {
        BVar7 = PeekMessageW(&local_1028,(HWND)0x0,0,0,1);
        if (BVar7 != 0) {
          TranslateMessage(&local_1028);
          DispatchMessageW(&local_1028);
        }
      }
      DAT_005c5888 = DAT_005c5888 & 0xffffff9f;
LAB_0041e93d:
      iVar2 = FUN_0041c320();
    } while (((iVar2 != 0) || (iVar2 = FUN_0041ccf0(&DAT_005b6758,param_1), iVar2 != 0)) ||
            (iVar2 = FUN_0041c3e0(), iVar2 != 0));
    DAT_005b66d8 = FUN_004187a0();
    DAT_005c0028 = FUN_00418830();
    iVar2 = FUN_00415800(0x5c4d40);
    if ((iVar2 == 0) || (iVar2 = FUN_0041d0f0(0x5c4d40), iVar2 != 0)) {
      WINNLSEnableIME(0,0);
      do {
        iVar2 = ShowCursor(0);
      } while (-1 < iVar2);
      SetCursor((HCURSOR)0x0);
    }
    DAT_005b8838 = 0;
    fVar8 = (float10)FUN_0041cb10();
    DAT_005b8830 = (double)fVar8;
    DAT_005b8820 = DAT_005b8830;
    _DAT_005b8828 = DAT_005b8830;
    fVar8 = (float10)FUN_0041cb10();
    DAT_005b8848 = (double)fVar8;
    _DAT_005b8840 = DAT_005b8848;
    FUN_0041b480(DAT_005b6758);
    iVar2 = FUN_004de1f0();
    if (iVar2 == 0) {
      local_102c = 0;
      FUN_0041dcc0(&DAT_005b6758,0xfc);
      DAT_005b87e8 = DAT_005b87e8 | 1;
LAB_0041ea9c:
      if (DAT_005b6760 == 0) {
        BVar7 = PeekMessageW(&local_1028,(HWND)0x0,0,0,1);
        if (BVar7 == 0) {
          iVar2 = FUN_00412730(0x5c4d40);
          if (iVar2 != 0) {
            piVar3 = (int *)FUN_00412730(0x5c4d40);
            iVar2 = (**(code **)(*piVar3 + 0xc))(piVar3);
            if (iVar2 == 0) {
              uVar4 = FUN_0041d080(0x5b6758);
              if (uVar4 == 0) {
                if ((DAT_005b87e8 >> 2 & 1) == 0) {
                  if ((DAT_005c4e58 == 1) && (DAT_005c4f84 == '\0')) {
                    local_102c = FUN_0041a030(0x5b6758);
                  }
                  else {
                    local_102c = FUN_00419de0(0x5b6758);
                  }
                }
                else {
                  local_102c = FUN_00419c20(0x5b6758);
                }
                if (local_102c != 0) goto LAB_0041eef5;
                DAT_005c5888 = DAT_005c5888 & 0xfffffff7;
                goto LAB_0041ea9c;
              }
            }
            else if (iVar2 != -0x7789f797) goto LAB_0041ea9c;
            FUN_0041de30(&DAT_005b6758,10);
            uVar4 = FUN_0041d080(0x5b6758);
            if (uVar4 != 0) {
              iVar2 = FUN_0041cc70(0x5b6758);
              if (iVar2 < 3) {
                GetWindowRect(DAT_005b6758,(LPRECT)&DAT_005c4d4c);
                _DAT_005c4e44 = 0;
                if (DAT_005c4f7c == '\0') {
                  DAT_005c4e2c = 0x15;
                }
                else {
                  DAT_005c4e2c = 0x17;
                }
              }
              else {
                DAT_005c4e2c = DAT_005c4ed8;
                _DAT_005c4e54 = 0;
                _DAT_005c4e44 = 1;
              }
              FUN_0041e050(&DAT_005b6758,0);
            }
            FUN_004dd840(&DAT_005c4d40);
            FUN_0041dbe0((int)DAT_005c0028);
            iVar2 = FUN_0041c730(1);
            if (iVar2 != 0) goto LAB_0041eef5;
            FUN_0041a2c0();
            FUN_0041d9f0((int)DAT_005c0028);
            _DAT_005c586c = 3;
            DAT_005c5888 = DAT_005c5888 | 8;
            uVar4 = FUN_0041d080(0x5b6758);
            if (uVar4 != 0) {
              FUN_004dbce0();
              iVar2 = FUN_0041cc70(0x5b6758);
              if ((iVar2 == 8) || (iVar2 = FUN_0041cc70(0x5b6758), iVar2 == 9)) {
                SetWindowLongW(DAT_005b6758,-0x10,0);
                ShowWindow(DAT_005b6758,5);
                iVar2 = DAT_005b8804;
                cx = DAT_005b8800;
                std::_Adl_verify_range<char*,char_const*>((char **)"%d, %d\n",DAT_005b8800);
                SetWindowPos(DAT_005b6758,(HWND)0x0,0,0,(int)cx,iVar2,0x40);
                WINNLSEnableIME(0,0);
                do {
                  iVar2 = ShowCursor(0);
                } while (-1 < iVar2);
                SetCursor((HCURSOR)0x0);
                DAT_005b67c1 = 0;
              }
              else {
                iVar5 = FUN_0041cc70(0x5b6758);
                iVar6 = DAT_005b87fc;
                iVar2 = DAT_005b87f8;
                if (iVar5 == 3) {
                  SetWindowLongW(DAT_005b6758,-0x10,0x10cb0000);
                  iVar2 = GetSystemMetrics(7);
                  iVar2 = (int)DAT_005b8800 + iVar2 * 2;
                  iVar6 = GetSystemMetrics(8);
                  iVar6 = DAT_005b8804 + iVar6 * 2;
                  iVar5 = GetSystemMetrics(4);
                  SetWindowPos(DAT_005b6758,(HWND)0x0,DAT_005c4d4c,DAT_005c4d50,iVar2,iVar6 + iVar5,
                               0x60);
                  ShowWindow(DAT_005b6758,1);
                  WINNLSEnableIME(0,1);
                  do {
                    iVar2 = ShowCursor(1);
                  } while (iVar2 < 0);
                }
                else {
                  SetWindowLongW(DAT_005b6758,-0x10,-0x70000000);
                  SetWindowPos(DAT_005b6758,(HWND)0x0,0,0,iVar2,iVar6,0x20);
                  WINNLSEnableIME(0,0);
                  do {
                    iVar2 = ShowCursor(0);
                  } while (-1 < iVar2);
                  SetCursor((HCURSOR)0x0);
                  DAT_005b67c1 = 0;
                }
              }
            }
            GetWindowRect(DAT_005b6758,(LPRECT)&DAT_005c4d4c);
            FUN_004dbd70(0x5c4d40);
            FUN_0041de00(&DAT_005b6758,0);
          }
        }
        else {
          TranslateMessage(&local_1028);
          DispatchMessageW(&local_1028);
        }
        goto LAB_0041ea9c;
      }
    }
    else if (iVar2 == -1) {
      local_102c = -1;
    }
    else {
      local_102c = 2;
    }
LAB_0041eef5:
    DAT_005c4f80 = FUN_0041cc70(0x5b6758);
    if (2 < DAT_005c4f80) {
      GetWindowRect(DAT_005b6758,(LPRECT)&DAT_005c4d4c);
      DAT_005c4f90 = DAT_005c4d4c;
      DAT_005c4f94 = DAT_005c4d50;
    }
    FUN_004dd490();
    FUN_004186c0(DAT_005b66d8);
    DAT_005b66d8 = (undefined4 *)0x0;
  } while( true );
}

