
// block 00470f6e
00470f6e  mov        edi, edi
00470f70  push       ebp
00470f71  mov        ebp, esp
00470f73  push       ebx
00470f74  mov        bl, byte ptr [ebp + 0x10]
00470f77  push       esi
00470f78  mov        esi, dword ptr [ebp + 8]
00470f7b  mov        al, byte ptr [esi + 8]
00470f7e  cmp        al, 0x70
00470f80  je         0x47108c

// block 00470f86
00470f86  cmp        bl, 0x70
00470f89  je         0x47108c

// block 00470f8f
00470f8f  cmp        al, 0x73
00470f91  je         0x470f9b

// block 00470f93
00470f93  cmp        al, 0x53
00470f95  je         0x470f9b

// block 00470f97
00470f97  xor        edx, edx
00470f99  jmp        0x470f9e

// block 00470f9b
00470f9b  xor        edx, edx
00470f9d  inc        edx

// block 00470f9e
00470f9e  cmp        bl, 0x73
00470fa1  je         0x470fac

// block 00470fa3
00470fa3  cmp        bl, 0x53
00470fa6  je         0x470fac

// block 00470fa8
00470fa8  xor        ecx, ecx
00470faa  jmp        0x470faf

// block 00470fac
00470fac  xor        ecx, ecx
00470fae  inc        ecx

// block 00470faf
00470faf  test       edx, edx
00470fb1  jne        0x471060

// block 00470fb7
00470fb7  test       ecx, ecx
00470fb9  jne        0x471088

// block 00470fbf
00470fbf  mov        dl, 0x64
00470fc1  cmp        al, dl
00470fc3  je         0x471012

// block 00470fc5
00470fc5  cmp        al, 0x69
00470fc7  je         0x470ff6

// block 00470fc9
00470fc9  cmp        al, 0x6f
00470fcb  je         0x470ff6

// block 00470fcd
00470fcd  cmp        al, 0x75
00470fcf  je         0x470ff6

// block 00470fd1
00470fd1  cmp        al, 0x78
00470fd3  je         0x470ff6

// block 00470fd5
00470fd5  cmp        al, 0x58
00470fd7  je         0x470ff6

// block 00470fd9
00470fd9  cmp        bl, dl
00470fdb  je         0x470ff6

// block 00470fdd
00470fdd  cmp        bl, 0x69
00470fe0  je         0x470ff6

// block 00470fe2
00470fe2  cmp        bl, 0x6f
00470fe5  je         0x470ff6

// block 00470fe7
00470fe7  cmp        bl, 0x75
00470fea  je         0x470ff6

// block 00470fec
00470fec  cmp        bl, 0x78
00470fef  je         0x470ff6

// block 00470ff1
00470ff1  cmp        bl, 0x58
00470ff4  jne        0x471054

// block 00470ff6
00470ff6  cmp        al, dl
00470ff8  je         0x471012

// block 00470ffa
00470ffa  cmp        al, 0x69
00470ffc  je         0x471012

// block 00470ffe
00470ffe  cmp        al, 0x6f
00471000  je         0x471012

// block 00471002
00471002  cmp        al, 0x75
00471004  je         0x471012

// block 00471006
00471006  cmp        al, 0x78
00471008  je         0x471012

// block 0047100a
0047100a  cmp        al, 0x58
0047100c  je         0x471012

// block 0047100e
0047100e  xor        ecx, ecx
00471010  jmp        0x471015

// block 00471012
00471012  xor        ecx, ecx
00471014  inc        ecx

// block 00471015
00471015  cmp        bl, dl
00471017  je         0x471036

// block 00471019
00471019  cmp        bl, 0x69
0047101c  je         0x471036

// block 0047101e
0047101e  cmp        bl, 0x6f
00471021  je         0x471036

// block 00471023
00471023  cmp        bl, 0x75
00471026  je         0x471036

// block 00471028
00471028  cmp        bl, 0x78
0047102b  je         0x471036

// block 0047102d
0047102d  cmp        bl, 0x58
00471030  je         0x471036

// block 00471032
00471032  xor        eax, eax
00471034  jmp        0x471039

// block 00471036
00471036  xor        eax, eax
00471038  inc        eax

// block 00471039
00471039  cmp        ecx, eax
0047103b  jne        0x471088

// block 0047103d
0047103d  mov        eax, dword ptr [esi + 0xc]
00471040  mov        ecx, eax
00471042  xor        ecx, dword ptr [ebp + 0x14]
00471045  test       ecx, 0x10000
0047104b  jne        0x471088

// block 0047104d
0047104d  xor        eax, dword ptr [ebp + 0x14]
00471050  test       al, 0x20
00471052  jne        0x471088

// block 00471054
00471054  mov        ecx, dword ptr [esi]
00471056  xor        eax, eax
00471058  cmp        ecx, dword ptr [ebp + 0xc]
0047105b  sete       al
0047105e  jmp        0x471095

// block 00471060
00471060  cmp        edx, ecx
00471062  jne        0x471088

// block 00471064
00471064  mov        ecx, dword ptr [esi + 0xc]
00471067  mov        edx, dword ptr [ebp + 0x14]
0047106a  mov        eax, 0x810
0047106f  and        ecx, eax
00471071  neg        ecx
00471073  sbb        ecx, ecx
00471075  and        edx, eax
00471077  neg        ecx
00471079  neg        edx
0047107b  sbb        edx, edx
0047107d  neg        edx
0047107f  cmp        ecx, edx
00471081  jne        0x471088

// block 00471083
00471083  xor        eax, eax
00471085  inc        eax
00471086  jmp        0x471095

// block 00471088
00471088  xor        eax, eax
0047108a  jmp        0x471095

// block 0047108c
0047108c  xor        ecx, ecx
0047108e  cmp        al, bl
00471090  sete       cl
00471093  mov        eax, ecx

// block 00471095
00471095  pop        esi
00471096  pop        ebx
00471097  pop        ebp
00471098  ret        
