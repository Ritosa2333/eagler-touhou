/* Entry: 0x005534f6; symbol: uninitialize_environment_internal<>; body bytes: 27 */

/* Library Function - Multiple Matches With Same Base Name
    void __cdecl uninitialize_environment_internal<char>(char * * &)
    void __cdecl uninitialize_environment_internal<wchar_t>(wchar_t * * &)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl uninitialize_environment_internal<>(undefined4 *param_1)

{
  if ((undefined4 *)*param_1 != DAT_005e55b0) {
    free_environment<>((undefined4 *)*param_1);
  }
  return;
}

