
// block 0044cc30
0044cc30  lea        eax, [eax + eax*2]
0044cc33  mov        eax, dword ptr [eax*4 + 0x4b4544]
0044cc3a  lea        ecx, [eax + eax*2]
0044cc3d  mov        ecx, dword ptr [ecx*4 + 0x4b4548]
0044cc44  test       ecx, ecx
0044cc46  je         0x44cc60

// block 0044cc48
0044cc48  jmp        0x44cc50

// block 0044cc50
0044cc50  mov        eax, ecx
0044cc52  lea        edx, [eax + eax*2]
0044cc55  mov        ecx, dword ptr [edx*4 + 0x4b4548]
0044cc5c  test       ecx, ecx
0044cc5e  jne        0x44cc50

// block 0044cc60
0044cc60  ret        
