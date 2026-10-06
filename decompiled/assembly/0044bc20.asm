
// block 0044bc20
0044bc20  mov        eax, edi
0044bc22  lea        edx, [eax + 1]

// block 0044bc25
0044bc25  mov        cl, byte ptr [eax]
0044bc27  inc        eax
0044bc28  test       cl, cl
0044bc2a  jne        0x44bc25

// block 0044bc2c
0044bc2c  sub        eax, edx
0044bc2e  inc        eax
0044bc2f  push       eax
0044bc30  call       0x46d04a

// block 0044bc35
0044bc35  add        esp, 4
0044bc38  test       eax, eax
0044bc3a  je         0x44bc4e

// block 0044bc3c
0044bc3c  push       esi
0044bc3d  mov        esi, eax
0044bc3f  mov        ecx, edi
0044bc41  sub        esi, edi

// block 0044bc43
0044bc43  mov        dl, byte ptr [ecx]
0044bc45  mov        byte ptr [esi + ecx], dl
0044bc48  inc        ecx
0044bc49  test       dl, dl
0044bc4b  jne        0x44bc43

// block 0044bc4d
0044bc4d  pop        esi

// block 0044bc4e
0044bc4e  ret        
