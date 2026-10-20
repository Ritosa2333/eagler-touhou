
// block 0042f470
0042f470  cmp        dword ptr [0x4cea94], 0
0042f477  je         0x42f4a6

// block 0042f479
0042f479  mov        eax, dword ptr [0x4ce8cc]
0042f47e  push       esi
0042f47f  push       eax
0042f480  mov        eax, dword ptr [0x4ceaac]
0042f485  call       0x45c900

// block 0042f48a
0042f48a  mov        ecx, dword ptr [0x4ceaac]
0042f490  mov        esi, dword ptr [0x4ce8cc]
0042f496  mov        dword ptr [ecx + 0x3bc], 0xffffffff
0042f4a0  call       0x45a3c0

// block 0042f4a5
0042f4a5  pop        esi

// block 0042f4a6
0042f4a6  mov        eax, 1
0042f4ab  ret        
