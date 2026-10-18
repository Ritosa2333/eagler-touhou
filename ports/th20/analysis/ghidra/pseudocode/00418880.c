/* Entry: 0x00418880; symbol: __ExceptionPtrDestroy; body bytes: 16 */

/* Library Function - Single Match
    void __cdecl __ExceptionPtrDestroy(void *)
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

void __cdecl __ExceptionPtrDestroy(void *param_1)

{
  std::shared_ptr<class___ExceptionPtr>::_scalar_deleting_destructor_
            ((shared_ptr<class___ExceptionPtr> *)param_1,0);
  return;
}

