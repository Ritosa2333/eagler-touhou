
// block 004127b0
004127b0  push       esi
004127b1  mov        esi, dword ptr [0x4b43cc]
004127b7  or         dword ptr [esi + 0x7c], 0x10
004127bb  mov        eax, dword ptr [esi + 0x20]
004127be  push       eax
004127bf  call       0x461a70

// block 004127c4
004127c4  mov        dword ptr [esi + 0x20], 0
004127cb  pop        esi
004127cc  ret        
