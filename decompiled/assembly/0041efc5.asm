
// block 0041efc5
0041efc5  push       eax
0041efc6  mov        eax, esi
0041efc8  call       0x45c900

// block 0041efcd
0041efcd  add        esi, 0x4b4
0041efd3  sub        edi, 1
0041efd6  jne        0x41efc0

// block 0041efd8
0041efd8  lea        esi, [ebx + 0x25b0]
0041efde  mov        edi, 8

// block 0041efe3
0041efe3  mov        ecx, dword ptr [0x4ce8cc]
0041efe9  push       ecx
0041efea  mov        eax, esi
0041efec  call       0x45c900

// block 0041eff1
0041eff1  add        esi, 0x4b4
0041eff7  sub        edi, 1
0041effa  jne        0x41efe3

// block 0041effc
0041effc  mov        eax, dword ptr [0x4b43dc]
0041f001  pop        edi
0041f002  pop        esi
0041f003  test       eax, eax
0041f005  je         0x41f038

// block 0041f007
0041f007  mov        eax, dword ptr [eax + 0x1c]
0041f00a  test       eax, eax
0041f00c  je         0x41f038

// block 0041f00e
0041f00e  mov        eax, dword ptr [eax + 0x26f8]
0041f014  mov        edx, eax
0041f016  shr        edx, 5
0041f019  not        edx
0041f01b  test       dl, 1
0041f01e  je         0x41f038

// block 0041f020
0041f020  not        eax
0041f022  test       al, 1
0041f024  je         0x41f038

// block 0041f026
0041f026  mov        ecx, dword ptr [0x4ce8cc]

// block 0041f038
0041f038  mov        eax, 1
0041f03d  ret        
