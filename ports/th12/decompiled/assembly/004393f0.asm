
// block 004393f0
004393f0  fld        dword ptr [esi]
004393f2  fld        qword ptr [0x4a3d48]
004393f8  fmul       st(1), st(0)
004393fa  fxch       st(1)
004393fc  call       0x4931e0

// block 00439401
00439401  mov        dword ptr [edi], eax
00439403  fmul       dword ptr [esi + 4]
00439406  call       0x4931e0

// block 0043940b
0043940b  mov        dword ptr [edi + 4], eax
0043940e  ret        
