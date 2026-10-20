
// block 0047e3b8
0047e3b8  mov        edi, edi
0047e3ba  push       ebp
0047e3bb  mov        ebp, esp
0047e3bd  push       esi
0047e3be  push       edi
0047e3bf  mov        edi, dword ptr [ebp + 0xc]
0047e3c2  mov        esi, ecx
0047e3c4  mov        dword ptr [esi], 0x49ddec
0047e3ca  test       edi, edi
0047e3cc  je         0x47e3ff

// block 0047e3ce
0047e3ce  cmp        dword ptr [ebp + 8], 0
0047e3d2  je         0x47e3ff

// block 0047e3d4
0047e3d4  push       0
0047e3d6  push       edi
0047e3d7  mov        ecx, 0x4b42f8
0047e3dc  call       0x47dc29

// block 0047e3e1
0047e3e1  mov        dword ptr [esi + 4], eax
0047e3e4  mov        dword ptr [esi + 8], edi
0047e3e7  test       eax, eax
0047e3e9  je         0x47e407

// block 0047e3eb
0047e3eb  test       edi, edi
0047e3ed  jbe        0x47e407

// block 0047e3ef
0047e3ef  mov        ecx, dword ptr [ebp + 8]
0047e3f2  sub        ecx, eax

// block 0047e3f4
0047e3f4  mov        dl, byte ptr [ecx + eax]
0047e3f7  mov        byte ptr [eax], dl
0047e3f9  inc        eax
0047e3fa  dec        edi
0047e3fb  jne        0x47e3f4

// block 0047e3fd
0047e3fd  jmp        0x47e407

// block 0047e3ff
0047e3ff  and        dword ptr [esi + 4], 0
0047e403  and        dword ptr [esi + 8], 0

// block 0047e407
0047e407  pop        edi
0047e408  mov        eax, esi
0047e40a  pop        esi
0047e40b  pop        ebp
0047e40c  ret        8
