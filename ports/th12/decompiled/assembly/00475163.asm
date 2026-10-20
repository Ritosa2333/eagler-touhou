
// block 00475163
00475163  mov        edi, edi
00475165  push       ebp
00475166  mov        ebp, esp
00475168  push       esi
00475169  push       dword ptr [0x4adacc]
0047516f  mov        esi, dword ptr [0x49814c]
00475175  call       esi

// block 00475177
00475177  test       eax, eax
00475179  je         0x47519c

// block 0047517b
0047517b  mov        eax, dword ptr [0x4adac8]
00475180  cmp        eax, -1
00475183  je         0x47519c

// block 00475185
00475185  push       eax
00475186  push       dword ptr [0x4adacc]
0047518c  call       esi

// block 0047518e
0047518e  call       eax

// block 00475190
00475190  test       eax, eax
00475192  je         0x47519c

// block 00475194
00475194  mov        eax, dword ptr [eax + 0x1f8]
0047519a  jmp        0x4751c3

// block 0047519c
0047519c  mov        esi, 0x49d51c
004751a1  push       esi
004751a2  call       dword ptr [0x498174]

// block 004751a8
004751a8  test       eax, eax
004751aa  jne        0x4751b7

// block 004751ac
004751ac  push       esi
004751ad  call       0x472bdd

// block 004751b2
004751b2  pop        ecx
004751b3  test       eax, eax
004751b5  je         0x4751cf

// block 004751b7
004751b7  push       0x49d50c
004751bc  push       eax
004751bd  call       dword ptr [0x498170]

// block 004751c3
004751c3  test       eax, eax
004751c5  je         0x4751cf

// block 004751c7
004751c7  push       dword ptr [ebp + 8]
004751ca  call       eax

// block 004751cc
004751cc  mov        dword ptr [ebp + 8], eax

// block 004751cf
004751cf  mov        eax, dword ptr [ebp + 8]
004751d2  pop        esi
004751d3  pop        ebp
004751d4  ret        
