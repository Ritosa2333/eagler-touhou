/* Entry: 0x0054c645; symbol: assemble_floating_point_infinity; body bytes: 64 */

/* Library Function - Single Match
    void __cdecl __crt_strtox::assemble_floating_point_infinity(bool,class
   __crt_strtox::floating_point_value const &)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl
__crt_strtox::assemble_floating_point_infinity(bool param_1,floating_point_value *param_2)

{
  double *pdVar1;
  float *pfVar2;
  
  if (param_2[4] != (floating_point_value)0x0) {
    pdVar1 = floating_point_value::as_double(param_2);
    *(undefined4 *)pdVar1 = 0;
    *(uint *)((int)pdVar1 + 4) = (uint)param_1 << 0x1f | 0x7ff00000;
    return;
  }
  pfVar2 = floating_point_value::as_float(param_2);
  *pfVar2 = (float)((uint)param_1 << 0x1f | 0x7f800000);
  return;
}

