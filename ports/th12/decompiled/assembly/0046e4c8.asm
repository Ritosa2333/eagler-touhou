
// block 0046e4c8
0046e4c8  mov        edi, edi
0046e4ca  push       ebp
0046e4cb  mov        ebp, esp
0046e4cd  push       esi
0046e4ce  call       0x475279

// block 0046e4d3
0046e4d3  call       0x475273

// block 0046e4d8
0046e4d8  push       eax
0046e4d9  call       0x475259

// block 0046e4de
0046e4de  test       eax, eax
0046e4e0  jne        0x46e50c

// block 0046e4e2
0046e4e2  mov        esi, dword ptr [ebp + 8]
0046e4e5  push       esi
0046e4e6  call       0x475273

// block 0046e4eb
0046e4eb  push       eax
0046e4ec  call       0x4752ad

// block 0046e4f1
0046e4f1  test       eax, eax
0046e4f3  jne        0x46e502

// block 0046e4f5
0046e4f5  call       dword ptr [0x4980e4]

// block 0046e4fb
0046e4fb  push       eax
0046e4fc  call       dword ptr [0x4981e0]

// block 0046e502
0046e502  call       dword ptr [0x4981f0]

// block 0046e508
0046e508  mov        dword ptr [esi], eax
0046e50a  jmp        0x46e527

// block 0046e50c
0046e50c  mov        ecx, dword ptr [ebp + 8]
0046e50f  mov        edx, dword ptr [ecx + 0x54]
0046e512  mov        dword ptr [eax + 0x54], edx
0046e515  mov        edx, dword ptr [ecx + 0x58]
0046e518  mov        dword ptr [eax + 0x58], edx
0046e51b  mov        edx, dword ptr [ecx + 4]
0046e51e  push       ecx
0046e51f  mov        dword ptr [eax + 4], edx
0046e522  call       0x475481

// block 0046e527
0046e527  cmp        dword ptr [0x49d578], 0
0046e52e  je         0x46e545

// block 0046e530
0046e530  push       0x49d578
0046e535  call       0x477a40

// block 0046e53a
0046e53a  pop        ecx
0046e53b  test       eax, eax
0046e53d  je         0x46e545

// block 0046e53f
0046e53f  call       dword ptr [0x49d578]

// block 0046e545
0046e545  call       0x46e487

// block 0046e54a
0046e54a  int3       
