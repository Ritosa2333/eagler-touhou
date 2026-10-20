
// block 00464780
00464780  push       ebp
00464781  mov        ebp, esp
00464783  and        esp, 0xfffffff8
00464786  sub        esp, 8
00464789  fld        dword ptr [ebp + 8]
0046478c  call       0x4938c0

// block 00464791
00464791  fstp       dword ptr [esp]
00464794  fld        dword ptr [esp]
00464797  fstp       dword ptr [esp + 4]
0046479b  fld        dword ptr [ebp + 8]
0046479e  call       0x4939f0

// block 004647a3
004647a3  fstp       dword ptr [esp]
004647a6  fld        dword ptr [esp]
004647a9  fstp       dword ptr [esp]
004647ac  fld        dword ptr [esi]
004647ae  fld        dword ptr [esp]
004647b1  fld        st(0)
004647b3  fmulp      st(2)
004647b5  fld        dword ptr [esp + 4]
004647b9  fld        st(0)
004647bb  fmul       dword ptr [esi + 4]
004647be  fsubp      st(3)
004647c0  fxch       st(2)
004647c2  fstp       dword ptr [edi]
004647c4  fld        dword ptr [esi]
004647c6  fmulp      st(2)
004647c8  fmul       dword ptr [esi + 4]
004647cb  faddp      st(1)
004647cd  fstp       dword ptr [edi + 4]
004647d0  mov        esp, ebp
004647d2  pop        ebp
004647d3  ret        4
