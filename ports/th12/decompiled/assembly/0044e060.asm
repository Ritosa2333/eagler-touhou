
// block 0044e060
0044e060  sub        esp, 0x40
0044e063  push       ebx
0044e064  xor        ebx, ebx
0044e066  cmp        dword ptr [esi + 0x11c], ebx
0044e06c  jne        0x44e077

// block 0044e06e
0044e06e  xor        al, al
0044e070  pop        ebx
0044e071  add        esp, 0x40
0044e074  ret        4

// block 0044e077
0044e077  push       ebp
0044e078  push       edi
0044e079  mov        edi, dword ptr [esp + 0x50]
0044e07d  mov        eax, dword ptr [edi]
0044e07f  mov        edx, dword ptr [eax + 0x30]
0044e082  lea        ecx, [esp + 0x2c]
0044e086  push       ecx
0044e087  push       edi
0044e088  call       edx

// block 0044e08a
0044e08a  mov        eax, dword ptr [esi + 0x104]
0044e090  mov        ecx, dword ptr [esi + 0x108]
0044e096  mov        edx, dword ptr [edi]
0044e098  mov        edx, dword ptr [edx + 0x34]
0044e09b  push       0x10
0044e09d  mov        dword ptr [esp + 0x28], eax
0044e0a1  lea        eax, [esp + 0x20]
0044e0a5  mov        dword ptr [esp + 0x2c], ecx
0044e0a9  push       eax
0044e0aa  lea        ecx, [esp + 0x1c]
0044e0ae  push       ecx
0044e0af  push       edi
0044e0b0  mov        dword ptr [esp + 0x2c], ebx
0044e0b4  mov        dword ptr [esp + 0x30], ebx
0044e0b8  call       edx

// block 0044e0ba
0044e0ba  mov        ecx, dword ptr [esp + 0x2c]
0044e0be  mov        eax, dword ptr [esp + 0x14]
0044e0c2  mov        ebp, dword ptr [esi + 0x110]
0044e0c8  mov        ebx, dword ptr [esi + 0x120]
0044e0ce  mov        edi, dword ptr [esp + 0x18]
0044e0d2  mov        dword ptr [esp + 0x10], eax
0044e0d6  cmp        ecx, dword ptr [esi + 0x100]
0044e0dc  jne        0x44e112

// block 0044e0de
0044e0de  cmp        dword ptr [esi + 0x108], 0
0044e0e5  mov        dword ptr [esp + 0xc], 0
0044e0ed  jle        0x44e112

// block 0044e0ef
0044e0ef  nop        

// block 0044e0f0
0044e0f0  push       ebp
0044e0f1  push       edi
0044e0f2  push       ebx
0044e0f3  call       0x47a330

// block 0044e0f8
0044e0f8  mov        eax, dword ptr [esp + 0x18]
0044e0fc  add        edi, dword ptr [esp + 0x1c]
0044e100  inc        eax
0044e101  add        esp, 0xc
0044e104  add        ebx, ebp
0044e106  cmp        eax, dword ptr [esi + 0x108]
0044e10c  mov        dword ptr [esp + 0xc], eax
0044e110  jl         0x44e0f0

// block 0044e112
0044e112  mov        eax, dword ptr [esp + 0x50]
0044e116  mov        edx, dword ptr [eax]
0044e118  push       eax
0044e119  mov        eax, dword ptr [edx + 0x38]
0044e11c  call       eax

// block 0044e11e
0044e11e  pop        edi
0044e11f  pop        ebp
0044e120  mov        al, 1
0044e122  pop        ebx
0044e123  add        esp, 0x40
0044e126  ret        4
