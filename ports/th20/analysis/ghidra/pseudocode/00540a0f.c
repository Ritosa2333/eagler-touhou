/* Entry: 0x00540a0f; symbol: FUN_00540a0f; body bytes: 24 */

int __cdecl FUN_00540a0f(HANDLE param_1)

{
  BOOL BVar1;
  
  BVar1 = CloseHandle(param_1);
  return (-(uint)(BVar1 != 0) & 0xfffffffc) + 4;
}

