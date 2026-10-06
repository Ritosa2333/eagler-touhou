
// block 0040ea30
0040ea30  mov        dword ptr [esi + 0x10], 0x4a3738
0040ea37  push       edi
0040ea38  xor        edi, edi
0040ea3a  mov        dword ptr [esi + 0x14], edi
0040ea3d  mov        dword ptr [esi + 0x18], edi
0040ea40  mov        dword ptr [esi + 0x1c], edi
0040ea43  mov        dword ptr [esi + 0x20], edi
0040ea46  mov        dword ptr [esi + 0xc8], edi
0040ea4c  mov        dword ptr [esi + 0x3c], edi
0040ea4f  mov        dword ptr [esi + 0x110], edi
0040ea55  mov        ecx, 1
0040ea5a  mov        dword ptr [esi + 0x10c], ecx
0040ea60  mov        eax, 0x3e7
0040ea65  mov        dword ptr [esi + 0x44], eax
0040ea68  mov        dword ptr [esi + 0x1e4], ecx
0040ea6e  mov        dword ptr [esi + 0x1a0], edi
0040ea74  mov        dword ptr [esi + 0x114], edi
0040ea7a  mov        dword ptr [esi + 0x1e8], edi
0040ea80  mov        dword ptr [esi + 0x11c], eax
0040ea86  mov        dword ptr [esi + 0x2bc], ecx
0040ea8c  lea        ecx, [esi + 0x2c8]
0040ea92  mov        dword ptr [esi + 0x278], edi
0040ea98  mov        dword ptr [esi + 0x1ec], edi
0040ea9e  mov        dword ptr [esi + 0x2c0], edi
0040eaa4  mov        dword ptr [esi + 0x1f4], eax
0040eaaa  call       0x4027e0

// block 0040eaaf
0040eaaf  push       0x790
0040eab4  push       edi
0040eab5  push       esi
0040eab6  call       0x477420

// block 0040eabb
0040eabb  or         dword ptr [esi], 2
0040eabe  add        esp, 0xc
0040eac1  mov        dword ptr [0x4b43d0], esi
0040eac7  mov        eax, esi
0040eac9  pop        edi
0040eaca  ret        
