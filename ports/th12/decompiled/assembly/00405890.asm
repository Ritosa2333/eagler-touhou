
// block 00405890
00405890  mov        eax, dword ptr [0x4b43c0]
00405895  mov        ecx, dword ptr [eax + 0x35d0]
0040589b  test       cl, 1
0040589e  jne        0x4058cf

// block 004058a0
004058a0  fldz       
004058a2  or         ecx, 1
004058a5  fstp       dword ptr [eax + 0x35c8]
004058ab  mov        dword ptr [eax + 0x35c4], 0
004058b5  mov        dword ptr [eax + 0x35c0], 0xfff0bdc1
004058bf  mov        dword ptr [eax + 0x35cc], 0x4b2ed0
004058c9  mov        dword ptr [eax + 0x35d0], ecx

// block 004058cf
004058cf  fld        dword ptr [0x4a4040]
004058d5  mov        dword ptr [eax + 0x35c4], 0x3c
004058df  fstp       dword ptr [eax + 0x35c8]
004058e5  mov        dword ptr [eax + 0x35c0], 0x3b
004058ef  or         dword ptr [eax + 0x35bc], 4
004058f6  ret        
