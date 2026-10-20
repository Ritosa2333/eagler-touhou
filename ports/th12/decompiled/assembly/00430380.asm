
// block 00430380
00430380  cmp        dword ptr [edi + 0x994], 0
00430387  je         0x4303b3

// block 00430389
00430389  push       esi
0043038a  mov        esi, dword ptr [0x4ce8cc]
00430390  call       0x45a3c0

// block 00430395
00430395  mov        eax, dword ptr [edi + 8]
00430398  push       0
0043039a  mov        dword ptr [edi + 0x994], 0
004303a4  mov        ecx, dword ptr [eax]
004303a6  mov        edx, dword ptr [ecx + 0xe4]
004303ac  push       0xe
004303ae  push       eax
004303af  call       edx

// block 004303b1
004303b1  pop        esi
004303b2  ret        

// block 004303b3
004303b3  xor        eax, eax
004303b5  ret        
