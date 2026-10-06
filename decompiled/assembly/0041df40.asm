
// block 0041df40
0041df40  push       -1
0041df42  push       0x4977db
0041df47  mov        eax, dword ptr fs:[0]
0041df4d  push       eax
0041df4e  push       ecx
0041df4f  push       esi
0041df50  mov        eax, dword ptr [0x4ad138]
0041df55  xor        eax, esp
0041df57  push       eax
0041df58  lea        eax, [esp + 0xc]
0041df5c  mov        dword ptr fs:[0], eax
0041df62  push       0x6d50
0041df67  call       0x46c9ea

// block 0041df6c
0041df6c  add        esp, 4
0041df6f  mov        dword ptr [esp + 8], eax
0041df73  mov        dword ptr [esp + 0x14], 0
0041df7b  test       eax, eax
0041df7d  je         0x41df89

// block 0041df7f
0041df7f  push       eax
0041df80  call       0x41d150

// block 0041df85
0041df85  mov        esi, eax
0041df87  jmp        0x41df8b

// block 0041df89
0041df89  xor        esi, esi

// block 0041df8b
0041df8b  push       esi
0041df8c  mov        dword ptr [esp + 0x18], 0xffffffff
0041df94  call       0x41d250

// block 0041df99
0041df99  test       eax, eax
0041df9b  je         0x41dfc3

// block 0041df9d
0041df9d  test       esi, esi
0041df9f  je         0x41dfb0

// block 0041dfa1
0041dfa1  push       esi
0041dfa2  call       0x41dc50

// block 0041dfa7
0041dfa7  push       esi
0041dfa8  call       0x46ca4f

// block 0041dfad
0041dfad  add        esp, 4

// block 0041dfb0
0041dfb0  xor        eax, eax
0041dfb2  mov        ecx, dword ptr [esp + 0xc]
0041dfb6  mov        dword ptr fs:[0], ecx
0041dfbd  pop        ecx
0041dfbe  pop        esi
0041dfbf  add        esp, 0x10
0041dfc2  ret        

// block 0041dfc3
0041dfc3  mov        eax, esi
0041dfc5  mov        ecx, dword ptr [esp + 0xc]
0041dfc9  mov        dword ptr fs:[0], ecx
0041dfd0  pop        ecx
0041dfd1  pop        esi
0041dfd2  add        esp, 0x10
0041dfd5  ret        
