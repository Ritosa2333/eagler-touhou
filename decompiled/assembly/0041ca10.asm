
// block 0041ca10
0041ca10  push       ebx
0041ca11  push       esi
0041ca12  push       0x24
0041ca14  call       0x46c9ea

// block 0041ca19
0041ca19  xor        ecx, ecx
0041ca1b  add        esp, 4
0041ca1e  cmp        eax, ecx
0041ca20  je         0x41ca3e

// block 0041ca22
0041ca22  and        dword ptr [eax + 4], 0xfffffffe
0041ca26  mov        dword ptr [eax + 8], ecx
0041ca29  mov        dword ptr [eax + 0xc], ecx
0041ca2c  mov        dword ptr [eax + 0x10], ecx
0041ca2f  mov        dword ptr [eax], ecx
0041ca31  mov        dword ptr [eax + 0x14], eax
0041ca34  mov        dword ptr [eax + 0x18], ecx
0041ca37  mov        dword ptr [eax + 0x1c], ecx
0041ca3a  mov        esi, eax
0041ca3c  jmp        0x41ca40

// block 0041ca3e
0041ca3e  xor        esi, esi

// block 0041ca40
0041ca40  or         dword ptr [esi + 4], 3
0041ca44  mov        ebx, 0x3e
0041ca49  mov        dword ptr [esi + 8], 0x41cd60
0041ca50  mov        dword ptr [esi + 0xc], ecx
0041ca53  mov        dword ptr [esi + 0x10], ecx
0041ca56  mov        dword ptr [esi + 0x20], edi
0041ca59  call       0x462420

// block 0041ca5e
0041ca5e  mov        dword ptr [edi + 0xc], esi
0041ca61  pop        esi
0041ca62  xor        eax, eax
0041ca64  pop        ebx
0041ca65  ret        
