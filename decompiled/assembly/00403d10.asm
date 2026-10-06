
// block 00403d10
00403d10  or         al, 0x37
00403d12  add        byte ptr [eax], al
00403d14  call       0x45a3c0

// block 00403d19
00403d19  mov        eax, dword ptr [0x4ce8f0]
00403d1e  mov        ecx, dword ptr [eax]
00403d20  mov        edx, dword ptr [ecx + 0xe4]
00403d26  push       edi
00403d27  push       0x25
00403d29  push       eax
00403d2a  call       edx
