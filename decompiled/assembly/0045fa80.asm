
// block 0045fa80
0045fa80  sub        esp, 0x1c
0045fa83  test       byte ptr [0x4ceae8], 1
0045fa8a  push       ebx
0045fa8b  push       esi
0045fa8c  mov        ebx, eax
0045fa8e  mov        esi, ecx
0045fa90  mov        dword ptr [esp + 0xc], 0
0045fa98  je         0x45fab9

// block 0045fa9a
0045fa9a  mov        eax, dword ptr [ebx*4 + 0x4a2338]
0045faa1  cmp        eax, 0x15
0045faa4  je         0x45fab4

// block 0045faa6
0045faa6  test       eax, eax
0045faa8  je         0x45fab4

// block 0045faaa
0045faaa  cmp        eax, 0x14
0045faad  jne        0x45fab9

// block 0045faaf
0045faaf  lea        ebx, [eax - 0x11]
0045fab2  jmp        0x45fab9

// block 0045fab4
0045fab4  mov        ebx, 5

// block 0045fab9
0045fab9  and        dword ptr [edi + 0x10], 0xfffffffe
0045fabd  mov        edx, dword ptr [ebx*4 + 0x4a2338]
0045fac4  movsx      eax, word ptr [esi + 8]
0045fac8  movsx      ecx, word ptr [esi + 0xa]
0045facc  push       edi
0045facd  push       1
0045facf  push       edx
0045fad0  mov        edx, dword ptr [0x4ce8f0]
0045fad6  push       0
0045fad8  push       1
0045fada  mov        dword ptr [esp + 0x2c], eax
0045fade  mov        eax, dword ptr [esp + 0x40]
0045fae2  mov        dword ptr [esp + 0x30], ecx
0045fae6  mov        ecx, dword ptr [esp + 0x3c]
0045faea  push       eax
0045faeb  push       ecx
0045faec  push       edx
0045faed  mov        dword ptr [esp + 0x30], 0
0045faf5  mov        dword ptr [esp + 0x34], 0
0045fafd  call       0x46c6e6

// block 0045fb02
0045fb02  test       eax, eax
0045fb04  je         0x45fb21

// block 0045fb06
0045fb06  mov        eax, dword ptr [esp + 0xc]
0045fb0a  test       eax, eax
0045fb0c  je         0x45fb16

// block 0045fb0e
0045fb0e  mov        ecx, dword ptr [eax]
0045fb10  mov        edx, dword ptr [ecx + 8]
0045fb13  push       eax
0045fb14  call       edx

// block 0045fb16
0045fb16  or         eax, 0xffffffff
0045fb19  pop        esi
0045fb1a  pop        ebx
0045fb1b  add        esp, 0x1c
0045fb1e  ret        8

// block 0045fb21
0045fb21  mov        eax, dword ptr [edi]
0045fb23  mov        ecx, dword ptr [eax]
0045fb25  lea        edx, [esp + 0xc]
0045fb29  push       edx
0045fb2a  push       0
0045fb2c  push       eax
0045fb2d  mov        eax, dword ptr [ecx + 0x48]
0045fb30  call       eax

// block 0045fb32
0045fb32  movsx      eax, word ptr [esi + 6]
0045fb36  movsx      edx, word ptr [esi + 8]
0045fb3a  add        eax, eax
0045fb3c  imul       edx, dword ptr [eax + eax + 0x4a235c]
0045fb44  push       0
0045fb46  add        eax, eax
0045fb48  mov        eax, dword ptr [eax + 0x4a2338]
0045fb4e  push       1
0045fb50  lea        ecx, [esp + 0x18]
0045fb54  push       ecx
0045fb55  push       0
0045fb57  push       edx
0045fb58  mov        edx, dword ptr [esp + 0x20]
0045fb5c  push       eax
0045fb5d  add        esi, 0x10
0045fb60  push       esi
0045fb61  push       ecx
0045fb62  push       0
0045fb64  push       edx
0045fb65  call       0x46c6ec

// block 0045fb6a
0045fb6a  mov        eax, dword ptr [ebx*4 + 0x4a235c]
0045fb71  lea        ebx, [ebx*4 + 0x4a235c]
0045fb78  mov        dword ptr [edi + 0xc], eax
0045fb7b  mov        eax, dword ptr [esp + 0xc]
0045fb7f  test       eax, eax
0045fb81  je         0x45fb8b

// block 0045fb83
0045fb83  mov        ecx, dword ptr [eax]
0045fb85  mov        edx, dword ptr [ecx + 8]
0045fb88  push       eax
0045fb89  call       edx

// block 0045fb8b
0045fb8b  mov        eax, dword ptr [ebx]
0045fb8d  imul       eax, dword ptr [esp + 0x28]
0045fb92  imul       eax, dword ptr [esp + 0x2c]
0045fb97  pop        esi
0045fb98  pop        ebx
0045fb99  add        esp, 0x1c
0045fb9c  ret        8
