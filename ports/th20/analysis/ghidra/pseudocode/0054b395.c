/* Entry: 0x0054b395; symbol: FUN_0054b395; body bytes: 207 */

int __cdecl
FUN_0054b395(wchar_t *param_1,
            input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>
            *param_2,undefined4 param_3,undefined4 param_4)

{
  wchar_t wVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_> *local_10;
  wchar_t *local_c;
  undefined4 *local_8;
  
  local_8 = &param_3;
  iVar4 = 0;
  local_10 = param_2;
  iVar3 = 0;
  local_c = param_1;
  while ((*param_1 == *(wchar_t *)((int)&DAT_00598910 + iVar3) ||
         (*param_1 == *(wchar_t *)((int)&DAT_00598918 + iVar3)))) {
    wVar1 = FUN_0054eec7((undefined4 *)param_2);
    iVar3 = iVar3 + 2;
    *param_1 = wVar1;
    if (iVar3 == 6) {
      __crt_strtox::
      input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::unget
                (param_2,wVar1);
      param_4 = *(undefined4 *)(param_2 + 0x14);
      param_3 = *(undefined4 *)(param_2 + 0x10);
      wVar1 = FUN_0054eec7((undefined4 *)param_2);
      *param_1 = wVar1;
      while ((wVar1 == *(wchar_t *)((int)L"INITY" + iVar4) ||
             (wVar1 == *(wchar_t *)((int)L"inity" + iVar4)))) {
        wVar1 = FUN_0054eec7((undefined4 *)param_2);
        iVar4 = iVar4 + 2;
        *param_1 = wVar1;
        if (iVar4 == 10) {
          __crt_strtox::
          input_adapter_character_source<class___crt_stdio_input::string_input_adapter<wchar_t>_>::
          unget(param_2,wVar1);
          return 3;
        }
      }
      uVar2 = FID_conflict_operator__(&local_10);
      return (uVar2 & 0xff ^ 1) * 4 + 3;
    }
  }
  FID_conflict_operator__(&local_10);
  return 7;
}

