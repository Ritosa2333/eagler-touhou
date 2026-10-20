
// block 004061e0
004061e0  push       esi
004061e1  mov        esi, eax
004061e3  call       0x402520

// block 004061e8
004061e8  mov        ecx, dword ptr [esp + 8]
004061ec  mov        al, 0x10
004061ee  mov        byte ptr [esi + 0x49d], al
004061f4  mov        byte ptr [esi + 0x49c], al
004061fa  mov        eax, dword ptr [esp + 0xc]
004061fe  push       eax
004061ff  push       esi
00406200  call       0x454d10

// block 00406205
00406205  pop        esi
00406206  ret        8
