/* Entry: 0x005597f1; symbol: __fileno; body bytes: 39 */

/* Library Function - Single Match
    __fileno
   
   Library: Visual Studio */

int __cdecl __fileno(FILE *_File)

{
  __acrt_ptd *p_Var1;
  
  if (_File == (FILE *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return -1;
  }
  return _File->_file;
}

