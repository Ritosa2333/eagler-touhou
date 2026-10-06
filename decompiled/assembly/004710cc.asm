
// block 004710cc
004710cc  mov        edi, edi
004710ce  push       ebp
004710cf  mov        ebp, esp
004710d1  push       esi
004710d2  mov        esi, eax
004710d4  jmp        0x4710e9

// block 004710d6
004710d6  mov        ecx, dword ptr [ebp + 0x10]
004710d9  mov        al, byte ptr [ebp + 8]
004710dc  dec        dword ptr [ebp + 0xc]
004710df  call       0x471099

// block 004710e4
004710e4  cmp        dword ptr [esi], -1
004710e7  je         0x4710ef

// block 004710e9
004710e9  cmp        dword ptr [ebp + 0xc], 0
004710ed  jg         0x4710d6

// block 004710ef
004710ef  pop        esi
004710f0  pop        ebp
004710f1  ret        
