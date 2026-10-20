
// block 0044b780
0044b780  mov        ecx, dword ptr [eax - 4]
0044b783  push       esi
0044b784  lea        esi, [eax - 4]
0044b787  push       0x44bc50
0044b78c  push       ecx
0044b78d  push       0x10
0044b78f  push       eax
0044b790  call       0x46cf1e

// block 0044b795
0044b795  push       esi
0044b796  call       0x46ca4f

// block 0044b79b
0044b79b  add        esp, 4
0044b79e  mov        eax, esi
0044b7a0  pop        esi
0044b7a1  ret        
