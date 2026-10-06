
// block 0042f4b0
0042f4b0  sub        esp, 0x88
0042f4b6  mov        eax, dword ptr [0x4ad138]
0042f4bb  xor        eax, esp
0042f4bd  mov        dword ptr [esp + 0x84], eax
0042f4c4  push       edi
0042f4c5  mov        eax, 0x4a0910
0042f4ca  mov        ecx, 0x4d4c90
0042f4cf  call       0x44b610

// block 0042f4d4
0042f4d4  test       al, al
0042f4d6  je         0x42f533

// block 0042f4d8
0042f4d8  push       0x62
0042f4da  push       0x100
0042f4df  lea        eax, [esp + 0x10]
0042f4e3  push       0x4a091c
0042f4e8  push       eax
0042f4e9  call       0x46cbbb

// block 0042f4ee
0042f4ee  add        esp, 0x10
0042f4f1  push       0
0042f4f3  lea        ecx, [esp + 8]
0042f4f7  push       ecx
0042f4f8  lea        eax, [esp + 0x10]
0042f4fc  call       0x463c10

// block 0042f501
0042f501  mov        edx, dword ptr [esp + 4]
0042f505  mov        dword ptr [0x4cf28c], eax
0042f50a  mov        dword ptr [0x4cf288], edx
0042f510  test       eax, eax
0042f512  jne        0x42f51b

// block 0042f514
0042f514  push       0x4a092c
0042f519  jmp        0x42f538

// block 0042f51b
0042f51b  xor        eax, eax
0042f51d  pop        edi
0042f51e  mov        ecx, dword ptr [esp + 0x84]
0042f525  xor        ecx, esp
0042f527  call       0x46c932

// block 0042f52c
0042f52c  add        esp, 0x88
0042f532  ret        

// block 0042f533
0042f533  push       0x4a0954

// block 0042f538
0042f538  mov        edi, 0x4b0ec8
0042f53d  call       0x464300

// block 0042f542
0042f542  mov        ecx, dword ptr [esp + 0x8c]
0042f549  add        esp, 4
0042f54c  pop        edi
0042f54d  xor        ecx, esp
0042f54f  or         eax, 0xffffffff
0042f552  call       0x46c932

// block 0042f557
0042f557  add        esp, 0x88
0042f55d  ret        
