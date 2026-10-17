/* Entry: 0x00428990; symbol: FUN_00428990; body bytes: 252 */

void __fastcall FUN_00428990(int param_1)

{
  DWORD DVar1;
  char **in_stack_fffffff4;
  char **this;
  
  if (*(int *)(param_1 + 0x57c4) != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)"Streming BGM stop\r\n",in_stack_fffffff4);
    this = *(char ***)(param_1 + 0x57c4);
    FUN_0045bb00(this,1);
    if (*(int *)(param_1 + 0x14) != 0) {
      PostThreadMessageW(*(DWORD *)(param_1 + 0x10),0x12,0,0);
      std::_Adl_verify_range<char*,char_const*>((char **)"stop m_dwNotifyThreadID\r\n",this);
      while( true ) {
        DVar1 = WaitForSingleObject(*(HANDLE *)(param_1 + 0x14),0x100);
        if (DVar1 == 0) break;
        PostThreadMessageW(*(DWORD *)(param_1 + 0x10),0x12,0,0);
      }
      std::_Adl_verify_range<char*,char_const*>((char **)"stop comp\r\n",this);
      CloseHandle(*(HANDLE *)(param_1 + 0x14));
      CloseHandle(*(HANDLE *)(param_1 + 0x57cc));
      *(undefined4 *)(param_1 + 0x14) = 0;
    }
    if (*(int *)(param_1 + 0x57c4) != 0) {
      FUN_0041f7c0(*(undefined4 **)(param_1 + 0x57c4));
      *(undefined4 *)(param_1 + 0x57c4) = 0;
    }
  }
  return;
}

