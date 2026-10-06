
// block 004733c6
004733c6  mov        edi, edi
004733c8  push       ebp
004733c9  mov        ebp, esp
004733cb  xor        eax, eax
004733cd  push       eax
004733ce  push       dword ptr [ebp + 0x10]
004733d1  push       dword ptr [ebp + 0xc]
004733d4  push       dword ptr [ebp + 8]
004733d7  cmp        dword ptr [0x4b40dc], eax
004733dd  jne        0x4733e6

// block 004733df
004733df  push       0x4adac0
004733e4  jmp        0x4733e7

// block 004733e6
004733e6  push       eax

// block 004733e7
004733e7  call       0x473197

// block 004733ec
004733ec  add        esp, 0x14
004733ef  pop        ebp
004733f0  ret        
