
// block 00491b69
00491b69  mov        edi, edi
00491b6b  push       ebp
00491b6c  mov        ebp, esp
00491b6e  sub        esp, 0x38
00491b71  push       ebx
00491b72  cmp        dword ptr [ebp + 8], 0x123
00491b79  jne        0x491b8d

// block 00491b7b
00491b7b  mov        eax, 0x491c14
00491b80  mov        ecx, dword ptr [ebp + 0xc]
00491b83  mov        dword ptr [ecx], eax
00491b85  xor        eax, eax
00491b87  inc        eax
00491b88  jmp        0x491c3d

// block 00491b8d
00491b8d  and        dword ptr [ebp - 0x28], 0
00491b91  mov        dword ptr [ebp - 0x24], 0x491c40
00491b98  mov        eax, dword ptr [0x4ad138]
00491b9d  lea        ecx, [ebp - 0x28]
00491ba0  xor        eax, ecx
00491ba2  mov        dword ptr [ebp - 0x20], eax
00491ba5  mov        eax, dword ptr [ebp + 0x18]
00491ba8  mov        dword ptr [ebp - 0x1c], eax
00491bab  mov        eax, dword ptr [ebp + 0xc]
00491bae  mov        dword ptr [ebp - 0x18], eax
00491bb1  mov        eax, dword ptr [ebp + 0x1c]
00491bb4  mov        dword ptr [ebp - 0x14], eax
00491bb7  mov        eax, dword ptr [ebp + 0x20]
00491bba  mov        dword ptr [ebp - 0x10], eax
00491bbd  and        dword ptr [ebp - 0xc], 0
00491bc1  and        dword ptr [ebp - 8], 0
00491bc5  and        dword ptr [ebp - 4], 0
00491bc9  mov        dword ptr [ebp - 0xc], esp
00491bcc  mov        dword ptr [ebp - 8], ebp
00491bcf  mov        eax, dword ptr fs:[0]
00491bd5  mov        dword ptr [ebp - 0x28], eax
00491bd8  lea        eax, [ebp - 0x28]
00491bdb  mov        dword ptr fs:[0], eax
00491be1  mov        dword ptr [ebp - 0x38], 1
00491be8  mov        eax, dword ptr [ebp + 8]
00491beb  mov        dword ptr [ebp - 0x34], eax
00491bee  mov        eax, dword ptr [ebp + 0x10]
00491bf1  mov        dword ptr [ebp - 0x30], eax
00491bf4  call       0x475467

// block 00491bf9
00491bf9  mov        eax, dword ptr [eax + 0x80]
00491bff  mov        dword ptr [ebp - 0x2c], eax
00491c02  lea        eax, [ebp - 0x34]
00491c05  push       eax
00491c06  mov        eax, dword ptr [ebp + 8]
00491c09  push       dword ptr [eax]
00491c0b  call       dword ptr [ebp - 0x2c]

// block 00491c0e
00491c0e  pop        ecx
00491c0f  pop        ecx
00491c10  and        dword ptr [ebp - 0x38], 0
00491c14  cmp        dword ptr [ebp - 4], 0
00491c18  je         0x491c31

// block 00491c1a
00491c1a  mov        ebx, dword ptr fs:[0]
00491c21  mov        eax, dword ptr [ebx]
00491c23  mov        ebx, dword ptr [ebp - 0x28]
00491c26  mov        dword ptr [ebx], eax
00491c28  mov        dword ptr fs:[0], ebx
00491c2f  jmp        0x491c3a

// block 00491c31
00491c31  mov        eax, dword ptr [ebp - 0x28]
00491c34  mov        dword ptr fs:[0], eax

// block 00491c3a
00491c3a  mov        eax, dword ptr [ebp - 0x38]

// block 00491c3d
00491c3d  pop        ebx
00491c3e  leave      
00491c3f  ret        
