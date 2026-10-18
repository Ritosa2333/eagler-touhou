/* Entry: 0x00552d10; symbol: FUN_00552d10; body bytes: 45 */

void FUN_00552d10(UINT param_1,char param_2)

{
  HANDLE hProcess;
  UINT uExitCode;
  
  if (param_2 != '\0') {
    uExitCode = param_1;
    hProcess = GetCurrentProcess();
    TerminateProcess(hProcess,uExitCode);
  }
  FUN_00552d3e(param_1);
                    /* WARNING: Subroutine does not return */
  ExitProcess(param_1);
}

