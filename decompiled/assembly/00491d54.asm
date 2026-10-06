
// block 00491d54
00491d54  mov        edi, edi
00491d56  push       ebp
00491d57  mov        ebp, esp
00491d59  mov        eax, dword ptr [ebp + 0xc]
00491d5c  push       esi
00491d5d  mov        esi, dword ptr [ebp + 8]
00491d60  mov        dword ptr [esi], eax
00491d62  call       0x475467

// block 00491d67
00491d67  mov        eax, dword ptr [eax + 0x98]
00491d6d  mov        dword ptr [esi + 4], eax
00491d70  call       0x475467

// block 00491d75
00491d75  mov        dword ptr [eax + 0x98], esi
00491d7b  mov        eax, esi
00491d7d  pop        esi
00491d7e  pop        ebp
00491d7f  ret        
