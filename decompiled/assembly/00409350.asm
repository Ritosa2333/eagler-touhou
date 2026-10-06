
// block 00409350
00409350  fldz       
00409352  xor        ecx, ecx
00409354  mov        word ptr [eax + 0x532], cx
0040935b  mov        ecx, dword ptr [eax + 0x4f4]
00409361  xor        edx, edx
00409363  test       cl, 1
00409366  jne        0x409391

// block 00409368
00409368  or         ecx, 1
0040936b  fst        dword ptr [eax + 0x4ec]
00409371  mov        dword ptr [eax + 0x4e8], edx
00409377  mov        dword ptr [eax + 0x4e4], 0xfff0bdc1
00409381  mov        dword ptr [eax + 0x4f0], 0x4b2ed0
0040938b  mov        dword ptr [eax + 0x4f4], ecx

// block 00409391
00409391  fst        dword ptr [eax + 0x4ec]
00409397  mov        dword ptr [eax + 0x4e8], edx
0040939d  mov        dword ptr [eax + 0x4e4], 0xffffffff
004093a7  mov        ecx, dword ptr [eax + 0x508]
004093ad  test       cl, 1
004093b0  jne        0x4093db

// block 004093b2
004093b2  or         ecx, 1
004093b5  fst        dword ptr [eax + 0x500]
004093bb  mov        dword ptr [eax + 0x4fc], edx
004093c1  mov        dword ptr [eax + 0x4f8], 0xfff0bdc1
004093cb  mov        dword ptr [eax + 0x504], 0x4b2ed0
004093d5  mov        dword ptr [eax + 0x508], ecx

// block 004093db
004093db  fstp       dword ptr [eax + 0x500]
004093e1  mov        dword ptr [eax + 0x4fc], edx
004093e7  mov        dword ptr [eax + 0x4f8], 0xffffffff
004093f1  ret        
