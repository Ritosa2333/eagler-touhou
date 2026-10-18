/* Entry: 0x0051ccf0; symbol: FUN_0051ccf0; body bytes: 98 */

uint __fastcall FUN_0051ccf0(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  bool bVar1;
  undefined1 uVar2;
  int iVar3;
  int _FileHandle;
  char *pcVar4;
  undefined3 extraout_var;
  int in_stack_ffffffec;
  undefined4 local_c;
  
  iVar3 = (**(code **)(*(int *)param_1 + 0x18))();
  _FileHandle = eof(in_stack_ffffffec);
  bVar1 = FUN_0051b5b0(_FileHandle,iVar3);
  if (bVar1) {
    local_c = eof(_FileHandle);
  }
  else {
    pcVar4 = std::basic_streambuf<char,struct_std::char_traits<char>_>::_Gninc(param_1);
    uVar2 = FUN_004ed5a0(*pcVar4);
    local_c = CONCAT31(extraout_var,uVar2);
  }
  return local_c;
}

