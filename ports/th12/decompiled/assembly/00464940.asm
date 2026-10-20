
// block 00464940
00464940  add        dword ptr [eax + 0x8c], -1
00464947  mov        edx, 0
0046494c  jns        0x464954

// block 0046494e
0046494e  mov        dword ptr [eax + 0x8c], edx

// block 00464954
00464954  mov        ecx, dword ptr [eax + 0x8c]
0046495a  push       esi
0046495b  mov        esi, dword ptr [eax + ecx*4 + 0xc]
0046495f  mov        dword ptr [eax], esi
00464961  mov        ecx, dword ptr [eax + ecx*4 + 0x4c]
00464965  mov        dword ptr [eax + 8], ecx
00464968  mov        dword ptr [eax + 0xd4], edx
0046496e  pop        esi
0046496f  ret        
