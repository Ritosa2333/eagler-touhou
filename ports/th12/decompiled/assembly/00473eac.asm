
// block 00473eac
00473eac  mov        edi, edi
00473eae  push       ebp
00473eaf  mov        ebp, esp
00473eb1  push       ebx
00473eb2  push       esi
00473eb3  mov        esi, dword ptr [ebp + 8]
00473eb6  mov        eax, dword ptr [esi + 0xbc]
00473ebc  xor        ebx, ebx
00473ebe  push       edi
00473ebf  cmp        eax, ebx
00473ec1  je         0x473f32

// block 00473ec3
00473ec3  cmp        eax, 0x4adf68
00473ec8  je         0x473f32

// block 00473eca
00473eca  mov        eax, dword ptr [esi + 0xb0]
00473ed0  cmp        eax, ebx
00473ed2  je         0x473f32

// block 00473ed4
00473ed4  cmp        dword ptr [eax], ebx
00473ed6  jne        0x473f32

// block 00473ed8
00473ed8  mov        eax, dword ptr [esi + 0xb8]
00473ede  cmp        eax, ebx
00473ee0  je         0x473ef9

// block 00473ee2
00473ee2  cmp        dword ptr [eax], ebx
00473ee4  jne        0x473ef9

// block 00473ee6
00473ee6  push       eax
00473ee7  call       0x46c941

// block 00473eec
00473eec  push       dword ptr [esi + 0xbc]
00473ef2  call       0x48415b

// block 00473ef7
00473ef7  pop        ecx
00473ef8  pop        ecx

// block 00473ef9
00473ef9  mov        eax, dword ptr [esi + 0xb4]
00473eff  cmp        eax, ebx
00473f01  je         0x473f1a

// block 00473f03
00473f03  cmp        dword ptr [eax], ebx
00473f05  jne        0x473f1a

// block 00473f07
00473f07  push       eax
00473f08  call       0x46c941

// block 00473f0d
00473f0d  push       dword ptr [esi + 0xbc]
00473f13  call       0x483f19

// block 00473f18
00473f18  pop        ecx
00473f19  pop        ecx

// block 00473f1a
00473f1a  push       dword ptr [esi + 0xb0]
00473f20  call       0x46c941

// block 00473f25
00473f25  push       dword ptr [esi + 0xbc]
00473f2b  call       0x46c941

// block 00473f30
00473f30  pop        ecx
00473f31  pop        ecx

// block 00473f32
00473f32  mov        eax, dword ptr [esi + 0xc0]
00473f38  cmp        eax, ebx
00473f3a  je         0x473f80

// block 00473f3c
00473f3c  cmp        dword ptr [eax], ebx
00473f3e  jne        0x473f80

// block 00473f40
00473f40  mov        eax, dword ptr [esi + 0xc4]
00473f46  sub        eax, 0xfe
00473f4b  push       eax
00473f4c  call       0x46c941

// block 00473f51
00473f51  mov        eax, dword ptr [esi + 0xcc]
00473f57  mov        edi, 0x80
00473f5c  sub        eax, edi
00473f5e  push       eax
00473f5f  call       0x46c941

// block 00473f64
00473f64  mov        eax, dword ptr [esi + 0xd0]
00473f6a  sub        eax, edi
00473f6c  push       eax
00473f6d  call       0x46c941

// block 00473f72
00473f72  push       dword ptr [esi + 0xc0]
00473f78  call       0x46c941

// block 00473f7d
00473f7d  add        esp, 0x10

// block 00473f80
00473f80  lea        edi, [esi + 0xd4]
00473f86  mov        eax, dword ptr [edi]
00473f88  cmp        eax, 0x4adea8
00473f8d  je         0x473fa6

// block 00473f8f
00473f8f  cmp        dword ptr [eax + 0xb4], ebx
00473f95  jne        0x473fa6

// block 00473f97
00473f97  push       eax
00473f98  call       0x483cd8

// block 00473f9d
00473f9d  push       dword ptr [edi]
00473f9f  call       0x46c941

// block 00473fa4
00473fa4  pop        ecx
00473fa5  pop        ecx

// block 00473fa6
00473fa6  lea        edi, [esi + 0x50]
00473fa9  mov        dword ptr [ebp + 8], 6

// block 00473fb0
00473fb0  cmp        dword ptr [edi - 8], 0x4ad9d8
00473fb7  je         0x473fca

// block 00473fb9
00473fb9  mov        eax, dword ptr [edi]
00473fbb  cmp        eax, ebx
00473fbd  je         0x473fca

// block 00473fbf
00473fbf  cmp        dword ptr [eax], ebx
00473fc1  jne        0x473fca

// block 00473fc3
00473fc3  push       eax
00473fc4  call       0x46c941

// block 00473fc9
00473fc9  pop        ecx

// block 00473fca
00473fca  cmp        dword ptr [edi - 4], ebx
00473fcd  je         0x473fe1

// block 00473fcf
00473fcf  mov        eax, dword ptr [edi + 4]
00473fd2  cmp        eax, ebx
00473fd4  je         0x473fe1

// block 00473fd6
00473fd6  cmp        dword ptr [eax], ebx
00473fd8  jne        0x473fe1

// block 00473fda
00473fda  push       eax
00473fdb  call       0x46c941

// block 00473fe0
00473fe0  pop        ecx

// block 00473fe1
00473fe1  add        edi, 0x10
00473fe4  dec        dword ptr [ebp + 8]
00473fe7  jne        0x473fb0

// block 00473fe9
00473fe9  push       esi
00473fea  call       0x46c941

// block 00473fef
00473fef  pop        ecx
00473ff0  pop        edi
00473ff1  pop        esi
00473ff2  pop        ebx
00473ff3  pop        ebp
00473ff4  ret        
