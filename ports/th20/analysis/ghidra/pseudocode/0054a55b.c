/* Entry: 0x0054a55b; symbol: assemble_floating_point_snan<double>; body bytes: 33 */

/* Library Function - Single Match
    void __cdecl __crt_strtox::assemble_floating_point_snan<double>(bool,double &)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __crt_strtox::assemble_floating_point_snan<double>(bool param_1,double *param_2)

{
  *(undefined4 *)param_2 = 1;
  *(uint *)((int)param_2 + 4) = (uint)param_1 << 0x1f | 0x7ff00000;
  return;
}

