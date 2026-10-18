/* Entry: 0x005577e1; symbol: destroy_fls; body bytes: 33 */

/* Library Function - Single Match
    void __stdcall destroy_fls(void *)
   
   Library: Visual Studio 2019 Release */

void destroy_fls(void *param_1)

{
  if (param_1 != (void *)0x0) {
    destroy_ptd((__acrt_ptd *)param_1);
    FUN_00557ba0(param_1);
  }
  return;
}

