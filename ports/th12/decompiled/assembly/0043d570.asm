
// block 0043d570
0043d570  xor        eax, eax
0043d572  mov        dword ptr [esi + 0x10], 0x4a3738
0043d579  mov        dword ptr [esi + 0x14], eax
0043d57c  mov        dword ptr [esi + 0x18], eax
0043d57f  mov        dword ptr [esi + 0x1c], eax
0043d582  mov        dword ptr [esi + 0x20], eax
0043d585  mov        dword ptr [esi + 0xc0], eax
0043d58b  mov        dword ptr [esi + 0x34], eax
0043d58e  mov        dword ptr [esi + 0x108], eax
0043d594  mov        edx, 1
0043d599  mov        dword ptr [esi + 0x104], edx
0043d59f  mov        ecx, 0x3e7
0043d5a4  mov        dword ptr [esi + 0x3c], ecx
0043d5a7  mov        dword ptr [esi + 0x198], eax
0043d5ad  mov        dword ptr [esi + 0x10c], eax
0043d5b3  mov        dword ptr [esi + 0x1e0], eax
0043d5b9  mov        dword ptr [esi + 0x1dc], edx
0043d5bf  mov        dword ptr [esi + 0x114], ecx
0043d5c5  push       0x2bc
0043d5ca  push       eax
0043d5cb  push       esi
0043d5cc  mov        dword ptr [esi + 0x270], eax
0043d5d2  mov        dword ptr [esi + 0x1e4], eax
0043d5d8  mov        dword ptr [esi + 0x2b8], eax
0043d5de  mov        dword ptr [esi + 0x2b4], edx
0043d5e4  mov        dword ptr [esi + 0x1ec], ecx
0043d5ea  call       0x477420

// block 0043d5ef
0043d5ef  or         dword ptr [esi], 2
0043d5f2  add        esp, 0xc
0043d5f5  mov        dword ptr [0x4b4520], esi
0043d5fb  mov        eax, esi
0043d5fd  ret        
