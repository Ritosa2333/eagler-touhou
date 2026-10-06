
// block 0045fe60
0045fe60  mov        eax, dword ptr [0x4ce8cc]
0045fe65  cmp        dword ptr [eax + ecx*4 + 0x4b50c0], 0
0045fe6d  je         0x45fe85

// block 0045fe6f
0045fe6f  push       ebx
0045fe70  push       0x4a3384
0045fe75  call       0x4622e0

// block 0045fe7a
0045fe7a  mov        eax, dword ptr [eax + ecx*4 + 0x4b50c0]
0045fe81  add        esp, 8
0045fe84  ret        

// block 0045fe85
0045fe85  push       esi
0045fe86  push       ecx
0045fe87  push       eax
0045fe88  mov        ecx, ebx
0045fe8a  call       0x45fc90

// block 0045fe8f
0045fe8f  mov        esi, eax
0045fe91  test       esi, esi
0045fe93  jne        0x45fe97

// block 0045fe95
0045fe95  pop        esi
0045fe96  ret        

// block 0045fe97
0045fe97  push       edi
0045fe98  mov        edi, dword ptr [0x498084]
0045fe9e  mov        dword ptr [esi + 0x124], 1
0045fea8  jmp        0x45feb0

// block 0045feb0
0045feb0  test       dword ptr [0x4cee78], 0x180
0045feba  jne        0x45fec9

// block 0045febc
0045febc  push       1
0045febe  call       edi

// block 0045fec0
0045fec0  cmp        dword ptr [esi + 0x124], 0
0045fec7  jne        0x45feb0

// block 0045fec9
0045fec9  push       ebx
0045feca  push       0x4a33a0
0045fecf  call       0x4622e0

// block 0045fed4
0045fed4  add        esp, 8
0045fed7  pop        edi
0045fed8  mov        eax, esi
0045feda  pop        esi
0045fedb  ret        
