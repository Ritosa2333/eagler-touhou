
// block 0043f0f0
0043f0f0  push       ebx
0043f0f1  push       esi
0043f0f2  mov        ebx, edx
0043f0f4  lea        esi, [eax + ecx*4 + 0x2c8]
0043f0fb  call       0x462060

// block 0043f100
0043f100  pop        esi
0043f101  mov        eax, ebx
0043f103  pop        ebx
0043f104  ret        
