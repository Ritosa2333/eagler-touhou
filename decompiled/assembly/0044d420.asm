
// block 0044d420
0044d420  push       -1
0044d422  push       0x496f7b
0044d427  mov        eax, dword ptr fs:[0]
0044d42d  push       eax
0044d42e  push       esi
0044d42f  mov        eax, dword ptr [0x4ad138]
0044d434  xor        eax, esp
0044d436  push       eax
0044d437  lea        eax, [esp + 8]
0044d43b  mov        dword ptr fs:[0], eax
0044d441  mov        esi, dword ptr [esp + 0x18]
0044d445  lea        ecx, [esi + 0xc]
0044d448  xor        eax, eax
0044d44a  mov        dword ptr [ecx + 0x18], 0xf
0044d451  mov        dword ptr [ecx + 0x14], eax
0044d454  mov        byte ptr [ecx + 4], al
0044d457  mov        dword ptr [esp + 0x10], eax
0044d45b  push       0xd
0044d45d  push       0x4a245c
0044d462  mov        dword ptr [esi], eax
0044d464  mov        dword ptr [esi + 0x28], 0x258
0044d46b  mov        dword ptr [esi + 8], 0x10
0044d472  mov        dword ptr [esi + 4], eax
0044d475  call       0x44ec80

// block 0044d47a
0044d47a  mov        eax, esi
0044d47c  mov        ecx, dword ptr [esp + 8]
0044d480  mov        dword ptr fs:[0], ecx
0044d487  pop        ecx
0044d488  pop        esi
0044d489  add        esp, 0xc
0044d48c  ret        4
