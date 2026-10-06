
// block 004124e0
004124e0  mov        eax, dword ptr [0x4b43dc]
004124e5  mov        ecx, dword ptr [eax + 0x3c]
004124e8  xor        ecx, dword ptr [esp + 4]
004124ec  and        ecx, 1
004124ef  xor        dword ptr [eax + 0x3c], ecx
004124f2  ret        4
