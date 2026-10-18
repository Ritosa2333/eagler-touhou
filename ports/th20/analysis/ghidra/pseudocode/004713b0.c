/* Entry: 0x004713b0; symbol: __ExceptionPtrDestroy; body bytes: 16 */

/* Library Function - Single Match
    void __cdecl __ExceptionPtrDestroy(void *)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __cdecl __ExceptionPtrDestroy(void *param_1)

{
  Concurrency::details::_Condition_variable::_scalar_deleting_destructor_
            ((_Condition_variable *)param_1,0);
  return;
}

