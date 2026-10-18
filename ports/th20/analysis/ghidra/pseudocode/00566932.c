/* Entry: 0x00566932; symbol: ___dcrt_lowio_ensure_console_output_initialized; body bytes: 31 */

/* Library Function - Single Match
    ___dcrt_lowio_ensure_console_output_initialized
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

bool ___dcrt_lowio_ensure_console_output_initialized(void)

{
  if (DAT_005b3020 == -2) {
    __dcrt_lowio_initialize_console_output();
  }
  return DAT_005b3020 != -1;
}

