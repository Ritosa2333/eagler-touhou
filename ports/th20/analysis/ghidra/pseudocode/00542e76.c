/* Entry: 0x00542e76; symbol: operator_delete; body bytes: 14 */

/* Library Function - Single Match
    void __cdecl operator delete(void *,enum std::align_val_t)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl operator_delete(void *param_1,align_val_t param_2)

{
  __aligned_free(param_1);
  return;
}

