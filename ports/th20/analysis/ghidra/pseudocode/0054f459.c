/* Entry: 0x0054f459; symbol: FUN_0054f459; body bytes: 507 */

uint __fastcall FUN_0054f459(format_string_parser<wchar_t> *param_1)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  uint extraout_EAX;
  ushort uVar4;
  uint uVar5;
  int iVar6;
  longlong lVar7;
  int local_8;
  
  iVar6 = 0x30;
  uVar1 = **(ushort **)(param_1 + 8);
  uVar2 = (uint)uVar1;
  if (uVar1 < 0x30) goto LAB_0054f5ee;
  if (uVar2 < 0x3a) {
LAB_0054f5e5:
    uVar5 = uVar2 - iVar6;
    uVar3 = uVar2;
    if (uVar5 == 0xffffffff) goto LAB_0054f5ee;
  }
  else {
    iVar6 = 0xff10;
    if (uVar1 < 0xff10) {
      iVar6 = 0x660;
      if (0x65f < uVar1) {
        if (uVar1 < 0x66a) goto LAB_0054f5e5;
        iVar6 = 0x6f0;
        if (0x6ef < uVar1) {
          if (uVar1 < 0x6fa) goto LAB_0054f5e5;
          iVar6 = 0x966;
          if (0x965 < uVar1) {
            if (uVar1 < 0x970) goto LAB_0054f5e5;
            iVar6 = 0x9e6;
            if (0x9e5 < uVar1) {
              if (uVar1 < 0x9f0) goto LAB_0054f5e5;
              iVar6 = 0xa66;
              if (0xa65 < uVar1) {
                if (uVar1 < 0xa70) goto LAB_0054f5e5;
                iVar6 = 0xae6;
                if (0xae5 < uVar1) {
                  if (uVar1 < 0xaf0) goto LAB_0054f5e5;
                  iVar6 = 0xb66;
                  if (0xb65 < uVar1) {
                    if (uVar1 < 0xb70) goto LAB_0054f5e5;
                    iVar6 = 0xc66;
                    if (0xc65 < uVar1) {
                      if (uVar1 < 0xc70) goto LAB_0054f5e5;
                      iVar6 = 0xce6;
                      if (0xce5 < uVar1) {
                        if (uVar1 < 0xcf0) goto LAB_0054f5e5;
                        iVar6 = 0xd66;
                        if (0xd65 < uVar1) {
                          if (uVar1 < 0xd70) goto LAB_0054f5e5;
                          iVar6 = 0xe50;
                          if (0xe4f < uVar1) {
                            if (uVar1 < 0xe5a) goto LAB_0054f5e5;
                            iVar6 = 0xed0;
                            if (0xecf < uVar1) {
                              if (uVar1 < 0xeda) goto LAB_0054f5e5;
                              iVar6 = 0xf20;
                              if (0xf1f < uVar1) {
                                if (uVar1 < 0xf2a) goto LAB_0054f5e5;
                                iVar6 = 0x1040;
                                if (0x103f < uVar1) {
                                  if (uVar1 < 0x104a) goto LAB_0054f5e5;
                                  iVar6 = 0x17e0;
                                  if (0x17df < uVar1) {
                                    if (uVar1 < 0x17ea) goto LAB_0054f5e5;
                                    iVar6 = 0x1810;
                                    if (0x180f < uVar1) {
                                      uVar4 = 0x181a;
                                      goto LAB_0054f5e0;
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
      uVar4 = 0xff1a;
LAB_0054f5e0:
      if (uVar1 < uVar4) goto LAB_0054f5e5;
    }
LAB_0054f5ee:
    if (((uVar2 < 0x41) || (0x5a < uVar2)) && (uVar3 = uVar2 - 0x61, 0x19 < uVar3))
    goto LAB_0054f641;
    uVar3 = uVar2 - 0x61;
    if (uVar3 < 0x1a) {
      uVar2 = uVar2 - 0x20;
    }
    uVar5 = uVar2 - 0x37;
  }
  if (uVar5 < 10) {
    local_8 = 0;
    lVar7 = FUN_00551762(*(ushort **)(param_1 + 8),&local_8,10);
    uVar3 = (uint)lVar7;
    if ((lVar7 == 0) || (local_8 == *(int *)(param_1 + 8))) {
      __crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error(param_1,0x16);
      return extraout_EAX & 0xffffff00;
    }
    *(longlong *)(param_1 + 0x20) = lVar7;
    *(int *)(param_1 + 8) = local_8;
  }
LAB_0054f641:
  return CONCAT31((int3)(uVar3 >> 8),1);
}

