
// block 0048f7aa
0048f7aa  mov        edi, edi
0048f7ac  push       ebp
0048f7ad  mov        ebp, esp
0048f7af  mov        ecx, dword ptr [ebp + 8]
0048f7b2  xor        eax, eax
0048f7b4  test       cl, 1
0048f7b7  je         0x48f7bc

// block 0048f7b9
0048f7b9  push       0x10
0048f7bb  pop        eax

// block 0048f7bc
0048f7bc  test       cl, 4
0048f7bf  je         0x48f7c4

// block 0048f7c1
0048f7c1  or         eax, 8

// block 0048f7c4
0048f7c4  test       cl, 8
0048f7c7  je         0x48f7cc

// block 0048f7c9
0048f7c9  or         eax, 4

// block 0048f7cc
0048f7cc  test       cl, 0x10
0048f7cf  je         0x48f7d4

// block 0048f7d1
0048f7d1  or         eax, 2

// block 0048f7d4
0048f7d4  test       cl, 0x20
0048f7d7  je         0x48f7dc

// block 0048f7d9
0048f7d9  or         eax, 1

// block 0048f7dc
0048f7dc  test       cl, 2
0048f7df  je         0x48f7e6

// block 0048f7e1
0048f7e1  or         eax, 0x80000

// block 0048f7e6
0048f7e6  push       ebx
0048f7e7  movzx      edx, cx
0048f7ea  push       esi
0048f7eb  mov        ecx, edx
0048f7ed  mov        esi, 0xc00
0048f7f2  and        ecx, esi
0048f7f4  push       edi
0048f7f5  mov        edi, 0x300
0048f7fa  mov        ebx, 0x200
0048f7ff  je         0x48f822

// block 0048f801
0048f801  cmp        ecx, 0x400
0048f807  je         0x48f81d

// block 0048f809
0048f809  cmp        ecx, 0x800
0048f80f  je         0x48f819

// block 0048f811
0048f811  cmp        ecx, esi
0048f813  jne        0x48f822

// block 0048f815
0048f815  or         eax, edi
0048f817  jmp        0x48f822

// block 0048f819
0048f819  or         eax, ebx
0048f81b  jmp        0x48f822

// block 0048f81d
0048f81d  or         eax, 0x100

// block 0048f822
0048f822  and        edx, edi
0048f824  je         0x48f831

// block 0048f826
0048f826  cmp        edx, ebx
0048f828  jne        0x48f836

// block 0048f82a
0048f82a  or         eax, 0x10000
0048f82f  jmp        0x48f836

// block 0048f831
0048f831  or         eax, 0x20000

// block 0048f836
0048f836  test       dword ptr [ebp + 8], 0x1000
0048f83d  pop        edi
0048f83e  pop        esi
0048f83f  pop        ebx
0048f840  je         0x48f847

// block 0048f842
0048f842  or         eax, 0x40000

// block 0048f847
0048f847  pop        ebp
0048f848  ret        
