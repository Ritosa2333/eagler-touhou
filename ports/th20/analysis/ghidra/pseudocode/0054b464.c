/* Entry: 0x0054b464; symbol: FUN_0054b464; body bytes: 289 */

undefined4 __cdecl
FUN_0054b464(wchar_t *param_1,
            input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
            *param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  wchar_t wVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_> *local_10;
  wchar_t *local_c;
  undefined4 *local_8;
  
  local_8 = &param_3;
  iVar5 = 0;
  local_10 = param_2;
  local_c = param_1;
  do {
    if ((*param_1 != *(wchar_t *)((int)&DAT_00598938 + iVar5)) &&
       (*param_1 != *(wchar_t *)((int)&DAT_00598940 + iVar5))) {
      FID_conflict_operator__(&local_10);
      return 7;
    }
    wVar2 = FUN_0054eec7((undefined4 *)param_2);
    iVar5 = iVar5 + 2;
    *param_1 = wVar2;
  } while (iVar5 != 6);
  __crt_strtox::
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
            (param_2,wVar2);
  param_4 = *(undefined4 *)(param_2 + 0x14);
  param_3 = *(undefined4 *)(param_2 + 0x10);
  wVar2 = FUN_0054eec7((undefined4 *)param_2);
  *param_1 = wVar2;
  if (wVar2 == L'(') {
    wVar2 = FUN_0054eec7((undefined4 *)param_2);
    *param_1 = wVar2;
    uVar3 = parse_floating_point_possible_nan_is_snan<>((ushort *)param_1,(undefined4 *)param_2);
    if ((char)uVar3 == '\0') {
      uVar3 = parse_floating_point_possible_nan_is_ind<>((ushort *)param_1,(undefined4 *)param_2);
      wVar2 = *param_1;
      if ((char)uVar3 == '\0') {
        while (wVar2 != L')') {
          uVar4 = (uint)(ushort)wVar2;
          if ((wVar2 == L'\0') ||
             ((((9 < uVar4 - 0x30 && (0x19 < uVar4 - 0x61)) && (0x19 < uVar4 - 0x41)) &&
              (wVar2 != L'_')))) goto LAB_0054b4ce;
          wVar2 = FUN_0054eec7((undefined4 *)param_2);
          *param_1 = wVar2;
        }
        uVar3 = 4;
      }
      else {
        __crt_strtox::
        input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::
        unget(param_2,wVar2);
        uVar3 = 6;
      }
    }
    else {
      __crt_strtox::
      input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
                (param_2,*param_1);
      uVar3 = 5;
    }
  }
  else {
LAB_0054b4ce:
    cVar1 = FID_conflict_operator__(&local_10);
    uVar3 = 7;
    if (cVar1 != '\0') {
      uVar3 = 4;
    }
  }
  return uVar3;
}

