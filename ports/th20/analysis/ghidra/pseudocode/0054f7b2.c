/* Entry: 0x0054f7b2; symbol: FUN_0054f7b2; body bytes: 364 */

uint __fastcall FUN_0054f7b2(format_string_parser<wchar_t> *param_1)

{
  scanset_buffer<wchar_t> *this;
  short sVar1;
  wchar_t wVar2;
  wchar_t *pwVar3;
  wchar_t *pwVar4;
  int iVar5;
  uint extraout_EAX;
  undefined4 uVar6;
  wchar_t wVar7;
  wchar_t wVar8;
  short *psVar9;
  wchar_t *pwVar10;
  short sVar11;
  wchar_t *local_c;
  
  this = (scanset_buffer<wchar_t> *)(param_1 + 0x34);
  iVar5 = FUN_0054e9a0((int *)this);
  if (iVar5 == 0) {
    iVar5 = 0xc;
  }
  else {
    FUN_0054f27b((int *)this);
    psVar9 = *(short **)(param_1 + 8);
    sVar1 = *psVar9;
    sVar11 = sVar1;
    if (sVar1 == 0x5e) {
      psVar9 = psVar9 + 1;
      *(short **)(param_1 + 8) = psVar9;
      sVar11 = *psVar9;
    }
    if (sVar11 == 0x5d) {
      *(short **)(param_1 + 8) = psVar9 + 1;
      __crt_stdio_input::scanset_buffer<wchar_t>::set(this,L']');
    }
    pwVar3 = *(wchar_t **)(param_1 + 8);
    local_c = (wchar_t *)0x0;
    wVar2 = *pwVar3;
    pwVar10 = pwVar3;
    pwVar4 = local_c;
    while ((wVar2 != L']' && (wVar2 != L'\0'))) {
      if ((wVar2 == L'-') && ((pwVar10 + -1 != pwVar4 && (pwVar10 != pwVar3)))) {
        local_c = pwVar10 + 1;
        if (*local_c == L']') goto LAB_0054f8fa;
        wVar2 = pwVar10[-1];
        wVar8 = *local_c;
        wVar7 = wVar2;
        if ((ushort)wVar2 <= (ushort)wVar8) {
          wVar7 = *local_c;
        }
        if ((ushort)wVar2 <= (ushort)wVar8) {
          wVar8 = wVar2;
        }
        for (; wVar8 != (wchar_t)(wVar7 + L'\x01'); wVar8 = wVar8 + L'\x01') {
          __crt_stdio_input::scanset_buffer<wchar_t>::set
                    ((scanset_buffer<wchar_t> *)(param_1 + 0x34),wVar8);
        }
      }
      else {
LAB_0054f8fa:
        __crt_stdio_input::scanset_buffer<wchar_t>::set
                  ((scanset_buffer<wchar_t> *)(param_1 + 0x34),wVar2);
        local_c = pwVar4;
      }
      pwVar10 = (wchar_t *)(*(int *)(param_1 + 8) + 2);
      *(wchar_t **)(param_1 + 8) = pwVar10;
      pwVar4 = local_c;
      wVar2 = *pwVar10;
    }
    if (*pwVar10 != L'\0') {
      uVar6 = 0;
      if (sVar1 == 0x5e) {
        uVar6 = FUN_0054ef27((int *)(param_1 + 0x34));
      }
      *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 2;
      return CONCAT31((int3)((uint)uVar6 >> 8),1);
    }
    iVar5 = 0x16;
  }
  __crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error(param_1,iVar5);
  return extraout_EAX & 0xffffff00;
}

