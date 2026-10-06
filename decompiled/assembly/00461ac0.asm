
// block 00461ac0
00461ac0  mov        eax, dword ptr [esp + 4]
00461ac4  mov        edx, dword ptr [0x4ce8cc]
00461aca  push       eax
00461acb  call       0x461920

// block 00461ad0
00461ad0  test       eax, eax
00461ad2  je         0x461aee

// block 00461ad4
00461ad4  mov        ecx, dword ptr [esi]
00461ad6  mov        dword ptr [eax + 0x430], ecx
00461adc  mov        edx, dword ptr [esi + 4]
00461adf  mov        dword ptr [eax + 0x434], edx
00461ae5  mov        ecx, dword ptr [esi + 8]
00461ae8  mov        dword ptr [eax + 0x438], ecx

// block 00461aee
00461aee  ret        4
