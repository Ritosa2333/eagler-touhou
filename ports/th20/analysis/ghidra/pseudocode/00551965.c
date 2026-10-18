/* Entry: 0x00551965; symbol: __lock_file; body bytes: 20 */

/* Library Function - Single Match
    __lock_file
   
   Library: Visual Studio 2019 Release */

void __cdecl __lock_file(FILE *_File)

{
  EnterCriticalSection((LPCRITICAL_SECTION)(_File + 1));
  return;
}

