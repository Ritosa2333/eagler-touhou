
// block 0043efd0
0043efd0  mov        eax, dword ptr [edi + esi*4 + 0x2c8]
0043efd7  push       ebx
0043efd8  push       eax
0043efd9  mov        ebx, 1
0043efde  call       0x461970

// block 0043efe3
0043efe3  mov        dword ptr [edi + esi*4 + 0x2c8], 0
0043efee  pop        ebx
0043efef  ret        
