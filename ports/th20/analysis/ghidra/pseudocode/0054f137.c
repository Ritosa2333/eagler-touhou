/* Entry: 0x0054f137; symbol: FUN_0054f137; body bytes: 101 */

int __fastcall
FUN_0054f137(input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
             *param_1)

{
  undefined4 *puVar1;
  uint3 uVar3;
  int iVar2;
  ulonglong uVar4;
  undefined1 auStack_3c [28];
  undefined4 uStack_20;
  undefined4 uStack_8;
  
  uStack_8 = param_1;
  __crt_stdio_input::
  input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>::
  process_whitespace(param_1);
  puVar1 = *(undefined4 **)(param_1 + 0x50);
  uStack_8 = (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_> *)
             ((uint)uStack_8 & 0xffffff);
  FUN_0054a75b(auStack_3c,param_1 + 8,*(undefined4 *)(param_1 + 0x38),
               *(undefined4 *)(param_1 + 0x3c),(undefined1 *)((int)&uStack_8 + 3));
  uVar4 = FUN_0054bea0(puVar1);
  uVar3 = (uint3)(uVar4 >> 8);
  if (uStack_8._3_1_ == '\0') {
    iVar2 = (uint)uVar3 << 8;
  }
  else if (param_1[0x30] ==
           (input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>)0x0) {
    uStack_20 = 0x54f196;
    iVar2 = FUN_0054fba4(param_1,(int)uVar4,(int)(uVar4 >> 0x20));
  }
  else {
    iVar2 = CONCAT31(uVar3,1);
  }
  return iVar2;
}

