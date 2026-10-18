/* Entry: 0x0055049b; symbol: FUN_0055049b; body bytes: 1571 */

uint __cdecl
FUN_0055049b(int *param_1,wchar_t *param_2,undefined4 *param_3,uint param_4,byte param_5)

{
  wchar_t *pwVar1;
  bool bVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  wchar_t wVar6;
  byte bVar7;
  ushort uVar8;
  wchar_t wVar9;
  uint uVar10;
  wint_t _C;
  uint local_8;
  
  uVar3 = FUN_0054a3bb((int *)&param_2);
  pwVar1 = param_2;
  if ((char)uVar3 == '\0') {
LAB_005504df:
    if (param_3 != (undefined4 *)0x0) {
      *param_3 = param_2;
    }
    return 0;
  }
  if ((param_4 != 0) && (((int)param_4 < 2 || (0x24 < (int)param_4)))) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
    goto LAB_005504df;
  }
  local_8 = 0;
  uVar10 = (uint)(ushort)*param_2;
  param_2 = param_2 + 1;
  if ((char)param_1[5] == '\0') {
    FUN_0054a360(param_1);
  }
  while( true ) {
    _C = (wint_t)uVar10;
    iVar4 = _iswctype(_C,8);
    if (iVar4 == 0) break;
    uVar10 = (uint)(ushort)*param_2;
    param_2 = param_2 + 1;
  }
  bVar7 = param_5 | 2;
  if ((_C == 0x2d) || (bVar7 = param_5, _C == 0x2b)) {
    uVar10 = (uint)(ushort)*param_2;
    param_2 = param_2 + 1;
  }
  if ((param_4 != 0) && (uVar5 = param_4, param_4 != 0x10)) goto LAB_0055083b;
  uVar8 = (ushort)uVar10;
  if (uVar8 < 0x30) goto LAB_005507c7;
  if (uVar8 < 0x3a) {
    iVar4 = uVar10 - 0x30;
LAB_005507c2:
    if (iVar4 == -1) goto LAB_005507c7;
LAB_005507ef:
    if (iVar4 != 0) goto LAB_0055082e;
    wVar9 = *param_2;
    if ((wVar9 == L'x') || (wVar9 == L'X')) {
      uVar10 = (uint)(ushort)param_2[1];
      param_2 = param_2 + 2;
      uVar5 = 0x10;
      if (param_4 != 0) {
        uVar5 = param_4;
      }
      goto LAB_0055083b;
    }
    param_2 = param_2 + 1;
    __crt_strtox::c_string_character_source<wchar_t>::unget
              ((c_string_character_source<wchar_t> *)&param_2,wVar9);
    uVar5 = 8;
  }
  else {
    if (uVar8 < 0xff10) {
      if (0x65f < uVar8) {
        if (uVar8 < 0x66a) {
          iVar4 = uVar10 - 0x660;
        }
        else {
          if (uVar8 < 0x6f0) goto LAB_005507c7;
          if (uVar8 < 0x6fa) {
            iVar4 = uVar10 - 0x6f0;
          }
          else {
            if (uVar8 < 0x966) goto LAB_005507c7;
            if (uVar8 < 0x970) {
              iVar4 = uVar10 - 0x966;
            }
            else {
              if (uVar8 < 0x9e6) goto LAB_005507c7;
              if (uVar8 < 0x9f0) {
                iVar4 = uVar10 - 0x9e6;
              }
              else {
                if (uVar8 < 0xa66) goto LAB_005507c7;
                if (uVar8 < 0xa70) {
                  iVar4 = uVar10 - 0xa66;
                }
                else {
                  if (uVar8 < 0xae6) goto LAB_005507c7;
                  if (uVar8 < 0xaf0) {
                    iVar4 = uVar10 - 0xae6;
                  }
                  else {
                    if (uVar8 < 0xb66) goto LAB_005507c7;
                    if (uVar8 < 0xb70) {
                      iVar4 = uVar10 - 0xb66;
                    }
                    else {
                      if (uVar8 < 0xc66) goto LAB_005507c7;
                      if (uVar8 < 0xc70) {
                        iVar4 = uVar10 - 0xc66;
                      }
                      else {
                        if (uVar8 < 0xce6) goto LAB_005507c7;
                        if (uVar8 < 0xcf0) {
                          iVar4 = uVar10 - 0xce6;
                        }
                        else {
                          if (uVar8 < 0xd66) goto LAB_005507c7;
                          if (uVar8 < 0xd70) {
                            iVar4 = uVar10 - 0xd66;
                          }
                          else {
                            if (uVar8 < 0xe50) goto LAB_005507c7;
                            if (uVar8 < 0xe5a) {
                              iVar4 = uVar10 - 0xe50;
                            }
                            else {
                              if (uVar8 < 0xed0) goto LAB_005507c7;
                              if (uVar8 < 0xeda) {
                                iVar4 = uVar10 - 0xed0;
                              }
                              else {
                                if (uVar8 < 0xf20) goto LAB_005507c7;
                                if (uVar8 < 0xf2a) {
                                  iVar4 = uVar10 - 0xf20;
                                }
                                else {
                                  if (uVar8 < 0x1040) goto LAB_005507c7;
                                  if (uVar8 < 0x104a) {
                                    iVar4 = uVar10 - 0x1040;
                                  }
                                  else {
                                    if (uVar8 < 0x17e0) goto LAB_005507c7;
                                    if (uVar8 < 0x17ea) {
                                      iVar4 = uVar10 - 0x17e0;
                                    }
                                    else {
                                      if ((uVar8 < 0x1810) || (0x1819 < uVar8)) goto LAB_005507c7;
                                      iVar4 = uVar10 - 0x1810;
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
        goto LAB_005507c2;
      }
    }
    else if (uVar8 < 0xff1a) {
      iVar4 = uVar10 - 0xff10;
      goto LAB_005507c2;
    }
LAB_005507c7:
    if (((0x40 < uVar10) && (uVar10 < 0x5b)) || (uVar10 - 0x61 < 0x1a)) {
      uVar5 = uVar10;
      if (uVar10 - 0x61 < 0x1a) {
        uVar5 = uVar10 - 0x20;
      }
      iVar4 = uVar5 - 0x37;
      goto LAB_005507ef;
    }
LAB_0055082e:
    uVar5 = 10;
  }
  if (param_4 != 0) {
    uVar5 = param_4;
  }
LAB_0055083b:
  param_4 = uVar5;
  do {
    iVar4 = 0x30;
    wVar9 = (wchar_t)uVar10;
    if ((ushort)wVar9 < 0x30) goto LAB_005509cd;
    if ((ushort)wVar9 < 0x3a) {
LAB_005509c3:
      uVar5 = uVar10 - iVar4;
      if (uVar5 == 0xffffffff) goto LAB_005509cd;
    }
    else {
      iVar4 = 0xff10;
      if ((ushort)wVar9 < 0xff10) {
        iVar4 = 0x660;
        if (0x65f < (ushort)wVar9) {
          if ((ushort)wVar9 < 0x66a) goto LAB_005509c3;
          iVar4 = 0x6f0;
          if (0x6ef < (ushort)wVar9) {
            if ((ushort)wVar9 < 0x6fa) goto LAB_005509c3;
            iVar4 = 0x966;
            if (0x965 < (ushort)wVar9) {
              if ((ushort)wVar9 < 0x970) goto LAB_005509c3;
              iVar4 = 0x9e6;
              if (0x9e5 < (ushort)wVar9) {
                if ((ushort)wVar9 < 0x9f0) goto LAB_005509c3;
                iVar4 = 0xa66;
                if (0xa65 < (ushort)wVar9) {
                  if ((ushort)wVar9 < 0xa70) goto LAB_005509c3;
                  iVar4 = 0xae6;
                  if (0xae5 < (ushort)wVar9) {
                    if ((ushort)wVar9 < 0xaf0) goto LAB_005509c3;
                    iVar4 = 0xb66;
                    if (0xb65 < (ushort)wVar9) {
                      if ((ushort)wVar9 < 0xb70) goto LAB_005509c3;
                      iVar4 = 0xc66;
                      if (0xc65 < (ushort)wVar9) {
                        if ((ushort)wVar9 < 0xc70) goto LAB_005509c3;
                        iVar4 = 0xce6;
                        if (0xce5 < (ushort)wVar9) {
                          if ((ushort)wVar9 < 0xcf0) goto LAB_005509c3;
                          iVar4 = 0xd66;
                          if (0xd65 < (ushort)wVar9) {
                            if ((ushort)wVar9 < 0xd70) goto LAB_005509c3;
                            iVar4 = 0xe50;
                            if (0xe4f < (ushort)wVar9) {
                              if ((ushort)wVar9 < 0xe5a) goto LAB_005509c3;
                              iVar4 = 0xed0;
                              if (0xecf < (ushort)wVar9) {
                                if ((ushort)wVar9 < 0xeda) goto LAB_005509c3;
                                iVar4 = 0xf20;
                                if (0xf1f < (ushort)wVar9) {
                                  if ((ushort)wVar9 < 0xf2a) goto LAB_005509c3;
                                  iVar4 = 0x1040;
                                  if (0x103f < (ushort)wVar9) {
                                    if ((ushort)wVar9 < 0x104a) goto LAB_005509c3;
                                    iVar4 = 0x17e0;
                                    if (0x17df < (ushort)wVar9) {
                                      if ((ushort)wVar9 < 0x17ea) goto LAB_005509c3;
                                      iVar4 = 0x1810;
                                      if (0x180f < (ushort)wVar9) {
                                        wVar6 = L'᠚';
                                        goto LAB_005509be;
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
        wVar6 = L'：';
LAB_005509be:
        if ((ushort)wVar9 < (ushort)wVar6) goto LAB_005509c3;
      }
LAB_005509cd:
      if (((uVar10 < 0x41) || (0x5a < uVar10)) && ((uVar10 < 0x61 || (0x7a < (ushort)wVar9)))) {
        uVar5 = 0xffffffff;
      }
      else {
        if ((ushort)(wVar9 + L'ﾟ') < 0x1a) {
          uVar10 = uVar10 - 0x20;
        }
        uVar5 = uVar10 - 0x37;
      }
    }
    if (param_4 <= uVar5) {
      __crt_strtox::c_string_character_source<wchar_t>::unget
                ((c_string_character_source<wchar_t> *)&param_2,wVar9);
      if ((bVar7 & 8) == 0) {
        if (param_3 == (undefined4 *)0x0) {
          return 0;
        }
        *param_3 = pwVar1;
        return 0;
      }
      bVar2 = FUN_005489e6(bVar7,local_8);
      if (bVar2) {
        *(undefined1 *)(param_1 + 7) = 1;
        param_1[6] = 0x22;
        if ((bVar7 & 1) != 0) {
          if ((bVar7 & 2) == 0) {
            if (param_3 != (undefined4 *)0x0) {
              *param_3 = param_2;
            }
            return 0x7fffffff;
          }
          if (param_3 != (undefined4 *)0x0) {
            *param_3 = param_2;
          }
          return 0x80000000;
        }
        local_8 = 0xffffffff;
      }
      else if ((bVar7 & 2) != 0) {
        local_8 = -local_8;
      }
      if (param_3 != (undefined4 *)0x0) {
        *param_3 = param_2;
        return local_8;
      }
      return local_8;
    }
    uVar5 = local_8 * param_4 + uVar5;
    bVar7 = bVar7 | (uVar5 < local_8 * param_4 || (uint)(0xffffffff / (ulonglong)param_4) < local_8)
                    << 2 | 8U;
    uVar10 = (uint)(ushort)*param_2;
    param_2 = param_2 + 1;
    local_8 = uVar5;
  } while( true );
}

