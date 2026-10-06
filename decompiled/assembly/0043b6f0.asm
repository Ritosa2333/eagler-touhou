
// block 0043b6f0
0043b6f0  push       -1
0043b6f2  push       0x49760b
0043b6f7  mov        eax, dword ptr fs:[0]
0043b6fd  push       eax
0043b6fe  push       ecx
0043b6ff  push       esi
0043b700  mov        eax, dword ptr [0x4ad138]
0043b705  xor        eax, esp
0043b707  push       eax
0043b708  lea        eax, [esp + 0xc]
0043b70c  mov        dword ptr fs:[0], eax
0043b712  push       0x2e0
0043b717  call       0x46c9ea

// block 0043b71c
0043b71c  mov        esi, eax
0043b71e  add        esp, 4
0043b721  mov        dword ptr [esp + 8], esi
0043b725  mov        dword ptr [esp + 0x14], 0
0043b72d  test       esi, esi
0043b72f  je         0x43b75d

// block 0043b731
0043b731  push       0x43cd80
0043b736  push       0x43cd40
0043b73b  push       8
0043b73d  push       0x24
0043b73f  lea        eax, [esi + 0xa8]
0043b745  push       eax
0043b746  call       0x46ce49

// block 0043b74b
0043b74b  push       0x2e0
0043b750  push       0
0043b752  push       esi
0043b753  call       0x477420

// block 0043b758
0043b758  add        esp, 0xc
0043b75b  jmp        0x43b75f

// block 0043b75d
0043b75d  xor        esi, esi

// block 0043b75f
0043b75f  mov        ecx, dword ptr [esp + 0x1c]
0043b763  push       ecx
0043b764  mov        dword ptr [esp + 0x18], 0xffffffff
0043b76c  push       esi
0043b76d  mov        dword ptr [esi + 0x10], 2
0043b774  call       0x43c350

// block 0043b779
0043b779  test       eax, eax
0043b77b  je         0x43b7a1

// block 0043b77d
0043b77d  push       esi
0043b77e  call       0x43b450

// block 0043b783
0043b783  push       esi
0043b784  call       0x46ca4f

// block 0043b789
0043b789  add        esp, 4
0043b78c  xor        eax, eax
0043b78e  mov        ecx, dword ptr [esp + 0xc]
0043b792  mov        dword ptr fs:[0], ecx
0043b799  pop        ecx
0043b79a  pop        esi
0043b79b  add        esp, 0x10
0043b79e  ret        4

// block 0043b7a1
0043b7a1  mov        eax, esi
0043b7a3  mov        ecx, dword ptr [esp + 0xc]
0043b7a7  mov        dword ptr fs:[0], ecx
0043b7ae  pop        ecx
0043b7af  pop        esi
0043b7b0  add        esp, 0x10
0043b7b3  ret        4
