
// block 0040d9c0
0040d9c0  push       ebx
0040d9c1  push       ebp
0040d9c2  push       esi
0040d9c3  mov        esi, eax
0040d9c5  mov        eax, dword ptr [esi + 0x14]
0040d9c8  push       edi
0040d9c9  push       eax
0040d9ca  call       0x461a70

// block 0040d9cf
0040d9cf  xor        ebp, ebp
0040d9d1  mov        dword ptr [esi + 0x14], ebp
0040d9d4  mov        ecx, dword ptr [esi + 0x18]
0040d9d7  push       ecx
0040d9d8  call       0x461a70

// block 0040d9dd
0040d9dd  mov        dword ptr [esi + 0x18], ebp
0040d9e0  mov        edx, dword ptr [esi + 0x1c]
0040d9e3  push       edx
0040d9e4  call       0x461a70

// block 0040d9e9
0040d9e9  mov        ebx, dword ptr [0x4ce89c]
0040d9ef  mov        dword ptr [esi + 0x1c], ebp
0040d9f2  mov        edi, dword ptr [esi + 8]
0040d9f5  cmp        edi, ebp
0040d9f7  je         0x40da3f

// block 0040d9f9
0040d9f9  test       dword ptr [0x4cee78], 0x8000
0040da03  je         0x40da16

// block 0040da05
0040da05  push       0x4cf0f8
0040da0a  call       dword ptr [0x498088]

// block 0040da10
0040da10  inc        byte ptr [0x4cf218]

// block 0040da16
0040da16  mov        ecx, edi
0040da18  mov        edx, ebx
0040da1a  call       0x462890

// block 0040da1f
0040da1f  mov        ebx, 0x8000
0040da24  test       dword ptr [0x4cee78], ebx
0040da2a  je         0x40da44

// block 0040da2c
0040da2c  push       0x4cf0f8
0040da31  call       dword ptr [0x49808c]

// block 0040da37
0040da37  dec        byte ptr [0x4cf218]
0040da3d  jmp        0x40da44

// block 0040da3f
0040da3f  mov        ebx, 0x8000

// block 0040da44
0040da44  mov        esi, dword ptr [esi + 0xc]
0040da47  mov        edi, dword ptr [0x4ce89c]
0040da4d  cmp        esi, ebp
0040da4f  je         0x40da8c

// block 0040da51
0040da51  test       dword ptr [0x4cee78], ebx
0040da57  je         0x40da6a

// block 0040da59
0040da59  push       0x4cf0f8
0040da5e  call       dword ptr [0x498088]

// block 0040da64
0040da64  inc        byte ptr [0x4cf218]

// block 0040da6a
0040da6a  mov        ecx, esi
0040da6c  mov        edx, edi
0040da6e  call       0x462890

// block 0040da73
0040da73  test       dword ptr [0x4cee78], ebx
0040da79  je         0x40da8c

// block 0040da7b
0040da7b  push       0x4cf0f8
0040da80  call       dword ptr [0x49808c]

// block 0040da86
0040da86  dec        byte ptr [0x4cf218]

// block 0040da8c
0040da8c  pop        edi
0040da8d  pop        esi
0040da8e  mov        dword ptr [0x4b43cc], ebp
0040da94  pop        ebp
0040da95  pop        ebx
0040da96  ret        
