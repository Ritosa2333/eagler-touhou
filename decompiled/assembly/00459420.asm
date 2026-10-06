
// block 00459420
00459420  fild       dword ptr [edi]
00459422  fld        dword ptr [esp + 4]
00459426  fld        st(0)
00459428  fmulp      st(2)
0045942a  fxch       st(1)
0045942c  call       0x4931e0

// block 00459431
00459431  fild       dword ptr [edi + 4]
00459434  mov        dword ptr [esi], eax
00459436  fmul       st(1)
00459438  call       0x4931e0

// block 0045943d
0045943d  fimul      dword ptr [edi + 8]
00459440  mov        dword ptr [esi + 4], eax
00459443  call       0x4931e0

// block 00459448
00459448  mov        dword ptr [esi + 8], eax
0045944b  mov        eax, esi
0045944d  ret        4
