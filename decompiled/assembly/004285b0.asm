
// block 004285b0
004285b0  push       esi
004285b1  mov        esi, eax
004285b3  call       0x427ec0

// block 004285b8
004285b8  push       0x1e0
004285bd  lea        eax, [esi + 0x454]
004285c3  push       0
004285c5  push       eax
004285c6  mov        dword ptr [esi], 0x4a06ac
004285cc  call       0x477420

// block 004285d1
004285d1  add        esp, 0xc
004285d4  lea        ecx, [esi + 0x634]
004285da  call       0x4027e0

// block 004285df
004285df  lea        ecx, [esi + 0xae8]
004285e5  call       0x4027e0

// block 004285ea
004285ea  mov        eax, esi
004285ec  pop        esi
004285ed  ret        
