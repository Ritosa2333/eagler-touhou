
// block 00466650
00466650  push       esi
00466651  push       eax
00466652  mov        esi, edx
00466654  push       ecx
00466655  lea        edx, [esp + 0x10]
00466659  push       edx
0046665a  call       0x465d40

// block 0046665f
0046665f  mov        ecx, dword ptr [esp + 0xc]
00466663  xor        eax, eax
00466665  mov        dword ptr [esi + 0x60], eax
00466668  mov        dword ptr [esi + 0x64], eax
0046666b  mov        dword ptr [esi + 0x68], eax
0046666e  mov        dword ptr [esi + 0x6c], eax
00466671  mov        dword ptr [esi], 0x4a3b24
00466677  mov        dword ptr [esi + 0x70], ecx
0046667a  mov        eax, esi
0046667c  pop        esi
0046667d  ret        8
