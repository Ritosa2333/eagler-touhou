/* Entry: 0x0055737d; symbol: siglookup; body bytes: 40 */

/* Library Function - Single Match
    struct __crt_signal_action_t * __cdecl siglookup(int,struct __crt_signal_action_t * const)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

__crt_signal_action_t * __cdecl siglookup(int param_1,__crt_signal_action_t *param_2)

{
  __crt_signal_action_t *p_Var1;
  
  p_Var1 = param_2 + DAT_005993d8 * 0xc;
  if (param_2 != p_Var1) {
    do {
      if (*(int *)(param_2 + 4) == param_1) {
        return param_2;
      }
      param_2 = param_2 + 0xc;
    } while (param_2 != p_Var1);
  }
  return (__crt_signal_action_t *)0x0;
}

