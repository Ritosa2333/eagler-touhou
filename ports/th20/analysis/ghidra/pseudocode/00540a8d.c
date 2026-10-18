/* Entry: 0x00540a8d; symbol: FUN_00540a8d; body bytes: 34 */

undefined4 __cdecl FUN_00540a8d(int param_1)

{
  int *piVar1;
  
  piVar1 = (int *)(param_1 + 0x2c);
  *piVar1 = *piVar1 + -1;
  if (*piVar1 == 0) {
    *(undefined4 *)(param_1 + 0x28) = 0xffffffff;
    ReleaseSRWLockExclusive((PSRWLOCK)(param_1 + 8));
  }
  return 0;
}

