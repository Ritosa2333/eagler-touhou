
// block 0040fa30
0040fa30  push       ebx
0040fa31  push       ebp
0040fa32  push       esi
0040fa33  mov        esi, dword ptr [0x4ce8cc]
0040fa39  push       edi
0040fa3a  mov        edi, eax
0040fa3c  mov        edx, dword ptr [edi + 0x10]
0040fa3f  call       0x461be0

// block 0040fa44
0040fa44  mov        esi, dword ptr [edi + 8]
0040fa47  mov        ebx, dword ptr [0x4ce89c]
0040fa4d  mov        ebp, dword ptr [0x49808c]
0040fa53  test       esi, esi
0040fa55  je         0x40fa99

// block 0040fa57
0040fa57  test       dword ptr [0x4cee78], 0x8000
0040fa61  je         0x40fa74

// block 0040fa63
0040fa63  push       0x4cf0f8
0040fa68  call       dword ptr [0x498088]

// block 0040fa6e
0040fa6e  inc        byte ptr [0x4cf218]

// block 0040fa74
0040fa74  mov        ecx, esi
0040fa76  mov        edx, ebx
0040fa78  call       0x462890

// block 0040fa7d
0040fa7d  mov        ebx, 0x8000
0040fa82  test       dword ptr [0x4cee78], ebx
0040fa88  je         0x40fa9e

// block 0040fa8a
0040fa8a  push       0x4cf0f8
0040fa8f  call       ebp

// block 0040fa91
0040fa91  dec        byte ptr [0x4cf218]
0040fa97  jmp        0x40fa9e

// block 0040fa99
0040fa99  mov        ebx, 0x8000

// block 0040fa9e
0040fa9e  mov        edi, dword ptr [edi + 0xc]
0040faa1  mov        esi, dword ptr [0x4ce89c]
0040faa7  test       edi, edi
0040faa9  je         0x40fae2

// block 0040faab
0040faab  test       dword ptr [0x4cee78], ebx
0040fab1  je         0x40fac4

// block 0040fab3
0040fab3  push       0x4cf0f8
0040fab8  call       dword ptr [0x498088]

// block 0040fabe
0040fabe  inc        byte ptr [0x4cf218]

// block 0040fac4
0040fac4  mov        ecx, edi
0040fac6  mov        edx, esi
0040fac8  call       0x462890

// block 0040facd
0040facd  test       dword ptr [0x4cee78], ebx
0040fad3  je         0x40fae2

// block 0040fad5
0040fad5  push       0x4cf0f8
0040fada  call       ebp

// block 0040fadc
0040fadc  dec        byte ptr [0x4cf218]

// block 0040fae2
0040fae2  pop        edi
0040fae3  pop        esi
0040fae4  pop        ebp
0040fae5  mov        dword ptr [0x4b43d4], 0
0040faef  pop        ebx
0040faf0  ret        
