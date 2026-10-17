/* Entry: 0x00540842; symbol: FUN_00540842; body bytes: 96 */

void FUN_00540842(uint param_1)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = param_1 >> 0x10 ^ param_1;
  iVar3 = ((uVar2 >> 8 ^ uVar2) & 0xff) * 0x40;
  AcquireSRWLockExclusive((PSRWLOCK)(&DAT_005e0b80 + iVar3));
  puVar1 = *(uint **)(&DAT_005e0b88 + iVar3);
  if (puVar1 != (uint *)0x0) {
    for (; puVar1 != (uint *)(iVar3 + 0x5e0b84); puVar1 = (uint *)puVar1[1]) {
      if (*puVar1 == param_1) {
        WakeAllConditionVariable((PCONDITION_VARIABLE)(puVar1 + 3));
      }
    }
  }
  ReleaseSRWLockExclusive((PSRWLOCK)(&DAT_005e0b80 + iVar3));
  return;
}

