/* Entry: 0x0054fb1e; symbol: write_character; body bytes: 133 */

/* Library Function - Multiple Matches With Same Base Name
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::console_input_adapter<wchar_t> >::write_character(char * const,unsigned
   int,char * &,unsigned int &,wchar_t)
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::stream_input_adapter<wchar_t> >::write_character(char * const,unsigned
   int,char * &,unsigned int &,wchar_t)
    private: bool __thiscall __crt_stdio_input::input_processor<wchar_t,class
   __crt_stdio_input::string_input_adapter<wchar_t> >::write_character(char * const,unsigned
   int,char * &,unsigned int &,wchar_t)
   
   Library: Visual Studio 2019 Release */

undefined4 write_character(undefined1 *param_1,int param_2,int *param_3,uint *param_4,WCHAR param_5)

{
  uint *puVar1;
  int iVar2;
  uint *local_8;
  
  local_8 = (uint *)0x0;
  if (param_2 == -1) {
    puVar1 = (uint *)FUN_00559260((int *)&local_8,(byte *)*param_3,5,param_5);
    if (puVar1 == (uint *)0x0) {
      *param_3 = *param_3 + (int)local_8;
      *param_4 = *param_4 - (int)local_8;
      puVar1 = param_4;
    }
    else if ((puVar1 == (uint *)0x16) || (puVar1 == (uint *)0x22)) {
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
    iVar2 = FUN_00559260((int *)&local_8,(byte *)*param_3,*param_4,param_5);
    if (iVar2 == 0x22) {
      *param_1 = 0;
      return (uint)param_1 & 0xffffff00;
    }
    puVar1 = local_8;
    if (0 < (int)local_8) {
      *param_3 = *param_3 + (int)local_8;
      *param_4 = *param_4 - (int)local_8;
    }
  }
  return CONCAT31((int3)((uint)puVar1 >> 8),1);
}

