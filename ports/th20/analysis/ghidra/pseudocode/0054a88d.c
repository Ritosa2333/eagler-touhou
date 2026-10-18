/* Entry: 0x0054a88d; symbol: FUN_0054a88d; body bytes: 2824 */

uint __cdecl
FUN_0054a88d(int *param_1,
            input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
            *param_2,int *param_3)

{
  undefined4 uVar1;
  bool bVar2;
  char cVar3;
  byte bVar4;
  wint_t _C;
  wchar_t wVar5;
  ushort uVar6;
  undefined4 uVar7;
  int iVar8;
  uint uVar9;
  int iVar10;
  ushort uVar11;
  wchar_t wVar12;
  uint uVar13;
  int *piVar14;
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_> *local_40;
  undefined4 *local_3c;
  undefined4 *local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  uint local_28;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  int *local_18;
  int *local_14;
  int local_10;
  undefined4 local_c;
  byte local_5;
  
  uVar7 = FUN_0054fadb((int *)param_2);
  if ((char)uVar7 == '\0') {
    return 7;
  }
  local_30 = *(undefined4 *)(param_2 + 0x14);
  local_34 = *(undefined4 *)(param_2 + 0x10);
  _C = FUN_0054eec7((undefined4 *)param_2);
  local_3c = &local_c;
  local_40 = param_2;
  local_38 = &local_34;
  while( true ) {
    local_c = CONCAT22(local_c._2_2_,_C);
    iVar8 = _iswctype(_C,8);
    if (iVar8 == 0) break;
    _C = FUN_0054eec7((undefined4 *)param_2);
  }
  piVar14 = param_3 + 0xc2;
  local_2c = 0x2d;
  *(bool *)piVar14 = (wchar_t)local_c == L'-';
  local_18 = piVar14;
  if (((wchar_t)local_c == L'-') || (wVar5 = (wchar_t)local_c, (wchar_t)local_c == L'+')) {
    wVar5 = FUN_0054eec7((undefined4 *)param_2);
    local_c = CONCAT22(local_c._2_2_,wVar5);
  }
  if ((wVar5 == L'I') || (wVar5 == L'i')) {
    uVar13 = FUN_0054b395((wchar_t *)&local_c,param_2,local_34,local_30);
    return uVar13;
  }
  if ((wVar5 == L'N') || (wVar5 == L'n')) {
    uVar13 = FUN_0054b464((wchar_t *)&local_c,param_2,local_34,local_30);
    return uVar13;
  }
  local_10 = 0;
  local_5 = 0;
  local_20 = 0x30;
  if (wVar5 == L'0') {
    uVar7 = *(undefined4 *)(param_2 + 0x10);
    uVar1 = *(undefined4 *)(param_2 + 0x14);
    wVar5 = FUN_0054eec7((undefined4 *)param_2);
    if ((wVar5 == L'x') || (wVar5 == L'X')) {
      local_5 = 1;
      wVar5 = FUN_0054eec7((undefined4 *)param_2);
      local_c = CONCAT22(local_c._2_2_,wVar5);
      piVar14 = local_18;
      local_34 = uVar7;
      local_30 = uVar1;
    }
    else {
      __crt_strtox::
      input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
                (param_2,wVar5);
      piVar14 = local_18;
      wVar5 = (wchar_t)local_c;
    }
  }
  local_14 = param_3 + 2;
  local_1c = 0;
  bVar4 = local_5;
  bVar2 = false;
  while (local_5 = bVar4, wVar5 == L'0') {
    wVar5 = FUN_0054eec7((undefined4 *)param_2);
    local_c = CONCAT22(local_c._2_2_,wVar5);
    piVar14 = local_18;
    bVar4 = local_5;
    bVar2 = true;
  }
  local_24 = 0x3a;
  local_28 = 9;
  uVar13 = 9;
  if (bVar4 != 0) {
    local_28 = 0xf;
    uVar13 = 0xf;
  }
  do {
    iVar8 = 0x30;
    if ((ushort)wVar5 < 0x30) goto LAB_0054ab97;
    if ((ushort)wVar5 < (ushort)(wchar_t)local_24) {
LAB_0054aa06:
      uVar9 = (uint)(ushort)wVar5 - iVar8;
LAB_0054ab92:
      if (uVar9 == 0xffffffff) goto LAB_0054ab97;
    }
    else {
      if ((ushort)wVar5 < 0xff10) {
        iVar8 = 0x660;
        if ((0x65f < (ushort)wVar5) &&
           (((ushort)wVar5 < 0x66a ||
            ((iVar8 = 0x6f0, 0x6ef < (ushort)wVar5 &&
             (((ushort)wVar5 < 0x6fa ||
              ((iVar8 = 0x966, 0x965 < (ushort)wVar5 &&
               (((ushort)wVar5 < 0x970 ||
                ((iVar8 = 0x9e6, 0x9e5 < (ushort)wVar5 &&
                 (((ushort)wVar5 < 0x9f0 ||
                  ((iVar8 = 0xa66, 0xa65 < (ushort)wVar5 &&
                   (((ushort)wVar5 < 0xa70 ||
                    ((iVar8 = 0xae6, 0xae5 < (ushort)wVar5 &&
                     (((ushort)wVar5 < 0xaf0 ||
                      ((iVar8 = 0xb66, 0xb65 < (ushort)wVar5 &&
                       (((ushort)wVar5 < 0xb70 ||
                        ((iVar8 = 0xc66, 0xc65 < (ushort)wVar5 &&
                         (((ushort)wVar5 < 0xc70 ||
                          ((iVar8 = 0xce6, 0xce5 < (ushort)wVar5 &&
                           (((ushort)wVar5 < 0xcf0 ||
                            ((iVar8 = 0xd66, 0xd65 < (ushort)wVar5 &&
                             (((ushort)wVar5 < 0xd70 ||
                              ((iVar8 = 0xe50, 0xe4f < (ushort)wVar5 &&
                               (((ushort)wVar5 < 0xe5a ||
                                ((iVar8 = 0xed0, 0xecf < (ushort)wVar5 &&
                                 (((ushort)wVar5 < 0xeda ||
                                  ((iVar8 = 0xf20, 0xf1f < (ushort)wVar5 &&
                                   (((ushort)wVar5 < 0xf2a ||
                                    ((iVar8 = 0x1040, 0x103f < (ushort)wVar5 &&
                                     (((ushort)wVar5 < 0x104a ||
                                      ((iVar8 = 0x17e0, 0x17df < (ushort)wVar5 &&
                                       (((ushort)wVar5 < 0x17ea ||
                                        ((iVar8 = 0x1810, 0x180f < (ushort)wVar5 &&
                                         ((ushort)wVar5 < 0x181a))))))))))))))))))))))))))))))))))))
                       )))))))))))))))))))))))))) goto LAB_0054aa06;
      }
      else if ((ushort)wVar5 < 0xff1a) {
        uVar9 = (ushort)wVar5 - 0xff10;
        goto LAB_0054ab92;
      }
LAB_0054ab97:
      uVar9 = (uint)(ushort)wVar5;
      if (((uVar9 < 0x41) || (0x5a < uVar9)) && (0x19 < uVar9 - 0x61)) {
        uVar9 = 0xffffffff;
      }
      else {
        if (uVar9 - 0x61 < 0x1a) {
          uVar9 = uVar9 - 0x20;
        }
        uVar9 = uVar9 - 0x37;
      }
    }
    if (uVar13 < uVar9) goto LAB_0054abeb;
    bVar2 = true;
    if (local_14 != piVar14) {
      *(char *)local_14 = (char)uVar9;
      local_14 = (int *)((int)local_14 + 1);
    }
    local_1c = local_1c + 1;
    wVar5 = FUN_0054eec7((undefined4 *)param_2);
    local_c = CONCAT22(local_c._2_2_,wVar5);
  } while( true );
LAB_0054b0ef:
  local_10 = 0x1451;
LAB_0054b0f9:
  do {
    iVar8 = 0x30;
    if ((ushort)(wchar_t)local_c < 0x30) goto LAB_0054b282;
    if ((ushort)(wchar_t)local_c < 0x3a) {
LAB_0054b278:
      uVar13 = (uint)(ushort)(wchar_t)local_c - iVar8;
      if (uVar13 == 0xffffffff) goto LAB_0054b282;
    }
    else {
      iVar8 = 0xff10;
      if ((ushort)(wchar_t)local_c < 0xff10) {
        iVar8 = 0x660;
        if (0x65f < (ushort)(wchar_t)local_c) {
          if ((ushort)(wchar_t)local_c < 0x66a) goto LAB_0054b278;
          iVar8 = 0x6f0;
          if (0x6ef < (ushort)(wchar_t)local_c) {
            if ((ushort)(wchar_t)local_c < 0x6fa) goto LAB_0054b278;
            iVar8 = 0x966;
            if (0x965 < (ushort)(wchar_t)local_c) {
              if ((ushort)(wchar_t)local_c < 0x970) goto LAB_0054b278;
              iVar8 = 0x9e6;
              if (0x9e5 < (ushort)(wchar_t)local_c) {
                if ((ushort)(wchar_t)local_c < 0x9f0) goto LAB_0054b278;
                iVar8 = 0xa66;
                if (0xa65 < (ushort)(wchar_t)local_c) {
                  if ((ushort)(wchar_t)local_c < 0xa70) goto LAB_0054b278;
                  iVar8 = 0xae6;
                  if (0xae5 < (ushort)(wchar_t)local_c) {
                    if ((ushort)(wchar_t)local_c < 0xaf0) goto LAB_0054b278;
                    iVar8 = 0xb66;
                    if (0xb65 < (ushort)(wchar_t)local_c) {
                      if ((ushort)(wchar_t)local_c < 0xb70) goto LAB_0054b278;
                      iVar8 = 0xc66;
                      if (0xc65 < (ushort)(wchar_t)local_c) {
                        if ((ushort)(wchar_t)local_c < 0xc70) goto LAB_0054b278;
                        iVar8 = 0xce6;
                        if (0xce5 < (ushort)(wchar_t)local_c) {
                          if ((ushort)(wchar_t)local_c < 0xcf0) goto LAB_0054b278;
                          iVar8 = 0xd66;
                          if (0xd65 < (ushort)(wchar_t)local_c) {
                            if ((ushort)(wchar_t)local_c < 0xd70) goto LAB_0054b278;
                            iVar8 = 0xe50;
                            if (0xe4f < (ushort)(wchar_t)local_c) {
                              if ((ushort)(wchar_t)local_c < 0xe5a) goto LAB_0054b278;
                              iVar8 = 0xed0;
                              if (0xecf < (ushort)(wchar_t)local_c) {
                                if ((ushort)(wchar_t)local_c < 0xeda) goto LAB_0054b278;
                                iVar8 = 0xf20;
                                if (0xf1f < (ushort)(wchar_t)local_c) {
                                  if ((ushort)(wchar_t)local_c < 0xf2a) goto LAB_0054b278;
                                  iVar8 = 0x1040;
                                  if (0x103f < (ushort)(wchar_t)local_c) {
                                    if ((ushort)(wchar_t)local_c < 0x104a) goto LAB_0054b278;
                                    iVar8 = 0x17e0;
                                    if (0x17df < (ushort)(wchar_t)local_c) {
                                      if ((ushort)(wchar_t)local_c < 0x17ea) goto LAB_0054b278;
                                      iVar8 = 0x1810;
                                      if (0x180f < (ushort)(wchar_t)local_c) {
                                        wVar12 = L'᠚';
                                        goto LAB_0054b273;
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      else {
        wVar12 = L'：';
LAB_0054b273:
        if ((ushort)(wchar_t)local_c < (ushort)wVar12) goto LAB_0054b278;
      }
LAB_0054b282:
      uVar13 = (uint)(ushort)(wchar_t)local_c;
      if (((uVar13 < 0x41) || (0x5a < uVar13)) && (0x19 < uVar13 - 0x61)) break;
      if (uVar13 - 0x61 < 0x1a) {
        uVar13 = uVar13 - 0x20;
      }
      uVar13 = uVar13 - 0x37;
    }
    if (9 < uVar13) break;
    local_c._0_2_ = FUN_0054eec7((undefined4 *)param_2);
  } while( true );
  if (wVar5 == L'-') {
    local_10 = -local_10;
  }
  iVar8 = local_10;
  if (cVar3 == '\0') {
    cVar3 = FID_conflict_operator__(&local_40);
    if (cVar3 == '\0') {
      return 7;
    }
    local_c._0_2_ = FUN_0054eec7((undefined4 *)param_2);
    iVar8 = local_10;
  }
  goto LAB_0054b2e9;
LAB_0054abeb:
  if ((uint)(ushort)wVar5 == (int)*(char *)**(undefined4 **)(*param_1 + 0x88)) {
    uVar6 = FUN_0054eec7((undefined4 *)param_2);
    local_c = CONCAT22(local_c._2_2_,uVar6);
    iVar8 = local_1c;
    piVar14 = local_18;
    if ((local_14 == param_3 + 2) && (uVar6 == 0x30)) {
      bVar2 = true;
      do {
        iVar8 = iVar8 + -1;
        uVar6 = FUN_0054eec7((undefined4 *)param_2);
        local_c = CONCAT22(local_c._2_2_,uVar6);
        piVar14 = local_18;
        uVar13 = local_28;
      } while (uVar6 == 0x30);
    }
    do {
      local_1c = iVar8;
      if (uVar6 < (ushort)(wchar_t)local_20) goto LAB_0054ade7;
      if (uVar6 < 0x3a) {
        iVar8 = 0x30;
LAB_0054addd:
        uVar9 = (uint)uVar6 - iVar8;
        if (uVar9 == 0xffffffff) goto LAB_0054ade7;
      }
      else {
        iVar8 = 0xff10;
        if (uVar6 < 0xff10) {
          iVar8 = 0x660;
          if (0x65f < uVar6) {
            if (uVar6 < 0x66a) goto LAB_0054addd;
            iVar8 = 0x6f0;
            if (0x6ef < uVar6) {
              if (uVar6 < 0x6fa) goto LAB_0054addd;
              iVar8 = 0x966;
              if (0x965 < uVar6) {
                if (uVar6 < 0x970) goto LAB_0054addd;
                iVar8 = 0x9e6;
                if (0x9e5 < uVar6) {
                  if (uVar6 < 0x9f0) goto LAB_0054addd;
                  iVar8 = 0xa66;
                  if (0xa65 < uVar6) {
                    if (uVar6 < 0xa70) goto LAB_0054addd;
                    iVar8 = 0xae6;
                    if (0xae5 < uVar6) {
                      if (uVar6 < 0xaf0) goto LAB_0054addd;
                      iVar8 = 0xb66;
                      if (0xb65 < uVar6) {
                        if (uVar6 < 0xb70) goto LAB_0054addd;
                        iVar8 = 0xc66;
                        if (0xc65 < uVar6) {
                          if (uVar6 < 0xc70) goto LAB_0054addd;
                          iVar8 = 0xce6;
                          if (0xce5 < uVar6) {
                            if (uVar6 < 0xcf0) goto LAB_0054addd;
                            iVar8 = 0xd66;
                            if (0xd65 < uVar6) {
                              if (uVar6 < 0xd70) goto LAB_0054addd;
                              iVar8 = 0xe50;
                              if (0xe4f < uVar6) {
                                if (uVar6 < 0xe5a) goto LAB_0054addd;
                                iVar8 = 0xed0;
                                if (0xecf < uVar6) {
                                  if (uVar6 < 0xeda) goto LAB_0054addd;
                                  iVar8 = 0xf20;
                                  if (0xf1f < uVar6) {
                                    if (uVar6 < 0xf2a) goto LAB_0054addd;
                                    iVar8 = 0x1040;
                                    if (0x103f < uVar6) {
                                      if (uVar6 < 0x104a) goto LAB_0054addd;
                                      iVar8 = 0x17e0;
                                      if (0x17df < uVar6) {
                                        if (uVar6 < 0x17ea) goto LAB_0054addd;
                                        iVar8 = 0x1810;
                                        if (0x180f < uVar6) {
                                          uVar11 = 0x181a;
                                          goto LAB_0054add8;
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else {
          uVar11 = 0xff1a;
LAB_0054add8:
          if (uVar6 < uVar11) goto LAB_0054addd;
        }
LAB_0054ade7:
        uVar9 = (uint)uVar6;
        if (((uVar9 < 0x41) || (0x5a < uVar9)) && (0x19 < uVar9 - 0x61)) {
          uVar9 = 0xffffffff;
        }
        else {
          if (uVar9 - 0x61 < 0x1a) {
            uVar9 = uVar9 - 0x20;
          }
          uVar9 = uVar9 - 0x37;
        }
      }
      if (uVar13 < uVar9) break;
      bVar2 = true;
      if (local_14 != piVar14) {
        *(char *)local_14 = (char)uVar9;
        local_14 = (int *)((int)local_14 + 1);
      }
      uVar6 = FUN_0054eec7((undefined4 *)param_2);
      local_c = CONCAT22(local_c._2_2_,uVar6);
      iVar8 = local_1c;
    } while( true );
  }
  if (!bVar2) {
    cVar3 = FID_conflict_operator__(&local_40);
    if (cVar3 == '\0') {
      return 7;
    }
    if (bVar4 == 0) {
      return 7;
    }
    return 2;
  }
  __crt_strtox::
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
            (param_2,(wchar_t)local_c);
  local_30 = *(undefined4 *)(param_2 + 0x14);
  local_34 = *(undefined4 *)(param_2 + 0x10);
  wVar5 = FUN_0054eec7((undefined4 *)param_2);
  iVar8 = 0;
  if (wVar5 == L'E') {
LAB_0054ae9f:
    bVar4 = local_5 ^ 1;
  }
  else if (wVar5 == L'P') {
LAB_0054ae9a:
    bVar4 = local_5;
  }
  else {
    if (wVar5 == L'e') goto LAB_0054ae9f;
    bVar4 = 0;
    if (wVar5 == L'p') goto LAB_0054ae9a;
  }
  local_c._0_2_ = wVar5;
  if (bVar4 != 0) {
    wVar5 = FUN_0054eec7((undefined4 *)param_2);
    local_c._0_2_ = wVar5;
    if ((wVar5 == L'+') || (wVar5 == (wchar_t)local_2c)) {
      local_c._0_2_ = FUN_0054eec7((undefined4 *)param_2);
    }
    if ((wchar_t)local_c == (wchar_t)local_20) {
      iVar8 = 1;
      do {
        local_c._0_2_ = FUN_0054eec7((undefined4 *)param_2);
      } while ((wchar_t)local_c == L'0');
    }
    do {
      cVar3 = (char)iVar8;
      iVar8 = 0x30;
      if ((ushort)(wchar_t)local_c < 0x30) goto LAB_0054b09b;
      if ((ushort)(wchar_t)local_c < 0x3a) {
LAB_0054b091:
        uVar13 = (uint)(ushort)(wchar_t)local_c - iVar8;
        if (uVar13 == 0xffffffff) goto LAB_0054b09b;
      }
      else {
        iVar8 = 0xff10;
        if ((ushort)(wchar_t)local_c < 0xff10) {
          iVar8 = 0x660;
          if (0x65f < (ushort)(wchar_t)local_c) {
            if ((ushort)(wchar_t)local_c < 0x66a) goto LAB_0054b091;
            iVar8 = 0x6f0;
            if (0x6ef < (ushort)(wchar_t)local_c) {
              if ((ushort)(wchar_t)local_c < 0x6fa) goto LAB_0054b091;
              iVar8 = 0x966;
              if (0x965 < (ushort)(wchar_t)local_c) {
                if ((ushort)(wchar_t)local_c < 0x970) goto LAB_0054b091;
                iVar8 = 0x9e6;
                if (0x9e5 < (ushort)(wchar_t)local_c) {
                  if ((ushort)(wchar_t)local_c < 0x9f0) goto LAB_0054b091;
                  iVar8 = 0xa66;
                  if (0xa65 < (ushort)(wchar_t)local_c) {
                    if ((ushort)(wchar_t)local_c < 0xa70) goto LAB_0054b091;
                    iVar8 = 0xae6;
                    if (0xae5 < (ushort)(wchar_t)local_c) {
                      if ((ushort)(wchar_t)local_c < 0xaf0) goto LAB_0054b091;
                      iVar8 = 0xb66;
                      if (0xb65 < (ushort)(wchar_t)local_c) {
                        if ((ushort)(wchar_t)local_c < 0xb70) goto LAB_0054b091;
                        iVar8 = 0xc66;
                        if (0xc65 < (ushort)(wchar_t)local_c) {
                          if ((ushort)(wchar_t)local_c < 0xc70) goto LAB_0054b091;
                          iVar8 = 0xce6;
                          if (0xce5 < (ushort)(wchar_t)local_c) {
                            if ((ushort)(wchar_t)local_c < 0xcf0) goto LAB_0054b091;
                            iVar8 = 0xd66;
                            if (0xd65 < (ushort)(wchar_t)local_c) {
                              if ((ushort)(wchar_t)local_c < 0xd70) goto LAB_0054b091;
                              iVar8 = 0xe50;
                              if (0xe4f < (ushort)(wchar_t)local_c) {
                                if ((ushort)(wchar_t)local_c < 0xe5a) goto LAB_0054b091;
                                iVar8 = 0xed0;
                                if (0xecf < (ushort)(wchar_t)local_c) {
                                  if ((ushort)(wchar_t)local_c < 0xeda) goto LAB_0054b091;
                                  iVar8 = 0xf20;
                                  if (0xf1f < (ushort)(wchar_t)local_c) {
                                    if ((ushort)(wchar_t)local_c < 0xf2a) goto LAB_0054b091;
                                    iVar8 = 0x1040;
                                    if (0x103f < (ushort)(wchar_t)local_c) {
                                      if ((ushort)(wchar_t)local_c < 0x104a) goto LAB_0054b091;
                                      iVar8 = 0x17e0;
                                      if (0x17df < (ushort)(wchar_t)local_c) {
                                        if ((ushort)(wchar_t)local_c < 0x17ea) goto LAB_0054b091;
                                        iVar8 = 0x1810;
                                        if (0x180f < (ushort)(wchar_t)local_c) {
                                          wVar12 = L'᠚';
                                          goto LAB_0054b08c;
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else {
          wVar12 = L'：';
LAB_0054b08c:
          if ((ushort)(wchar_t)local_c < (ushort)wVar12) goto LAB_0054b091;
        }
LAB_0054b09b:
        uVar13 = (uint)(ushort)(wchar_t)local_c;
        if (((uVar13 < 0x41) || (0x5a < uVar13)) && (0x19 < uVar13 - 0x61)) goto LAB_0054b0f9;
        if (uVar13 - 0x61 < 0x1a) {
          uVar13 = uVar13 - 0x20;
        }
        uVar13 = uVar13 - 0x37;
      }
      if (9 < uVar13) goto LAB_0054b0f9;
      iVar8 = 1;
      cVar3 = '\x01';
      local_10 = local_10 * 10 + uVar13;
      if (0x1450 < local_10) goto LAB_0054b0ef;
      local_c._0_2_ = FUN_0054eec7((undefined4 *)param_2);
    } while( true );
  }
LAB_0054b2e9:
  __crt_strtox::
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
            (param_2,(wchar_t)local_c);
  do {
    piVar14 = local_14;
    if (piVar14 == param_3 + 2) {
      return 2;
    }
    local_14 = (int *)((int)piVar14 + -1);
  } while (*(char *)((int)piVar14 + -1) == '\0');
  if (0x1450 < iVar8) {
    return 9;
  }
  if (-0x1451 < iVar8) {
    iVar10 = 1;
    if (local_5 != 0) {
      iVar10 = 4;
    }
    iVar8 = iVar8 + iVar10 * local_1c;
    if (0x1450 < iVar8) {
      return 9;
    }
    if (-0x1451 < iVar8) {
      *param_3 = iVar8;
      param_3[1] = (int)piVar14 - (int)(param_3 + 2);
      return (uint)local_5;
    }
  }
  return 8;
}

