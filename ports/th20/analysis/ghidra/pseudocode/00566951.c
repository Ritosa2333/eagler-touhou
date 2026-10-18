/* Entry: 0x00566951; symbol: FID_conflict:___dcrt_terminate_console_output; body bytes: 23 */

/* Library Function - Multiple Matches With Different Base Names
    ___dcrt_terminate_console_input
    ___dcrt_terminate_console_output
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void FID_conflict____dcrt_terminate_console_output(void)

{
  if ((DAT_005b3020 != (HANDLE)0xffffffff) && (DAT_005b3020 != (HANDLE)0xfffffffe)) {
    CloseHandle(DAT_005b3020);
  }
  return;
}

