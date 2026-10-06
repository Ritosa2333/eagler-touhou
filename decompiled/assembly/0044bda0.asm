
// block 0044bda0
0044bda0  sub        esp, 0x10c
0044bda6  mov        eax, dword ptr [0x4ad138]
0044bdab  xor        eax, esp
0044bdad  mov        dword ptr [esp + 0x108], eax
0044bdb4  push       ebx
0044bdb5  push       ebp
0044bdb6  mov        ebp, dword ptr [esp + 0x118]
0044bdbd  push       esi
0044bdbe  mov        esi, ecx
0044bdc0  mov        eax, dword ptr [esi]
0044bdc2  mov        edx, dword ptr [eax + 4]
0044bdc5  push       edi
0044bdc6  xor        edi, edi
0044bdc8  mov        dword ptr [esp + 0x10], edi
0044bdcc  call       edx

// block 0044bdce
0044bdce  mov        eax, dword ptr [esp + 0x124]
0044bdd5  mov        ebx, eax
0044bdd7  mov        al, byte ptr [eax]
0044bdd9  test       al, al
0044bddb  je         0x44be26

// block 0044bddd
0044bddd  lea        ecx, [ecx]

// block 0044bde0
0044bde0  cmp        al, 0x72
0044bde2  je         0x44bdf6

// block 0044bde4
0044bde4  cmp        al, 0x77
0044bde6  je         0x44be04

// block 0044bde8
0044bde8  cmp        al, 0x61
0044bdea  je         0x44be12

// block 0044bdec
0044bdec  mov        al, byte ptr [ebx + 1]
0044bdef  inc        ebx
0044bdf0  test       al, al
0044bdf2  jne        0x44bde0

// block 0044bdf4
0044bdf4  jmp        0x44be73

// block 0044bdf6
0044bdf6  mov        dword ptr [esi + 8], 0x80000000
0044bdfd  mov        edi, 3
0044be02  jmp        0x44be26

// block 0044be04
0044be04  push       ebp
0044be05  call       dword ptr [0x498098]

// block 0044be0b
0044be0b  mov        edi, 2
0044be10  jmp        0x44be1f

// block 0044be12
0044be12  mov        dword ptr [esp + 0x10], 1
0044be1a  mov        edi, 4

// block 0044be1f
0044be1f  mov        dword ptr [esi + 8], 0x40000000

// block 0044be26
0044be26  cmp        byte ptr [ebx], 0
0044be29  jne        0x44be2f

// block 0044be2b
0044be2b  xor        al, al
0044be2d  jmp        0x44be73

// block 0044be2f
0044be2f  mov        eax, ebp
0044be31  lea        ecx, [esp + 0x14]
0044be35  call       0x44c050

// block 0044be3a
0044be3a  mov        eax, dword ptr [esi + 8]
0044be3d  push       0
0044be3f  push       0x8000080
0044be44  push       edi
0044be45  push       0
0044be47  push       1
0044be49  push       eax
0044be4a  lea        ecx, [esp + 0x2c]
0044be4e  push       ecx
0044be4f  call       dword ptr [0x49809c]

// block 0044be55
0044be55  mov        dword ptr [esi + 4], eax
0044be58  cmp        eax, -1
0044be5b  je         0x44be2b

// block 0044be5d
0044be5d  cmp        dword ptr [esp + 0x10], 0
0044be62  je         0x44be71

// block 0044be64
0044be64  push       2
0044be66  push       0
0044be68  push       0
0044be6a  push       eax
0044be6b  call       dword ptr [0x4980a0]

// block 0044be71
0044be71  mov        al, 1

// block 0044be73
0044be73  mov        ecx, dword ptr [esp + 0x118]
0044be7a  pop        edi
0044be7b  pop        esi
0044be7c  pop        ebp
0044be7d  pop        ebx
0044be7e  xor        ecx, esp
0044be80  call       0x46c932

// block 0044be85
0044be85  add        esp, 0x10c
0044be8b  ret        8
