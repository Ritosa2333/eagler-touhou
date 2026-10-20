
// block 0046a250
0046a250  push       ebx
0046a251  push       esi
0046a252  push       edi
0046a253  push       0x30
0046a255  mov        edi, edx
0046a257  mov        ebx, ecx
0046a259  call       0x46d04a

// block 0046a25e
0046a25e  push       0x30
0046a260  mov        esi, eax
0046a262  push       0
0046a264  push       esi
0046a265  mov        dword ptr [ebx + 0x478], esi
0046a26b  call       0x477420

// block 0046a270
0046a270  fldz       
0046a272  fst        dword ptr [esi + 0x24]
0046a275  add        esp, 0x10
0046a278  fst        dword ptr [esi + 0x20]
0046a27b  fld        dword ptr [0x4a4048]
0046a281  fstp       dword ptr [esi + 0x28]
0046a284  fld        dword ptr [0x4a4044]
0046a28a  fstp       dword ptr [esi + 0x2c]
0046a28d  mov        eax, dword ptr [edi]
0046a28f  mov        dword ptr [esi], eax
0046a291  mov        ecx, dword ptr [edi + 4]
0046a294  mov        dword ptr [esi + 4], ecx
0046a297  mov        edx, dword ptr [edi + 8]
0046a29a  mov        dword ptr [esi + 8], edx
0046a29d  mov        eax, dword ptr [esi + 0x1c]
0046a2a0  test       al, 1
0046a2a2  jne        0x46a2c2

// block 0046a2a4
0046a2a4  or         eax, 1
0046a2a7  fst        dword ptr [esi + 0x14]
0046a2aa  mov        dword ptr [esi + 0x10], 0
0046a2b1  mov        dword ptr [esi + 0xc], 0xfff0bdc1
0046a2b8  mov        dword ptr [esi + 0x18], 0x4b2ed0
0046a2bf  mov        dword ptr [esi + 0x1c], eax

// block 0046a2c2
0046a2c2  fstp       dword ptr [esi + 0x14]
0046a2c5  mov        dword ptr [esi + 0x10], 0
0046a2cc  mov        dword ptr [esi + 0xc], 0xffffffff
0046a2d3  mov        eax, dword ptr [esi]
0046a2d5  mov        dword ptr [ebx + 0x430], eax
0046a2db  mov        ecx, dword ptr [esi + 4]
0046a2de  mov        dword ptr [ebx + 0x434], ecx
0046a2e4  mov        edx, dword ptr [esi + 8]
0046a2e7  and        dword ptr [ebx + 0x47c], 0xfffffffd
0046a2ee  pop        edi
0046a2ef  pop        esi
0046a2f0  mov        dword ptr [ebx + 0x438], edx
0046a2f6  xor        eax, eax
0046a2f8  pop        ebx
0046a2f9  ret        
