
// block 00466310
00466310  cmp        dword ptr [eax + 4], 0
00466314  jne        0x46631c

// block 00466316
00466316  mov        eax, 0x800401f0
0046631b  ret        

// block 0046631c
0046631c  cmp        edx, dword ptr [eax + 0x10]
0046631f  jb         0x466327

// block 00466321
00466321  mov        eax, 0x80070057
00466326  ret        

// block 00466327
00466327  mov        dword ptr [ecx], 0
0046632d  mov        eax, dword ptr [eax + 4]
00466330  mov        eax, dword ptr [eax + edx*4]
00466333  mov        edx, dword ptr [eax]
00466335  push       ecx
00466336  push       0x4997dc
