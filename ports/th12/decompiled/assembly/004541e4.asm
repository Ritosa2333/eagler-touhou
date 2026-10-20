
// block 004541e4
004541e4  mov        eax, dword ptr [0x4d4754]
004541e9  cmp        eax, ebx
004541eb  je         0x454282

// block 004541f1
004541f1  mov        ecx, dword ptr [ebp + 8]
004541f4  cmp        ecx, ebx
004541f6  je         0x45439a

// block 004541fc
004541fc  cmp        ecx, 1
004541ff  jne        0x4543a1

// block 00454205
00454205  jmp        0x454282
