/* Entry: 0x004111c0; symbol: make_error_code; body bytes: 26 */

/* Library Function - Multiple Matches With Same Base Name
    class std::error_code __cdecl std::make_error_code(enum std::io_errc)
    class std::error_code __cdecl std::make_error_code(enum std::io_errc::io_errc)
   
   Library: Visual Studio */

void * __cdecl make_error_code(void *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_00411160();
  FUN_0040de00(param_1,param_2,uVar1);
  return param_1;
}

