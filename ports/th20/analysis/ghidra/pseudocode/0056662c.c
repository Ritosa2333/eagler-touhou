/* Entry: 0x0056662c; symbol: __putwch_nolock; body bytes: 50 */

/* Library Function - Single Match
    __putwch_nolock
   
   Library: Visual Studio 2019 Release */

wint_t __cdecl __putwch_nolock(wchar_t _WCh)

{
  bool bVar1;
  undefined3 extraout_var;
  BOOL BVar2;
  DWORD local_8;
  
  bVar1 = ___dcrt_lowio_ensure_console_output_initialized();
  if (CONCAT31(extraout_var,bVar1) != 0) {
    BVar2 = ___dcrt_write_console(&_WCh,1,&local_8);
    if (BVar2 != 0) {
      return _WCh;
    }
  }
  return 0xffff;
}

