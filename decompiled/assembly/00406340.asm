
// block 00406340
00406340  mov        ecx, dword ptr [eax + 0x40]
00406343  fldz       
00406345  xor        edx, edx
00406347  test       cl, 1
0040634a  jne        0x406366

// block 0040634c
0040634c  or         ecx, 1
0040634f  fst        dword ptr [eax + 0x38]
00406352  mov        dword ptr [eax + 0x34], edx
00406355  mov        dword ptr [eax + 0x30], 0xfff0bdc1
0040635c  mov        dword ptr [eax + 0x3c], 0x4b2ed0
00406363  mov        dword ptr [eax + 0x40], ecx

// block 00406366
00406366  fstp       dword ptr [eax + 0x38]
00406369  mov        dword ptr [eax + 0x34], edx
0040636c  mov        dword ptr [eax + 0x30], 0xffffffff
00406373  ret        
