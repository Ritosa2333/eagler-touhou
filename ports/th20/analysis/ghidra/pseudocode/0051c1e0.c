/* Entry: 0x0051c1e0; symbol: FUN_0051c1e0; body bytes: 87 */

/* WARNING: Removing unreachable block (ram,0x0051c1ff) */

undefined1 __fastcall FUN_0051c1e0(basic_streambuf<char,struct_std::char_traits<char>_> *param_1)

{
  char *pcVar1;
  longlong lVar2;
  undefined1 local_c;
  
  lVar2 = Gnavail();
  if (lVar2 < 1) {
    local_c = (**(code **)(*(int *)param_1 + 0x1c))();
  }
  else {
    pcVar1 = std::basic_streambuf<char,struct_std::char_traits<char>_>::_Gninc(param_1);
    local_c = FUN_004ed5a0(*pcVar1);
  }
  return local_c;
}

