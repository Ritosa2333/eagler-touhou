
// block 004669f0
004669f0  push       ecx
004669f1  mov        eax, dword ptr [edi + 4]
004669f4  push       ebp
004669f5  xor        ebp, ebp
004669f7  cmp        dword ptr [eax], ebp
004669f9  je         0x466ac7

// block 004669ff
004669ff  cmp        dword ptr [edi + 0xc], ebp
00466a02  je         0x466ac7

// block 00466a08
00466a08  push       ebx
00466a09  push       esi
00466a0a  mov        dword ptr [edi + 0x60], ebp
00466a0d  mov        dword ptr [edi + 0x64], ebp
00466a10  mov        dword ptr [edi + 0x68], ebp
00466a13  mov        dword ptr [edi + 0x6c], ebp
00466a16  mov        esi, dword ptr [eax]
00466a18  cmp        esi, ebp
00466a1a  jne        0x466a26

// block 00466a1c
00466a1c  mov        eax, 0x800401f0

// block 00466a21
00466a21  pop        esi
00466a22  pop        ebx
00466a23  pop        ebp
00466a24  pop        ecx
00466a25  ret        

// block 00466a26
00466a26  lea        ebx, [esp + 0xc]
00466a2a  call       0x466220

// block 00466a2f
00466a2f  cmp        eax, ebp
00466a31  jl         0x466a21

// block 00466a33
00466a33  cmp        dword ptr [esp + 0xc], ebp
00466a37  je         0x466a4b

// block 00466a39
00466a39  mov        eax, dword ptr [edi + 4]
00466a3c  mov        ecx, dword ptr [eax]
00466a3e  push       ebp
00466a3f  push       ecx
00466a40  mov        ebx, edi
00466a42  call       0x465fe0

// block 00466a47
00466a47  cmp        eax, ebp
00466a49  jl         0x466a21

// block 00466a4b
00466a4b  mov        esi, dword ptr [edi + 0xc]
00466a4e  cmp        dword ptr [esi + 0x7c], ebp
00466a51  je         0x466a85

// block 00466a53
00466a53  mov        edx, dword ptr [esi + 0x80]
00466a59  mov        eax, dword ptr [esi + 0x90]
00466a5f  mov        dword ptr [esi + 0x84], edx
00466a65  mov        eax, dword ptr [eax + 0x1c]
00466a68  cmp        eax, ebp
00466a6a  jle        0x466ab4

// block 00466a6c
00466a6c  mov        dword ptr [esi + 0x88], eax
00466a72  mov        edx, dword ptr [edi + 4]
00466a75  mov        eax, dword ptr [edx]
00466a77  mov        ecx, dword ptr [eax]
00466a79  mov        edx, dword ptr [ecx + 0x34]
00466a7c  push       ebp
00466a7d  push       eax
00466a7e  call       edx

// block 00466a80
00466a80  pop        esi
00466a81  pop        ebx
00466a82  pop        ebp
00466a83  pop        ecx
00466a84  ret        

// block 00466a85
00466a85  mov        eax, dword ptr [esi + 0x8c]
00466a8b  cmp        eax, ebp
00466a8d  je         0x466ab4

// block 00466a8f
00466a8f  mov        ecx, dword ptr [esi + 0x90]
00466a95  mov        edx, dword ptr [ecx + 0x10]
00466a98  add        edx, dword ptr [0x4d4760]
00466a9e  push       ebp
00466a9f  push       ebp
00466aa0  push       edx
00466aa1  push       eax
00466aa2  call       dword ptr [0x4980a0]

// block 00466aa8
00466aa8  mov        eax, dword ptr [esi + 0x90]
00466aae  mov        ecx, dword ptr [eax + 0x1c]
00466ab1  mov        dword ptr [esi + 8], ecx

// block 00466ab4
00466ab4  mov        edx, dword ptr [edi + 4]
00466ab7  mov        eax, dword ptr [edx]
00466ab9  mov        ecx, dword ptr [eax]
00466abb  mov        edx, dword ptr [ecx + 0x34]
00466abe  push       ebp
00466abf  push       eax
00466ac0  call       edx

// block 00466ac2
00466ac2  pop        esi
00466ac3  pop        ebx
00466ac4  pop        ebp
00466ac5  pop        ecx
00466ac6  ret        

// block 00466ac7
00466ac7  mov        eax, 0x800401f0
00466acc  pop        ebp
00466acd  pop        ecx
00466ace  ret        
