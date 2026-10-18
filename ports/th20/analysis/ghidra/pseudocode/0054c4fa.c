/* Entry: 0x0054c4fa; symbol: FUN_0054c4fa; body bytes: 257 */

uint __fastcall FUN_0054c4fa(format_string_parser<wchar_t> *param_1)

{
  ushort _C;
  wint_t _C_00;
  wint_t *pwVar1;
  uint in_EAX;
  int iVar2;
  uint extraout_EAX;
  
  if (*(int *)(param_1 + 0xc) == 0) {
    *(undefined4 *)(param_1 + 0x10) = 0;
    *(undefined4 *)(param_1 + 0x14) = 0;
    param_1[0x18] = (format_string_parser<wchar_t>)0x0;
    *(undefined4 *)(param_1 + 0x20) = 0;
    *(undefined4 *)(param_1 + 0x24) = 0;
    *(undefined4 *)(param_1 + 0x28) = 0;
    param_1[0x2c] = (format_string_parser<wchar_t>)0x0;
    *(undefined4 *)(param_1 + 0x30) = 0;
    _C = **(ushort **)(param_1 + 8);
    in_EAX = (uint)_C;
    if (_C != 0) {
      iVar2 = _iswctype(_C,8);
      pwVar1 = *(wint_t **)(param_1 + 8);
      if (iVar2 == 0) {
        if ((*pwVar1 == 0x25) && (pwVar1[1] != 0x25)) {
          *(undefined4 *)(param_1 + 0x10) = 4;
          *(wint_t **)(param_1 + 8) = pwVar1 + 1;
          FUN_0054f445((int)param_1);
          in_EAX = FUN_0054f459(param_1);
          if ((char)in_EAX != '\0') {
            FUN_0054f654((int)param_1);
            __crt_stdio_input::format_string_parser<wchar_t>::scan_optional_wide_modifier(param_1);
            in_EAX = FUN_0054f2e5(param_1);
            if ((char)in_EAX != '\0') {
              iVar2 = *(int *)(param_1 + 0x28);
              if ((&DAT_00598898)[iVar2 + *(int *)(param_1 + 0x30) * 0xc] != '\0')
              goto LAB_0054c56b;
              __crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error
                        (param_1,0x16);
              in_EAX = extraout_EAX;
            }
          }
          goto LAB_0054c533;
        }
        *(undefined4 *)(param_1 + 0x10) = 3;
        *(wint_t *)(param_1 + 0x14) = *pwVar1;
        iVar2 = 4;
        if (*pwVar1 != 0x25) {
          iVar2 = 2;
        }
        iVar2 = iVar2 + (int)pwVar1;
        *(int *)(param_1 + 8) = iVar2;
      }
      else {
        *(undefined4 *)(param_1 + 0x10) = 2;
        _C_00 = *pwVar1;
        while (iVar2 = _iswctype(_C_00,8), iVar2 != 0) {
          *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 2;
          _C_00 = **(wint_t **)(param_1 + 8);
        }
      }
LAB_0054c56b:
      return CONCAT31((int3)((uint)iVar2 >> 8),1);
    }
    *(undefined4 *)(param_1 + 0x10) = 1;
  }
LAB_0054c533:
  return in_EAX & 0xffffff00;
}

