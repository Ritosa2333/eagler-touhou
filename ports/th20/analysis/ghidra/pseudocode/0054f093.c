/* Entry: 0x0054f093; symbol: FUN_0054f093; body bytes: 124 */

uint __fastcall
FUN_0054f093(input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
             *param_1)

{
  bool bVar1;
  uint uVar2;
  uint3 uVar3;
  undefined3 extraout_var;
  conversion_mode cVar4;
  
  uVar3 = (uint3)((uint)*(undefined4 *)(param_1 + 0x48) >> 8);
  switch(*(undefined4 *)(param_1 + 0x48)) {
  case 0:
    cVar4 = 0;
    goto LAB_0054f0a4;
  case 1:
    cVar4 = 1;
    goto LAB_0054f0a4;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    break;
  case 7:
    __crt_stdio_input::
    input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>::
    process_whitespace(param_1);
    uVar2 = FUN_0054ef86((int)(param_1 + 0x18));
    if (uVar2 == 4) {
      uVar2 = FUN_0054befc(param_1);
      return uVar2;
    }
    if (uVar2 == 8) {
      uVar2 = FUN_0054bf64(param_1);
      return uVar2;
    }
    return uVar2 & 0xffffff00;
  case 8:
    cVar4 = 8;
LAB_0054f0a4:
    bVar1 = __crt_stdio_input::
            input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>::
            process_string_specifier(param_1,cVar4);
    return CONCAT31(extraout_var,bVar1);
  case 9:
    if (param_1[0x30] ==
        (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>)0x0) {
      uVar2 = FUN_0054fba4(param_1,*(int *)(param_1 + 0x10) - *(int *)(param_1 + 8) >> 1,0);
      return uVar2;
    }
    return CONCAT31(uVar3,1);
  default:
    return (uint)uVar3 << 8;
  }
  uVar2 = FUN_0054f137(param_1);
  return uVar2;
}

