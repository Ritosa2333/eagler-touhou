
// block 004014b0
004014b0  push       esi
004014b1  mov        esi, ecx
004014b3  push       esi
004014b4  call       0x4022a0

// block 004014b9
004014b9  mov        eax, 1
004014be  add        dword ptr [esi + 0x18fb0], eax
004014c4  pop        esi
004014c5  ret        
