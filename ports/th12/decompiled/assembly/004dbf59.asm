
// block 004dbf59
004dbf59  push       esi
004dbf5a  mov        ebx, dword ptr [esi]
004dbf5c  add        ebx, dword ptr [ebp + 0x488]
004dbf62  push       dword ptr [ebp + 0x148]
004dbf68  push       dword ptr [esi + 4]
004dbf6b  push       eax
004dbf6c  push       ebx
004dbf6d  call       0x4dc539

// block 004dbf72
004dbf72  mov        bl, 0
004dbf74  cmp        bl, 0
004dbf77  jne        0x4dbfc6

// block 004dbf79
004dbf79  inc        byte ptr [ebp + 0xef]
004dbf7f  push       eax
004dbf80  push       ecx
004dbf81  push       esi
004dbf82  push       ebx
004dbf83  mov        ecx, eax
004dbf85  sub        ecx, 6
004dbf88  mov        esi, dword ptr [ebp + 0x144]
004dbf8e  xor        ebx, ebx

// block 004dbf90
004dbf90  or         ecx, ecx
004dbf92  je         0x4dbfc2

// block 004dbf94
004dbf94  js         0x4dbfc2

// block 004dbf96
004dbf96  lodsb      al, byte ptr [esi]
004dbf97  cmp        al, 0xe8
004dbf99  je         0x4dbfa5

// block 004dbf9b
004dbf9b  jmp        0x4dbf9d

// block 004dbf9d
004dbf9d  cmp        al, 0xe9
004dbf9f  je         0x4dbfa5

// block 004dbfa1
004dbfa1  inc        ebx
004dbfa2  dec        ecx
004dbfa3  jmp        0x4dbf90

// block 004dbfa5
004dbfa5  mov        eax, dword ptr [esi]
004dbfa7  jmp        0x4dbfa9

// block 004dbfa9
004dbfa9  cmp        byte ptr [esi], 0
004dbfac  jne        0x4dbfa1

// block 004dbfae
004dbfae  and        al, 0
004dbfb0  rol        eax, 0x18
004dbfb3  sub        eax, ebx
004dbfb5  mov        dword ptr [esi], eax
004dbfb7  add        ebx, 5
004dbfba  add        esi, 4
004dbfbd  sub        ecx, 5
004dbfc0  jmp        0x4dbf90

// block 004dbfc2
004dbfc2  pop        ebx
004dbfc3  pop        esi
004dbfc4  pop        ecx
004dbfc5  pop        eax

// block 004dbfc6
004dbfc6  jmp        0x4dbfd0

// block 004dbfd0
004dbfd0  mov        ecx, eax
004dbfd2  mov        edi, dword ptr [esi]
004dbfd4  add        edi, dword ptr [ebp + 0x488]
004dbfda  mov        esi, dword ptr [ebp + 0x144]
004dbfe0  sar        ecx, 2

// block 004dbfe3
004dbfe3  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004dbfe5
004dbfe5  mov        ecx, eax
004dbfe7  and        ecx, 3

// block 004dbfea
004dbfea  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 004dbfec
004dbfec  pop        esi
004dbfed  push       0x8000
004dbff2  push       0
004dbff4  push       dword ptr [ebp + 0x144]
004dbffa  call       dword ptr [ebp + 0x5e]
