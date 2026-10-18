/* Entry: 0x00554310; symbol: __aligned_free; body bytes: 26 */

/* Library Function - Single Match
    __aligned_free
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __aligned_free(void *_Memory)

{
  if (_Memory != (void *)0x0) {
    thunk_FUN_00557ba0(*(LPVOID *)(((uint)_Memory & 0xfffffffc) - 4));
  }
  return;
}

