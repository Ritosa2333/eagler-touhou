
// block 00405fd0
00405fd0  mov        ecx, dword ptr [eax + 0x80]
00405fd6  fldz       
00405fd8  xor        edx, edx
00405fda  test       cl, 1
00405fdd  jne        0x405ffc

// block 00405fdf
00405fdf  or         ecx, 1
00405fe2  fst        dword ptr [eax + 0x78]
00405fe5  mov        dword ptr [eax + 0x74], edx
00405fe8  mov        dword ptr [eax + 0x70], 0xfff0bdc1
00405fef  mov        dword ptr [eax + 0x7c], 0x4b2ed0
00405ff6  mov        dword ptr [eax + 0x80], ecx

// block 00405ffc
00405ffc  fstp       dword ptr [eax + 0x78]
00405fff  mov        dword ptr [eax + 0x74], edx
00406002  mov        dword ptr [eax + 0x70], 0xffffffff
00406009  ret        
