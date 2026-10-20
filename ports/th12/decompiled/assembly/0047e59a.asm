
// block 0047e59a
0047e59a  mov        edi, edi
0047e59c  push       ebp
0047e59d  mov        ebp, esp
0047e59f  mov        eax, dword ptr [ebp + 8]
0047e5a2  xor        edx, edx
0047e5a4  push       esi
0047e5a5  mov        esi, ecx
0047e5a7  mov        byte ptr [esi + 4], dl
0047e5aa  and        dword ptr [esi + 4], 0xffff00ff
0047e5b1  mov        dword ptr [esi], edx
0047e5b3  cmp        eax, edx
0047e5b5  je         0x47e5cc

// block 0047e5b7
0047e5b7  xor        ecx, ecx
0047e5b9  cmp        byte ptr [eax], dl
0047e5bb  je         0x47e5c3

// block 0047e5bd
0047e5bd  inc        ecx
0047e5be  cmp        byte ptr [ecx + eax], dl
0047e5c1  jne        0x47e5bd

// block 0047e5c3
0047e5c3  push       ecx
0047e5c4  push       eax
0047e5c5  mov        ecx, esi
0047e5c7  call       0x47e4f1

// block 0047e5cc
0047e5cc  mov        eax, esi
0047e5ce  pop        esi
0047e5cf  pop        ebp
0047e5d0  ret        4
