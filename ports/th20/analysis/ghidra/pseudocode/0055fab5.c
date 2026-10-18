/* Entry: 0x0055fab5; symbol: FUN_0055fab5; body bytes: 35 */

void __cdecl FUN_0055fab5(uint param_1)

{
  EnterCriticalSection
            ((LPCRITICAL_SECTION)((param_1 & 0x3f) * 0x38 + (&DAT_005e5690)[(int)param_1 >> 6]));
  return;
}

