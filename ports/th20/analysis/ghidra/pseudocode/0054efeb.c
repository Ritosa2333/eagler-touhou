/* Entry: 0x0054efeb; symbol: FUN_0054efeb; body bytes: 142 */

int __fastcall
FUN_0054efeb(input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>
             *param_1)

{
  string_input_adapter<wchar_t> *this;
  int iVar1;
  bool bVar2;
  ushort uVar3;
  uint uVar4;
  undefined4 uVar5;
  __acrt_ptd *p_Var6;
  int iVar7;
  
  this = (string_input_adapter<wchar_t> *)(param_1 + 8);
  uVar4 = FUN_0054fafc((int)this);
  if ((char)uVar4 != '\0') {
    uVar5 = FUN_0054fabf((int)(param_1 + 0x18));
    if ((char)uVar5 != '\0') {
      do {
        uVar5 = FUN_0054c4fa((format_string_parser<wchar_t> *)(param_1 + 0x18));
        if ((char)uVar5 == '\0') break;
        bVar2 = __crt_stdio_input::
                input_processor<wchar_t,class___crt_stdio_input::string_input_adapter<wchar_t>_>::
                process_state(param_1);
      } while (bVar2);
      iVar7 = *(int *)(param_1 + 0x58);
      if ((iVar7 == 0) && (*(int *)(param_1 + 0x28) != 1)) {
        uVar3 = __crt_stdio_input::string_input_adapter<wchar_t>::get(this);
        if (uVar3 == 0xffff) {
          iVar7 = -1;
        }
        __crt_stdio_input::string_input_adapter<wchar_t>::unget(this,uVar3);
      }
      if ((*(uint *)param_1 & 1) == 0) {
        return iVar7;
      }
      iVar1 = *(int *)(param_1 + 0x24);
      if (iVar1 == 0) {
        return iVar7;
      }
      p_Var6 = FUN_005516c1();
      *(int *)p_Var6 = iVar1;
      FUN_005480bc();
      return iVar7;
    }
  }
  return -1;
}

