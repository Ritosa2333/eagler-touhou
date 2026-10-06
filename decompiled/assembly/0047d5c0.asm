
// block 0047d5c0
0047d5c0  mov        edi, edi
0047d5c2  push       esi
0047d5c3  mov        esi, ecx
0047d5c5  cmp        dword ptr [esi + 4], 0
0047d5c9  je         0x47d5e6

// block 0047d5cb
0047d5cb  jmp        0x47d5dc

// block 0047d5cd
0047d5cd  mov        eax, dword ptr [esi + 0xc]
0047d5d0  mov        eax, dword ptr [eax]
0047d5d2  push       dword ptr [esi + 0xc]
0047d5d5  mov        dword ptr [esi + 8], eax
0047d5d8  call       dword ptr [esi + 4]

// block 0047d5db
0047d5db  pop        ecx

// block 0047d5dc
0047d5dc  mov        eax, dword ptr [esi + 8]
0047d5df  mov        dword ptr [esi + 0xc], eax
0047d5e2  test       eax, eax
0047d5e4  jne        0x47d5cd

// block 0047d5e6
0047d5e6  pop        esi
0047d5e7  ret        
