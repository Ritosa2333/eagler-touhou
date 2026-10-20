
// block 0047e9f6
0047e9f6  mov        edi, edi
0047e9f8  push       ebp
0047e9f9  mov        ebp, esp
0047e9fb  push       esi
0047e9fc  mov        esi, ecx
0047e9fe  cmp        byte ptr [esi + 4], 1
0047ea02  jg         0x47ea41

// block 0047ea04
0047ea04  push       ebx
0047ea05  mov        ebx, dword ptr [ebp + 8]
0047ea08  test       bl, bl
0047ea0a  je         0x47ea40

// block 0047ea0c
0047ea0c  cmp        dword ptr [esi], 0
0047ea0f  jne        0x47ea19

// block 0047ea11
0047ea11  push       ebx
0047ea12  call       0x47e869

// block 0047ea17
0047ea17  jmp        0x47ea40

// block 0047ea19
0047ea19  push       0
0047ea1b  push       8
0047ea1d  mov        ecx, 0x4b42f8
0047ea22  call       0x47dc29

// block 0047ea27
0047ea27  test       eax, eax
0047ea29  je         0x47ea36

// block 0047ea2b
0047ea2b  mov        dword ptr [eax], 0x49ddbc
0047ea31  mov        byte ptr [eax + 4], bl
0047ea34  jmp        0x47ea38

// block 0047ea36
0047ea36  xor        eax, eax

// block 0047ea38
0047ea38  push       eax
0047ea39  mov        ecx, esi
0047ea3b  call       0x47e26b

// block 0047ea40
0047ea40  pop        ebx

// block 0047ea41
0047ea41  mov        eax, esi
0047ea43  pop        esi
0047ea44  pop        ebp
0047ea45  ret        4
