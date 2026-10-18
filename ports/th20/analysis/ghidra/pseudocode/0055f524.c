/* Entry: 0x0055f524; symbol: ensure_current_environment_is_not_initial_environment_nolock<wchar_t>; body bytes: 26 */

/* Library Function - Single Match
    void __cdecl ensure_current_environment_is_not_initial_environment_nolock<wchar_t>(void)
   
   Library: Visual Studio 2019 Release */

void __cdecl ensure_current_environment_is_not_initial_environment_nolock<wchar_t>(void)

{
  if (DAT_005e55ac == DAT_005e55b0) {
    DAT_005e55ac = copy_environment<wchar_t>(DAT_005e55ac);
  }
  return;
}

