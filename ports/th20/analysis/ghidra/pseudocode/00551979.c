/* Entry: 0x00551979; symbol: __unlock_file; body bytes: 20 */

/* Library Function - Single Match
    __unlock_file
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl __unlock_file(FILE *_File)

{
  LeaveCriticalSection((LPCRITICAL_SECTION)(_File + 1));
  return;
}

