/* Entry: 0x0054f2e5; symbol: FUN_0054f2e5; body bytes: 352 */

undefined4 __fastcall FUN_0054f2e5(format_string_parser<wchar_t> *param_1)

{
  ushort uVar1;
  ushort *puVar2;
  undefined4 uVar3;
  uint extraout_EAX;
  
  uVar1 = **(ushort **)(param_1 + 8);
  if (uVar1 < 0x65) {
    if (uVar1 == 100) {
      *(undefined4 *)(param_1 + 0x30) = 3;
    }
    else {
      if (uVar1 < 0x4a) {
        if (uVar1 == 0x49) {
LAB_0054f3dd:
          *(undefined4 *)(param_1 + 0x30) = 2;
          goto LAB_0054f43b;
        }
        if (uVar1 != 0x41) {
          if (uVar1 == 0x43) {
LAB_0054f35b:
            if (*(int *)(param_1 + 0x20) == 0 && *(int *)(param_1 + 0x24) == 0) {
              *(undefined4 *)(param_1 + 0x24) = 0;
              *(undefined4 *)(param_1 + 0x20) = 1;
            }
            puVar2 = (ushort *)FUN_0054f94e((int)param_1);
            *(undefined4 *)(param_1 + 0x30) = 0;
            goto LAB_0054f378;
          }
          if (((uVar1 != 0x45) && (uVar1 != 0x46)) && (uVar1 != 0x47)) goto LAB_0054f404;
        }
      }
      else {
        if (uVar1 == 0x53) {
LAB_0054f41a:
          puVar2 = (ushort *)FUN_0054f94e((int)param_1);
          *(undefined4 *)(param_1 + 0x30) = 1;
LAB_0054f378:
          *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 2;
          goto LAB_0054f441;
        }
        if (uVar1 == 0x58) goto LAB_0054f434;
        if (uVar1 == 0x5b) {
          FUN_0054f94e((int)param_1);
          *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 2;
          *(undefined4 *)(param_1 + 0x30) = 8;
          uVar3 = FUN_0054f7b2(param_1);
          return uVar3;
        }
        if (uVar1 != 0x61) {
          if (uVar1 != 99) goto LAB_0054f404;
          goto LAB_0054f35b;
        }
      }
LAB_0054f329:
      *(undefined4 *)(param_1 + 0x30) = 7;
    }
  }
  else if (uVar1 < 0x70) {
    if (uVar1 == 0x6f) {
      *(undefined4 *)(param_1 + 0x30) = 4;
    }
    else {
      if (((uVar1 == 0x65) || (uVar1 == 0x66)) || (uVar1 == 0x67)) goto LAB_0054f329;
      if (uVar1 == 0x69) goto LAB_0054f3dd;
      if (uVar1 != 0x6e) {
LAB_0054f404:
        __crt_stdio_input::format_string_parser<wchar_t>::reset_token_state_for_error(param_1,0x16);
        return extraout_EAX & 0xffffff00;
      }
      *(undefined4 *)(param_1 + 0x30) = 9;
    }
  }
  else {
    if (uVar1 == 0x70) {
      *(undefined4 *)(param_1 + 0x28) = 9;
    }
    else {
      if (uVar1 == 0x73) goto LAB_0054f41a;
      if (uVar1 == 0x75) {
        *(undefined4 *)(param_1 + 0x30) = 5;
        goto LAB_0054f43b;
      }
      if (uVar1 != 0x78) goto LAB_0054f404;
    }
LAB_0054f434:
    *(undefined4 *)(param_1 + 0x30) = 6;
  }
LAB_0054f43b:
  puVar2 = *(ushort **)(param_1 + 8) + 1;
  *(ushort **)(param_1 + 8) = puVar2;
LAB_0054f441:
  return CONCAT31((int3)((uint)puVar2 >> 8),1);
}

