/* Entry: 0x0054b871; symbol: FUN_0054b871; body bytes: 1583 */

/* WARNING: Removing unreachable block (ram,0x0054bda1) */

ulonglong __cdecl FUN_0054b871(int *param_1)

{
  ulonglong uVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  ushort uVar6;
  wchar_t wVar7;
  undefined4 uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  byte bVar12;
  uint uVar13;
  uint uVar14;
  wchar_t wVar15;
  uint uVar16;
  undefined8 uVar17;
  ulonglong uVar18;
  int in_stack_00000018;
  int in_stack_0000001c;
  undefined1 *in_stack_00000020;
  uint in_stack_00000028;
  byte in_stack_0000002c;
  wint_t _C;
  uint local_28;
  uint local_24;
  
  uVar8 = FUN_0054fadb((int *)&stack0x00000008);
  uVar11 = in_stack_00000028;
  iVar4 = in_stack_0000001c;
  iVar3 = in_stack_00000018;
  if ((char)uVar8 == '\0') {
    if (in_stack_00000020 == (undefined1 *)0x0) {
      return 0;
    }
    if (in_stack_00000018 != 0 || in_stack_0000001c != 0) {
      return 0;
    }
    *in_stack_00000020 = 0;
    return 0;
  }
  if ((in_stack_00000028 != 0) && (((int)in_stack_00000028 < 2 || (0x24 < (int)in_stack_00000028))))
  {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
    goto LAB_0054bdf4;
  }
  uVar6 = FUN_0054eec7((undefined4 *)&stack0x00000008);
  uVar14 = (uint)uVar6;
  if ((char)param_1[5] == '\0') {
    FUN_0054a360(param_1);
  }
  while( true ) {
    _C = (wint_t)uVar14;
    iVar9 = _iswctype(_C,8);
    if (iVar9 == 0) break;
    uVar6 = FUN_0054eec7((undefined4 *)&stack0x00000008);
    uVar14 = (uint)uVar6;
  }
  bVar2 = in_stack_0000002c | 2;
  if ((_C == 0x2d) || (bVar2 = in_stack_0000002c, _C == 0x2b)) {
    uVar6 = FUN_0054eec7((undefined4 *)&stack0x00000008);
    uVar14 = (uint)uVar6;
  }
  iVar9 = 0x30;
  if ((uVar11 != 0) && (uVar10 = uVar11, uVar11 != 0x10)) goto LAB_0054bb84;
  uVar6 = (ushort)uVar14;
  if (uVar6 < 0x30) {
LAB_0054bb18:
    if (((0x40 < uVar14) && (uVar14 < 0x5b)) || (uVar14 - 0x61 < 0x1a)) {
      uVar10 = uVar14;
      if (uVar14 - 0x61 < 0x1a) {
        uVar10 = uVar14 - 0x20;
      }
      iVar9 = uVar10 - 0x37;
      goto LAB_0054bb3d;
    }
LAB_0054bb77:
    uVar10 = 10;
  }
  else {
    if (0x39 < uVar6) {
      if (uVar6 < 0xff10) {
        if (0x65f < uVar6) {
          if (uVar6 < 0x66a) {
            iVar9 = uVar14 - 0x660;
            goto LAB_0054bb13;
          }
          iVar9 = 0x6f0;
          if ((0x6ef < uVar6) &&
             ((uVar6 < 0x6fa ||
              ((iVar9 = 0x966, 0x965 < uVar6 &&
               ((uVar6 < 0x970 ||
                ((iVar9 = 0x9e6, 0x9e5 < uVar6 &&
                 ((uVar6 < 0x9f0 ||
                  ((iVar9 = 0xa66, 0xa65 < uVar6 &&
                   ((uVar6 < 0xa70 ||
                    ((iVar9 = 0xae6, 0xae5 < uVar6 &&
                     ((uVar6 < 0xaf0 ||
                      ((iVar9 = 0xb66, 0xb65 < uVar6 &&
                       ((uVar6 < 0xb70 ||
                        ((iVar9 = 0xc66, 0xc65 < uVar6 &&
                         ((uVar6 < 0xc70 ||
                          ((iVar9 = 0xce6, 0xce5 < uVar6 &&
                           ((uVar6 < 0xcf0 ||
                            ((iVar9 = 0xd66, 0xd65 < uVar6 &&
                             ((uVar6 < 0xd70 ||
                              ((iVar9 = 0xe50, 0xe4f < uVar6 &&
                               ((uVar6 < 0xe5a ||
                                ((iVar9 = 0xed0, 0xecf < uVar6 &&
                                 ((uVar6 < 0xeda ||
                                  ((iVar9 = 0xf20, 0xf1f < uVar6 &&
                                   ((uVar6 < 0xf2a ||
                                    ((iVar9 = 0x1040, 0x103f < uVar6 &&
                                     ((uVar6 < 0x104a ||
                                      ((iVar9 = 0x17e0, 0x17df < uVar6 &&
                                       ((uVar6 < 0x17ea ||
                                        ((iVar9 = 0x1810, 0x180f < uVar6 && (uVar6 < 0x181a)))))))))
                                     )))))))))))))))))))))))))))))))))))))))))))))))))
          goto LAB_0054b97c;
        }
      }
      else if (uVar6 < 0xff1a) {
        iVar9 = uVar14 - 0xff10;
        goto LAB_0054bb13;
      }
      goto LAB_0054bb18;
    }
LAB_0054b97c:
    iVar9 = uVar14 - iVar9;
LAB_0054bb13:
    if (iVar9 == -1) goto LAB_0054bb18;
LAB_0054bb3d:
    if (iVar9 != 0) goto LAB_0054bb77;
    wVar7 = FUN_0054eec7((undefined4 *)&stack0x00000008);
    if ((wVar7 == L'x') || (wVar7 == L'X')) {
      uVar6 = FUN_0054eec7((undefined4 *)&stack0x00000008);
      uVar14 = (uint)uVar6;
      uVar10 = 0x10;
      in_stack_00000028 = 0x10;
      if (uVar11 != 0) {
        uVar10 = uVar11;
        in_stack_00000028 = uVar11;
      }
      goto LAB_0054bb84;
    }
    __crt_strtox::
    input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
              ((input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
                *)&stack0x00000008,wVar7);
    uVar10 = 8;
  }
  in_stack_00000028 = uVar10;
  if (uVar11 != 0) {
    uVar10 = uVar11;
    in_stack_00000028 = uVar11;
  }
LAB_0054bb84:
  uVar17 = __aulldiv(0xffffffff,0xffffffff,uVar10,(int)uVar10 >> 0x1f);
  uVar1 = 0;
  do {
    uVar11 = (uint)(uVar1 >> 0x20);
    uVar13 = (uint)uVar1;
    iVar9 = 0x30;
    wVar7 = (wchar_t)uVar14;
    if ((ushort)wVar7 < 0x30) goto LAB_0054bd37;
    if ((ushort)wVar7 < 0x3a) {
LAB_0054bd2d:
      uVar16 = uVar14 - iVar9;
      if (uVar16 == 0xffffffff) goto LAB_0054bd37;
    }
    else {
      iVar9 = 0xff10;
      if ((ushort)wVar7 < 0xff10) {
        iVar9 = 0x660;
        if (0x65f < (ushort)wVar7) {
          if ((ushort)wVar7 < 0x66a) goto LAB_0054bd2d;
          iVar9 = 0x6f0;
          if (0x6ef < (ushort)wVar7) {
            if ((ushort)wVar7 < 0x6fa) goto LAB_0054bd2d;
            iVar9 = 0x966;
            if (0x965 < (ushort)wVar7) {
              if ((ushort)wVar7 < 0x970) goto LAB_0054bd2d;
              iVar9 = 0x9e6;
              if (0x9e5 < (ushort)wVar7) {
                if ((ushort)wVar7 < 0x9f0) goto LAB_0054bd2d;
                iVar9 = 0xa66;
                if (0xa65 < (ushort)wVar7) {
                  if ((ushort)wVar7 < 0xa70) goto LAB_0054bd2d;
                  iVar9 = 0xae6;
                  if (0xae5 < (ushort)wVar7) {
                    if ((ushort)wVar7 < 0xaf0) goto LAB_0054bd2d;
                    iVar9 = 0xb66;
                    if (0xb65 < (ushort)wVar7) {
                      if ((ushort)wVar7 < 0xb70) goto LAB_0054bd2d;
                      iVar9 = 0xc66;
                      if (0xc65 < (ushort)wVar7) {
                        if ((ushort)wVar7 < 0xc70) goto LAB_0054bd2d;
                        iVar9 = 0xce6;
                        if (0xce5 < (ushort)wVar7) {
                          if ((ushort)wVar7 < 0xcf0) goto LAB_0054bd2d;
                          iVar9 = 0xd66;
                          if (0xd65 < (ushort)wVar7) {
                            if ((ushort)wVar7 < 0xd70) goto LAB_0054bd2d;
                            iVar9 = 0xe50;
                            if (0xe4f < (ushort)wVar7) {
                              if ((ushort)wVar7 < 0xe5a) goto LAB_0054bd2d;
                              iVar9 = 0xed0;
                              if (0xecf < (ushort)wVar7) {
                                if ((ushort)wVar7 < 0xeda) goto LAB_0054bd2d;
                                iVar9 = 0xf20;
                                if (0xf1f < (ushort)wVar7) {
                                  if ((ushort)wVar7 < 0xf2a) goto LAB_0054bd2d;
                                  iVar9 = 0x1040;
                                  if (0x103f < (ushort)wVar7) {
                                    if ((ushort)wVar7 < 0x104a) goto LAB_0054bd2d;
                                    iVar9 = 0x17e0;
                                    if (0x17df < (ushort)wVar7) {
                                      if ((ushort)wVar7 < 0x17ea) goto LAB_0054bd2d;
                                      iVar9 = 0x1810;
                                      if (0x180f < (ushort)wVar7) {
                                        wVar15 = L'᠚';
                                        goto LAB_0054bd28;
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
        wVar15 = L'：';
LAB_0054bd28:
        if ((ushort)wVar7 < (ushort)wVar15) goto LAB_0054bd2d;
      }
LAB_0054bd37:
      if (((uVar14 < 0x41) || (0x5a < uVar14)) && (0x19 < uVar14 - 0x61)) {
        uVar16 = 0xffffffff;
      }
      else {
        if (uVar14 - 0x61 < 0x1a) {
          uVar14 = uVar14 - 0x20;
        }
        uVar16 = uVar14 - 0x37;
      }
    }
    if (in_stack_00000028 <= uVar16) break;
    uVar18 = __allmul(uVar10,(int)uVar10 >> 0x1f,uVar13,uVar11);
    uVar1 = uVar18 + uVar16;
    local_24 = (uint)((ulonglong)uVar17 >> 0x20);
    if ((uVar11 < local_24) ||
       ((uVar11 <= local_24 && (local_28 = (uint)uVar17, uVar13 <= local_28)))) {
      bVar12 = 0;
    }
    else {
      bVar12 = 1;
    }
    bVar2 = bVar2 | (uVar1 < uVar18 | bVar12) << 2 | 8;
    uVar6 = FUN_0054eec7((undefined4 *)&stack0x00000008);
    uVar14 = (uint)uVar6;
  } while( true );
  __crt_strtox::
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
            ((input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
              *)&stack0x00000008,wVar7);
  if ((bVar2 & 8) != 0) {
    cVar5 = FUN_0054a71a(bVar2,uVar13,uVar11);
    if (cVar5 == '\0') {
      if ((bVar2 & 2) != 0) {
        uVar1 = CONCAT44(-(uVar11 + (uVar13 != 0)),-uVar13);
      }
    }
    else {
      *(undefined1 *)(param_1 + 7) = 1;
      param_1[6] = 0x22;
      if ((bVar2 & 1) != 0) {
        if ((bVar2 & 2) == 0) {
          if ((in_stack_00000020 != (undefined1 *)0x0) &&
             (in_stack_00000018 == 0 && in_stack_0000001c == 0)) {
            *in_stack_00000020 = 0;
          }
          return 0x7fffffffffffffff;
        }
        if ((in_stack_00000020 != (undefined1 *)0x0) &&
           (in_stack_00000018 == 0 && in_stack_0000001c == 0)) {
          *in_stack_00000020 = 0;
        }
        return 0x8000000000000000;
      }
      uVar1 = 0xffffffffffffffff;
    }
    if (in_stack_00000020 != (undefined1 *)0x0) {
      if (in_stack_00000018 == 0 && in_stack_0000001c == 0) {
        *in_stack_00000020 = 0;
        return uVar1;
      }
      return uVar1;
    }
    return uVar1;
  }
  restore_state(&stack0x00000008,iVar3,iVar4);
LAB_0054bdf4:
  if ((in_stack_00000020 != (undefined1 *)0x0) && (in_stack_00000018 == 0 && in_stack_0000001c == 0)
     ) {
    *in_stack_00000020 = 0;
  }
  return 0;
}

