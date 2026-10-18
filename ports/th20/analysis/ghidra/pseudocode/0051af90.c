/* Entry: 0x0051af90; symbol: FUN_0051af90; body bytes: 173 */

void __fastcall FUN_0051af90(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  int iVar1;
  undefined4 uVar2;
  LPVOID pvVar3;
  char *local_c;
  
  if ((*(uint *)(param_1 + 0x3c) & 1) != 0) {
    iVar1 = FUN_0051c110((int)param_1);
    if (iVar1 == 0) {
      local_c = std::basic_streambuf<char,struct_std::char_traits<char>_>::egptr(param_1);
    }
    else {
      local_c = (char *)FUN_0051b560((int)param_1);
    }
    iVar1 = FUN_0051b4c0((int)param_1);
    uVar2 = FUN_0051b4c0((int)param_1);
    pvVar3 = (LPVOID)FUN_0048b180(uVar2);
    FUN_004100a0(pvVar3,(int)local_c - iVar1);
  }
  std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
            (param_1,(char *)0x0,(char *)0x0,(char *)0x0);
  std::basic_streambuf<char,struct_std::char_traits<char>_>::setp(param_1,(char *)0x0,(char *)0x0);
  *(undefined4 *)(param_1 + 0x38) = 0;
  *(uint *)(param_1 + 0x3c) = *(uint *)(param_1 + 0x3c) & 0xfffffffe;
  return;
}

