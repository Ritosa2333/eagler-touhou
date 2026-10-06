
// block 0041d150
0041d150  push       -1
0041d152  push       0x49716f
0041d157  mov        eax, dword ptr fs:[0]
0041d15d  push       eax
0041d15e  push       esi
0041d15f  mov        eax, dword ptr [0x4ad138]
0041d164  xor        eax, esp
0041d166  push       eax
0041d167  lea        eax, [esp + 8]
0041d16b  mov        dword ptr fs:[0], eax
0041d171  mov        esi, dword ptr [esp + 0x18]
0041d175  push       0x4026e0
0041d17a  push       0x4027e0
0041d17f  push       8
0041d181  push       0x4b4
0041d186  lea        eax, [esi + 0x10]
0041d189  push       eax
0041d18a  call       0x46ce49

// block 0041d18f
0041d18f  push       0x4026e0
0041d194  push       0x4027e0
0041d199  push       8
0041d19b  push       0x4b4
0041d1a0  lea        ecx, [esi + 0x25b0]
0041d1a6  push       ecx
0041d1a7  mov        dword ptr [esp + 0x24], 0
0041d1af  call       0x46ce49

// block 0041d1b4
0041d1b4  push       0x4026e0
0041d1b9  push       0x4027e0
0041d1be  push       2
0041d1c0  push       0x4b4
0041d1c5  lea        edx, [esi + 0x4b50]
0041d1cb  push       edx
0041d1cc  mov        byte ptr [esp + 0x24], 1
0041d1d1  call       0x46ce49

// block 0041d1d6
0041d1d6  push       0x4026e0
0041d1db  push       0x4027e0
0041d1e0  push       4
0041d1e2  push       0x4b4
0041d1e7  lea        eax, [esi + 0x54b8]
0041d1ed  push       eax
0041d1ee  mov        byte ptr [esp + 0x24], 2
0041d1f3  call       0x46ce49

// block 0041d1f8
0041d1f8  lea        ecx, [esi + 0x678c]
0041d1fe  call       0x4027e0

// block 0041d203
0041d203  push       0x6d50
0041d208  mov        eax, 0xfffffffe
0041d20d  and        dword ptr [esi + 0x6cd0], eax
0041d213  and        dword ptr [esi + 0x6d2c], eax
0041d219  push       0
0041d21b  push       esi
0041d21c  call       0x477420

// block 0041d221
0041d221  add        esp, 0xc
0041d224  or         dword ptr [esi], 2
0041d227  mov        dword ptr [0x4b43e4], esi
0041d22d  mov        eax, esi
0041d22f  mov        ecx, dword ptr [esp + 8]
0041d233  mov        dword ptr fs:[0], ecx
0041d23a  pop        ecx
0041d23b  pop        esi
0041d23c  add        esp, 0xc
0041d23f  ret        4
