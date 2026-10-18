/* Entry: 0x00411860; symbol: _wmemset; body bytes: 55 */

/* Library Function - Single Match
    _wmemset
   
   Library: Visual Studio */

wchar_t * __cdecl _wmemset(wchar_t *_S,wchar_t _C,size_t _N)

{
  wchar_t *local_8;
  
  local_8 = _S;
  for (; _N != 0; _N = _N - 1) {
    *local_8 = _C;
    local_8 = local_8 + 1;
  }
  return _S;
}

