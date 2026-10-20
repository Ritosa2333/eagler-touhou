
// block 0047e17b
0047e17b  mov        edi, edi
0047e17d  push       ebp
0047e17e  mov        ebp, esp
0047e180  cmp        dword ptr [ebp + 8], 0
0047e184  push       esi
0047e185  mov        esi, ecx
0047e187  je         0x47e1b9

// block 0047e189
0047e189  push       0
0047e18b  push       8
0047e18d  mov        ecx, 0x4b42f8
0047e192  call       0x47dc29

// block 0047e197
0047e197  test       eax, eax
0047e199  je         0x47e1a7

// block 0047e19b
0047e19b  push       dword ptr [ebp + 8]
0047e19e  mov        ecx, eax
0047e1a0  call       0x47de46

// block 0047e1a5
0047e1a5  jmp        0x47e1a9

// block 0047e1a7
0047e1a7  xor        eax, eax

// block 0047e1a9
0047e1a9  test       eax, eax
0047e1ab  mov        dword ptr [esi], eax
0047e1ad  setne      al
0047e1b0  dec        al
0047e1b2  and        al, 3
0047e1b4  mov        byte ptr [esi + 4], al
0047e1b7  jmp        0x47e1c0

// block 0047e1b9
0047e1b9  and        dword ptr [esi], 0
0047e1bc  mov        byte ptr [esi + 4], 0

// block 0047e1c0
0047e1c0  and        dword ptr [esi + 4], 0xffff00ff
0047e1c7  mov        eax, esi
0047e1c9  pop        esi
0047e1ca  pop        ebp
0047e1cb  ret        4
