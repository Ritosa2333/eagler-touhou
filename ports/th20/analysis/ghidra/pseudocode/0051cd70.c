/* Entry: 0x0051cd70; symbol: FUN_0051cd70; body bytes: 240 */

void __fastcall FUN_0051cd70(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  char *pcVar1;
  char *pcVar2;
  uint *puVar3;
  char *pcVar4;
  undefined1 *puVar5;
  int in_stack_ffffffe0;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pcVar1 = (char *)FUN_0051bab0((int)param_1);
  if (pcVar1 == (char *)0x0) {
    eof(in_stack_ffffffe0);
  }
  else {
    pcVar2 = std::basic_streambuf<char,struct_std::char_traits<char>_>::egptr(param_1);
    if (pcVar1 < pcVar2) {
      FUN_004ed5a0(*pcVar1);
    }
    else {
      local_c = FUN_0051c110((int)param_1);
      if ((local_c == 0) || ((*(uint *)(param_1 + 0x3c) & 4) != 0)) {
        eof(in_stack_ffffffe0);
      }
      else {
        puVar3 = _Max_value<>((uint *)(param_1 + 0x38),&local_c);
        pcVar2 = (char *)*puVar3;
        if (pcVar1 < pcVar2) {
          *(char **)(param_1 + 0x38) = pcVar2;
          pcVar1 = (char *)FUN_0051bab0((int)param_1);
          pcVar4 = (char *)FUN_0051b4c0((int)param_1);
          std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                    (param_1,pcVar4,pcVar1,pcVar2);
          puVar5 = (undefined1 *)FUN_0051bab0((int)param_1);
          FUN_004ed5a0(*puVar5);
        }
        else {
          eof(in_stack_ffffffe0);
        }
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

