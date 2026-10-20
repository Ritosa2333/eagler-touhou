
// block 004060d0
004060d0  fld        dword ptr [eax]
004060d2  mov        ecx, esi
004060d4  fld        dword ptr [esp + 4]
004060d8  fld        st(0)
004060da  fmulp      st(2)
004060dc  fxch       st(1)
004060de  fstp       dword ptr [esi]
004060e0  fld        dword ptr [eax + 4]
004060e3  fmul       st(1)
004060e5  fstp       dword ptr [esi + 4]
004060e8  fld        dword ptr [eax + 8]
004060eb  fmul       st(1)
004060ed  fstp       dword ptr [esi + 8]
004060f0  fld        dword ptr [eax + 0xc]
004060f3  fmul       st(1)
004060f5  fstp       dword ptr [esi + 0xc]
004060f8  fld        dword ptr [eax + 0x10]
004060fb  fmul       st(1)
004060fd  fstp       dword ptr [esi + 0x10]
00406100  fmul       dword ptr [eax + 0x14]
00406103  fstp       dword ptr [esi + 0x14]
00406106  call       0x406250

// block 0040610b
0040610b  mov        eax, esi
0040610d  ret        4
