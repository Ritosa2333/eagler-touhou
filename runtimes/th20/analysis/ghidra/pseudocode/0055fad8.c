/* Entry: 0x0055fad8; symbol: ___acrt_lowio_unlock_fh; body bytes: 35 */

/* Library Function - Single Match
    ___acrt_lowio_unlock_fh
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___acrt_lowio_unlock_fh(uint param_1)

{
  LeaveCriticalSection
            ((LPCRITICAL_SECTION)((param_1 & 0x3f) * 0x38 + (&DAT_005e5690)[(int)param_1 >> 6]));
  return;
}

