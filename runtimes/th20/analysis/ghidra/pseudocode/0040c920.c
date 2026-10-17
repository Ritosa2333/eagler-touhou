/* Entry: 0x0040c920; symbol: FUN_0040c920; body bytes: 101 */

uint __cdecl FUN_0040c920(uint param_1)

{
  int iVar1;
  uint uVar2;
  
  if (param_1 + 0x23 <= param_1) {
    std::_Throw_bad_array_new_length();
  }
  iVar1 = FUN_0040eca0(param_1 + 0x23);
  if (iVar1 == 0) {
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  uVar2 = iVar1 + 0x23U & 0xffffffe0;
  *(int *)(uVar2 - 4) = iVar1;
  return uVar2;
}

