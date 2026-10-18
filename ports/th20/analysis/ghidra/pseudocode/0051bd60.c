/* Entry: 0x0051bd60; symbol: FUN_0051bd60; body bytes: 62 */

uint __cdecl FUN_0051bd60(uint param_1)

{
  uint uVar1;
  int iVar2;
  int in_stack_fffffff4;
  uint local_8;
  
  uVar1 = eof(in_stack_fffffff4);
  local_8 = param_1;
  if (param_1 == uVar1) {
    iVar2 = eof(in_stack_fffffff4);
    local_8 = (uint)(iVar2 == 0);
  }
  return local_8;
}

