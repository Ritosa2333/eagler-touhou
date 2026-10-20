
// block 00460e98
00460e98  push       edx
00460e99  push       eax
00460e9a  mov        eax, dword ptr [ecx + 0xbc]
00460ea0  call       eax

// block 00460ea2
00460ea2  mov        esi, dword ptr [0x4ce8cc]
00460ea8  mov        dword ptr [0x4cee38], 1
00460eb2  call       0x45a3c0

// block 00460eb7
00460eb7  mov        eax, dword ptr [0x4ce8f0]
00460ebc  mov        ecx, dword ptr [eax]
00460ebe  mov        edx, dword ptr [ecx + 0xe4]
00460ec4  push       0
00460ec6  push       0xe
00460ec8  push       eax
00460ec9  call       edx

// block 00460ecb
00460ecb  mov        esi, dword ptr [0x4ce8cc]
00460ed1  call       0x45a3c0

// block 00460ed6
00460ed6  mov        eax, dword ptr [0x4ce8f0]
00460edb  mov        ecx, dword ptr [eax]
00460edd  mov        edx, dword ptr [ecx + 0xe4]
00460ee3  push       8
00460ee5  push       0x17
00460ee7  push       eax
00460ee8  call       edx

// block 00460eea
00460eea  mov        eax, 0x15
00460eef  mov        edi, ebx
00460ef1  call       0x4611d0

// block 00460ef6
00460ef6  pop        edi
00460ef7  pop        esi
00460ef8  pop        ebx
00460ef9  ret        
