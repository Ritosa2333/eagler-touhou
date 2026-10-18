/* Entry: 0x0051bcc0; symbol: make_error_code; body bytes: 26 */

/* Library Function - Single Match
    class std::error_code __cdecl std::make_error_code(enum std::io_errc)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

io_errc __cdecl std::make_error_code(io_errc param_1)

{
  undefined4 uVar1;
  undefined4 in_stack_00000008;
  
  uVar1 = FUN_0051bb40();
  FUN_0040de00((void *)param_1,in_stack_00000008,uVar1);
  return param_1;
}

