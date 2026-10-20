
// block 004096a0
004096a0  push       esi
004096a1  mov        esi, dword ptr [0x4ce8cc]
004096a7  add        esi, 0x4b50d8
004096ad  push       edi
004096ae  mov        edi, dword ptr [esi]
004096b0  test       edi, edi
004096b2  je         0x4096ca

// block 004096b4
004096b4  call       0x4604e0

// block 004096b9
004096b9  mov        eax, dword ptr [esi]
004096bb  push       eax
004096bc  call       0x46ca4f

// block 004096c1
004096c1  add        esp, 4
004096c4  mov        dword ptr [esi], 0

// block 004096ca
004096ca  pop        edi
004096cb  xor        eax, eax
004096cd  pop        esi
004096ce  ret        
