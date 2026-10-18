/* Entry: 0x00550abe; symbol: FUN_00550abe; body bytes: 1487 */

/* WARNING: Removing unreachable block (ram,0x00550fbd) */

undefined8 __cdecl
FUN_00550abe(int *param_1,wchar_t *param_2,int *param_3,uint param_4,byte param_5)

{
  byte bVar1;
  wchar_t *pwVar2;
  char cVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  byte bVar8;
  wchar_t wVar9;
  uint uVar10;
  ushort uVar11;
  wchar_t wVar12;
  uint uVar13;
  uint uVar14;
  bool bVar15;
  undefined8 uVar16;
  ulonglong uVar17;
  wint_t _C;
  ulonglong local_28;
  uint local_1c;
  uint local_18;
  uint local_8;
  
  uVar4 = FUN_0054a3bb((int *)&param_2);
  uVar10 = param_4;
  pwVar2 = param_2;
  if ((char)uVar4 != '\0') {
    if ((param_4 == 0) || ((1 < (int)param_4 && ((int)param_4 < 0x25)))) {
      local_28 = 0;
      uVar13 = (uint)(ushort)*param_2;
      param_2 = param_2 + 1;
      if ((char)param_1[5] == '\0') {
        FUN_0054a360(param_1);
      }
      while( true ) {
        _C = (wint_t)uVar13;
        iVar5 = _iswctype(_C,8);
        if (iVar5 == 0) break;
        uVar13 = (uint)(ushort)*param_2;
        param_2 = param_2 + 1;
      }
      bVar1 = param_5 | 2;
      if ((_C == 0x2d) || (bVar1 = param_5, _C == 0x2b)) {
        uVar13 = (uint)(ushort)*param_2;
        param_2 = param_2 + 1;
      }
      iVar5 = 0x30;
      if ((uVar10 != 0) && (uVar6 = uVar10, uVar10 != 0x10)) goto LAB_00550dae;
      uVar11 = (ushort)uVar13;
      if (uVar11 < 0x30) goto LAB_00550d46;
      if (uVar11 < 0x3a) {
LAB_00550bb5:
        iVar5 = -iVar5;
LAB_00550d41:
        iVar5 = uVar13 + iVar5;
        if (iVar5 == -1) goto LAB_00550d46;
LAB_00550d6b:
        if (iVar5 != 0) goto LAB_00550da1;
        wVar12 = *param_2;
        if ((wVar12 == L'x') || (wVar12 == L'X')) {
          uVar13 = (uint)(ushort)param_2[1];
          param_2 = param_2 + 2;
          uVar6 = 0x10;
          param_4 = 0x10;
          if (uVar10 != 0) {
            uVar6 = uVar10;
            param_4 = uVar10;
          }
          goto LAB_00550dae;
        }
        param_2 = param_2 + 1;
        __crt_strtox::c_string_character_source<wchar_t>::unget
                  ((c_string_character_source<wchar_t> *)&param_2,wVar12);
        uVar6 = 8;
      }
      else {
        if (uVar11 < 0xff10) {
          iVar5 = 0x660;
          if ((0x65f < uVar11) &&
             ((uVar11 < 0x66a ||
              ((iVar5 = 0x6f0, 0x6ef < uVar11 &&
               ((uVar11 < 0x6fa ||
                ((iVar5 = 0x966, 0x965 < uVar11 &&
                 ((uVar11 < 0x970 ||
                  ((iVar5 = 0x9e6, 0x9e5 < uVar11 &&
                   ((uVar11 < 0x9f0 ||
                    ((iVar5 = 0xa66, 0xa65 < uVar11 &&
                     ((uVar11 < 0xa70 ||
                      ((iVar5 = 0xae6, 0xae5 < uVar11 &&
                       ((uVar11 < 0xaf0 ||
                        ((iVar5 = 0xb66, 0xb65 < uVar11 &&
                         ((uVar11 < 0xb70 ||
                          ((iVar5 = 0xc66, 0xc65 < uVar11 &&
                           ((uVar11 < 0xc70 ||
                            ((iVar5 = 0xce6, 0xce5 < uVar11 &&
                             ((uVar11 < 0xcf0 ||
                              ((iVar5 = 0xd66, 0xd65 < uVar11 &&
                               ((uVar11 < 0xd70 ||
                                ((iVar5 = 0xe50, 0xe4f < uVar11 &&
                                 ((uVar11 < 0xe5a ||
                                  ((iVar5 = 0xed0, 0xecf < uVar11 &&
                                   ((uVar11 < 0xeda ||
                                    ((iVar5 = 0xf20, 0xf1f < uVar11 &&
                                     ((uVar11 < 0xf2a ||
                                      ((iVar5 = 0x1040, 0x103f < uVar11 &&
                                       ((uVar11 < 0x104a ||
                                        ((iVar5 = 0x17e0, 0x17df < uVar11 &&
                                         ((uVar11 < 0x17ea ||
                                          ((iVar5 = 0x1810, 0x180f < uVar11 && (uVar11 < 0x181a)))))
                                         )))))))))))))))))))))))))))))))))))))))))))))))))))))))))
          goto LAB_00550bb5;
        }
        else if (uVar11 < 0xff1a) {
          iVar5 = -0xff10;
          goto LAB_00550d41;
        }
LAB_00550d46:
        if (((0x40 < uVar13) && (uVar13 < 0x5b)) || (uVar13 - 0x61 < 0x1a)) {
          uVar6 = uVar13;
          if (uVar13 - 0x61 < 0x1a) {
            uVar6 = uVar13 - 0x20;
          }
          iVar5 = uVar6 - 0x37;
          goto LAB_00550d6b;
        }
LAB_00550da1:
        uVar6 = 10;
      }
      param_4 = uVar6;
      if (uVar10 != 0) {
        uVar6 = uVar10;
        param_4 = uVar10;
      }
LAB_00550dae:
      uVar16 = __aulldiv(0xffffffff,0xffffffff,uVar6,(int)uVar6 >> 0x1f);
      local_8 = 0;
      do {
        uVar10 = (uint)(local_28 >> 0x20);
        iVar5 = 0x30;
        wVar12 = (wchar_t)uVar13;
        if ((ushort)wVar12 < 0x30) goto LAB_00550f5c;
        if ((ushort)wVar12 < 0x3a) {
LAB_00550f52:
          uVar14 = uVar13 - iVar5;
          uVar7 = (uint)local_28;
          if (uVar14 == 0xffffffff) goto LAB_00550f5c;
        }
        else {
          iVar5 = 0xff10;
          if ((ushort)wVar12 < 0xff10) {
            iVar5 = 0x660;
            if (0x65f < (ushort)wVar12) {
              if ((ushort)wVar12 < 0x66a) goto LAB_00550f52;
              iVar5 = 0x6f0;
              if (0x6ef < (ushort)wVar12) {
                if ((ushort)wVar12 < 0x6fa) goto LAB_00550f52;
                iVar5 = 0x966;
                if (0x965 < (ushort)wVar12) {
                  if ((ushort)wVar12 < 0x970) goto LAB_00550f52;
                  iVar5 = 0x9e6;
                  if (0x9e5 < (ushort)wVar12) {
                    if ((ushort)wVar12 < 0x9f0) goto LAB_00550f52;
                    iVar5 = 0xa66;
                    if (0xa65 < (ushort)wVar12) {
                      if ((ushort)wVar12 < 0xa70) goto LAB_00550f52;
                      iVar5 = 0xae6;
                      if (0xae5 < (ushort)wVar12) {
                        if ((ushort)wVar12 < 0xaf0) goto LAB_00550f52;
                        iVar5 = 0xb66;
                        if (0xb65 < (ushort)wVar12) {
                          if ((ushort)wVar12 < 0xb70) goto LAB_00550f52;
                          iVar5 = 0xc66;
                          if (0xc65 < (ushort)wVar12) {
                            if ((ushort)wVar12 < 0xc70) goto LAB_00550f52;
                            iVar5 = 0xce6;
                            if (0xce5 < (ushort)wVar12) {
                              if ((ushort)wVar12 < 0xcf0) goto LAB_00550f52;
                              iVar5 = 0xd66;
                              if (0xd65 < (ushort)wVar12) {
                                if ((ushort)wVar12 < 0xd70) goto LAB_00550f52;
                                iVar5 = 0xe50;
                                if (0xe4f < (ushort)wVar12) {
                                  if ((ushort)wVar12 < 0xe5a) goto LAB_00550f52;
                                  iVar5 = 0xed0;
                                  if (0xecf < (ushort)wVar12) {
                                    if ((ushort)wVar12 < 0xeda) goto LAB_00550f52;
                                    iVar5 = 0xf20;
                                    if (0xf1f < (ushort)wVar12) {
                                      if ((ushort)wVar12 < 0xf2a) goto LAB_00550f52;
                                      iVar5 = 0x1040;
                                      if (0x103f < (ushort)wVar12) {
                                        if ((ushort)wVar12 < 0x104a) goto LAB_00550f52;
                                        iVar5 = 0x17e0;
                                        if (0x17df < (ushort)wVar12) {
                                          if ((ushort)wVar12 < 0x17ea) goto LAB_00550f52;
                                          iVar5 = 0x1810;
                                          if (0x180f < (ushort)wVar12) {
                                            wVar9 = L'᠚';
                                            goto LAB_00550f4d;
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
            wVar9 = L'：';
LAB_00550f4d:
            if ((ushort)wVar12 < (ushort)wVar9) goto LAB_00550f52;
          }
LAB_00550f5c:
          uVar7 = local_8;
          if (((uVar13 < 0x41) || (0x5a < uVar13)) && (0x19 < uVar13 - 0x61)) {
            uVar14 = 0xffffffff;
          }
          else {
            if (uVar13 - 0x61 < 0x1a) {
              uVar13 = uVar13 - 0x20;
            }
            uVar14 = uVar13 - 0x37;
          }
        }
        if (param_4 <= uVar14) goto LAB_00550feb;
        uVar17 = __allmul(uVar6,(int)uVar6 >> 0x1f,uVar7,uVar10);
        local_28 = uVar17 + uVar14;
        local_18 = (uint)((ulonglong)uVar16 >> 0x20);
        if ((uVar10 < local_18) ||
           ((uVar10 <= local_18 && (local_1c = (uint)uVar16, local_8 <= local_1c)))) {
          bVar8 = 0;
        }
        else {
          bVar8 = 1;
        }
        bVar1 = bVar1 | (local_28 < uVar17 | bVar8) << 2 | 8;
        uVar13 = (uint)(ushort)*param_2;
        param_2 = param_2 + 1;
        local_8 = (uint)local_28;
      } while( true );
    }
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
  }
  if (param_3 != (int *)0x0) {
    *param_3 = (int)param_2;
  }
  goto LAB_00551007;
LAB_00550feb:
  __crt_strtox::c_string_character_source<wchar_t>::unget
            ((c_string_character_source<wchar_t> *)&param_2,wVar12);
  if ((bVar1 & 8) != 0) {
    cVar3 = FUN_0054a71a(bVar1,local_8,uVar10);
    if (cVar3 == '\0') {
      if ((bVar1 & 2) != 0) {
        bVar15 = local_8 != 0;
        local_8 = -local_8;
        uVar10 = -(uVar10 + bVar15);
      }
    }
    else {
      *(undefined1 *)(param_1 + 7) = 1;
      param_1[6] = 0x22;
      if ((bVar1 & 1) != 0) {
        if ((bVar1 & 2) == 0) {
          if (param_3 != (int *)0x0) {
            *param_3 = (int)param_2;
          }
          local_8 = 0xffffffff;
          uVar10 = 0x7fffffff;
        }
        else {
          if (param_3 != (int *)0x0) {
            *param_3 = (int)param_2;
          }
          local_8 = 0;
          uVar10 = 0x80000000;
        }
        goto LAB_00551088;
      }
      local_8 = 0xffffffff;
      uVar10 = 0xffffffff;
    }
    if (param_3 != (int *)0x0) {
      *param_3 = (int)param_2;
    }
    goto LAB_00551088;
  }
  if (param_3 != (int *)0x0) {
    *param_3 = (int)pwVar2;
  }
LAB_00551007:
  local_8 = 0;
  uVar10 = 0;
LAB_00551088:
  return CONCAT44(uVar10,local_8);
}

