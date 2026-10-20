
// block 0042e9b0
0042e9b0  push       ecx
0042e9b1  push       ebx
0042e9b2  push       esi
0042e9b3  push       edi
0042e9b4  mov        edi, dword ptr [0x4b44f8]
0042e9ba  mov        ebx, 0x4a0770
0042e9bf  mov        ecx, 1
0042e9c4  call       0x45fe60

// block 0042e9c9
0042e9c9  mov        dword ptr [edi + 0x4e8], eax
0042e9cf  test       eax, eax
0042e9d1  je         0x42ea5d

// block 0042e9d7
0042e9d7  mov        eax, dword ptr [edi + 0xc]
0042e9da  or         dword ptr [eax + 4], 2
0042e9de  push       0x18fc4
0042e9e3  mov        dword ptr [edi + 0x4ec], 1
0042e9ed  call       0x46c9ea

// block 0042e9f2
0042e9f2  add        esp, 4
0042e9f5  test       eax, eax
0042e9f7  je         0x42ea04

// block 0042e9f9
0042e9f9  mov        esi, eax
0042e9fb  call       0x401000

// block 0042ea00
0042ea00  mov        esi, eax
0042ea02  jmp        0x42ea06

// block 0042ea04
0042ea04  xor        esi, esi

// block 0042ea06
0042ea06  mov        eax, esi
0042ea08  call       0x401090

// block 0042ea0d
0042ea0d  test       eax, eax
0042ea0f  je         0x42ea26

// block 0042ea11
0042ea11  test       esi, esi
0042ea13  je         0x42ea2a

// block 0042ea15
0042ea15  push       esi
0042ea16  call       0x401220

// block 0042ea1b
0042ea1b  push       esi
0042ea1c  call       0x46ca4f

// block 0042ea21
0042ea21  add        esp, 4
0042ea24  jmp        0x42ea2a

// block 0042ea26
0042ea26  test       esi, esi
0042ea28  jne        0x42ea3e

// block 0042ea2a
0042ea2a  push       0x4a0778
0042ea2f  mov        ecx, 0x4b0ec8
0042ea34  call       0x464220

// block 0042ea39
0042ea39  add        esp, 4
0042ea3c  jmp        0x42ea5d

// block 0042ea3e
0042ea3e  mov        ebx, 0x4a07a0
0042ea43  xor        ecx, ecx
0042ea45  mov        dword ptr [edi + 0x4f0], 1
0042ea4f  call       0x45fe60

// block 0042ea54
0042ea54  mov        dword ptr [0x4cee70], eax
0042ea59  test       eax, eax
0042ea5b  jne        0x42ea75

// block 0042ea5d
0042ea5d  mov        dword ptr [0x4cee40], 3
0042ea67  mov        edi, dword ptr [edi + 8]
0042ea6a  or         dword ptr [edi + 4], 2
0042ea6e  pop        edi
0042ea6f  pop        esi
0042ea70  xor        eax, eax
0042ea72  pop        ebx
0042ea73  pop        ecx
0042ea74  ret        

// block 0042ea75
0042ea75  push       0
0042ea77  push       0
0042ea79  mov        eax, 0x4a07ac
0042ea7e  call       0x463c10

// block 0042ea83
0042ea83  mov        dword ptr [0x4d0e6c], eax
0042ea88  test       eax, eax
0042ea8a  jne        0x42ea9e

// block 0042ea8c
0042ea8c  push       0x4a07c0
0042ea91  mov        ecx, 0x4b0ec8
0042ea96  call       0x464220

// block 0042ea9b
0042ea9b  add        esp, 4

// block 0042ea9e
0042ea9e  mov        esi, 0x4cf4e8
0042eaa3  call       0x453cd0

// block 0042eaa8
0042eaa8  push       0x4a07e8
0042eaad  call       0x463df0

// block 0042eab2
0042eab2  test       eax, eax
0042eab4  je         0x42eaec

// block 0042eab6
0042eab6  test       byte ptr [0x4ceae8], 0x10
0042eabd  jne        0x42eac8

// block 0042eabf
0042eabf  mov        eax, esi
0042eac1  call       0x4537b0

// block 0042eac6
0042eac6  jmp        0x42eaec

// block 0042eac8
0042eac8  mov        eax, dword ptr [0x4a07e8]
0042eacd  mov        ecx, dword ptr [0x4a07ec]
0042ead3  mov        dx, word ptr [0x4a07f0]
0042eada  mov        dword ptr [0x4d4654], eax
0042eadf  mov        dword ptr [0x4d4658], ecx
0042eae5  mov        word ptr [0x4d465c], dx

// block 0042eaec
0042eaec  call       0x42e8f0

// block 0042eaf1
0042eaf1  mov        edi, dword ptr [edi + 8]
0042eaf4  or         dword ptr [edi + 4], 2
0042eaf8  call       0x43d050

// block 0042eafd
0042eafd  pop        edi
0042eafe  pop        esi
0042eaff  xor        eax, eax
0042eb01  pop        ebx
0042eb02  pop        ecx
0042eb03  ret        
