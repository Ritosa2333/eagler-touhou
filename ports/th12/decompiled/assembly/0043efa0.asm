
// block 0043efa0
0043efa0  push       ecx
0043efa1  mov        ecx, dword ptr [edi + 0x14]
0043efa4  push       0
0043efa6  push       esi
0043efa7  lea        eax, [esp + 8]
0043efab  push       eax
0043efac  push       ecx
0043efad  mov        eax, 0x17
0043efb2  xor        ecx, ecx
0043efb4  call       0x4615a0

// block 0043efb9
0043efb9  mov        edx, dword ptr [esp]
0043efbc  mov        dword ptr [edi + esi*4 + 0x2c8], edx
0043efc3  pop        ecx
0043efc4  ret        
