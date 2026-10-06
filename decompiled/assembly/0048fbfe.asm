
// block 0048fbfe
0048fbfe  mov        edi, edi
0048fc00  push       ebp
0048fc01  mov        ebp, esp
0048fc03  push       ecx
0048fc04  push       ecx
0048fc05  and        dword ptr [ebp + 0xc], 0x308031f
0048fc0c  stmxcsr    dword ptr [ebp - 8]
0048fc10  mov        ecx, dword ptr [ebp - 8]
0048fc13  xor        eax, eax
0048fc15  test       cl, cl
0048fc17  jns        0x48fc1c

// block 0048fc19
0048fc19  push       0x10
0048fc1b  pop        eax

// block 0048fc1c
0048fc1c  push       ebx
0048fc1d  mov        ebx, 0x200
0048fc22  push       esi
0048fc23  push       edi
0048fc24  test       ebx, ecx
0048fc26  je         0x48fc2b

// block 0048fc28
0048fc28  or         eax, 8

// block 0048fc2b
0048fc2b  test       ecx, 0x400
0048fc31  je         0x48fc36

// block 0048fc33
0048fc33  or         eax, 4

// block 0048fc36
0048fc36  test       ecx, 0x800
0048fc3c  je         0x48fc41

// block 0048fc3e
0048fc3e  or         eax, 2

// block 0048fc41
0048fc41  test       ecx, 0x1000
0048fc47  je         0x48fc4c

// block 0048fc49
0048fc49  or         eax, 1

// block 0048fc4c
0048fc4c  mov        esi, 0x100
0048fc51  test       esi, ecx
0048fc53  je         0x48fc5a

// block 0048fc55
0048fc55  or         eax, 0x80000

// block 0048fc5a
0048fc5a  mov        edx, ecx
0048fc5c  mov        edi, 0x6000
0048fc61  and        edx, edi
0048fc63  je         0x48fc86

// block 0048fc65
0048fc65  cmp        edx, 0x2000
0048fc6b  je         0x48fc84

// block 0048fc6d
0048fc6d  cmp        edx, 0x4000
0048fc73  je         0x48fc80

// block 0048fc75
0048fc75  cmp        edx, edi
0048fc77  jne        0x48fc86

// block 0048fc79
0048fc79  or         eax, 0x300
0048fc7e  jmp        0x48fc86

// block 0048fc80
0048fc80  or         eax, ebx
0048fc82  jmp        0x48fc86

// block 0048fc84
0048fc84  or         eax, esi

// block 0048fc86
0048fc86  mov        esi, 0x8040
0048fc8b  and        ecx, esi
0048fc8d  sub        ecx, 0x40
0048fc90  je         0x48fcad

// block 0048fc92
0048fc92  sub        ecx, 0x7fc0
0048fc98  je         0x48fca6

// block 0048fc9a
0048fc9a  sub        ecx, 0x40
0048fc9d  jne        0x48fcb2

// block 0048fc9f
0048fc9f  or         eax, 0x1000000
0048fca4  jmp        0x48fcb2

// block 0048fca6
0048fca6  or         eax, 0x3000000
0048fcab  jmp        0x48fcb2

// block 0048fcad
0048fcad  or         eax, 0x2000000

// block 0048fcb2
0048fcb2  mov        edx, dword ptr [ebp + 0xc]
0048fcb5  mov        ecx, dword ptr [ebp + 8]
0048fcb8  and        ecx, dword ptr [ebp + 0xc]
0048fcbb  not        edx
0048fcbd  and        edx, eax
0048fcbf  or         edx, ecx
0048fcc1  cmp        edx, eax
0048fcc3  je         0x48fd6f

// block 0048fcc9
0048fcc9  call       0x48f9ba

// block 0048fcce
0048fcce  push       eax
0048fccf  mov        dword ptr [ebp - 4], eax
0048fcd2  call       0x491220

// block 0048fcd7
0048fcd7  pop        ecx
0048fcd8  stmxcsr    dword ptr [ebp - 4]
0048fcdc  mov        edx, dword ptr [ebp - 4]
0048fcdf  xor        eax, eax
0048fce1  test       dl, dl
0048fce3  jns        0x48fce8

// block 0048fce5
0048fce5  push       0x10
0048fce7  pop        eax

// block 0048fce8
0048fce8  test       ebx, edx
0048fcea  je         0x48fcef

// block 0048fcec
0048fcec  or         eax, 8

// block 0048fcef
0048fcef  test       edx, 0x400
0048fcf5  je         0x48fcfa

// block 0048fcf7
0048fcf7  or         eax, 4

// block 0048fcfa
0048fcfa  test       edx, 0x800
0048fd00  je         0x48fd05

// block 0048fd02
0048fd02  or         eax, 2

// block 0048fd05
0048fd05  test       edx, 0x1000
0048fd0b  je         0x48fd10

// block 0048fd0d
0048fd0d  or         eax, 1

// block 0048fd10
0048fd10  mov        ebx, 0x100
0048fd15  test       ebx, edx
0048fd17  je         0x48fd1e

// block 0048fd19
0048fd19  or         eax, 0x80000

// block 0048fd1e
0048fd1e  mov        ecx, edx
0048fd20  and        ecx, edi
0048fd22  je         0x48fd48

// block 0048fd24
0048fd24  cmp        ecx, 0x2000
0048fd2a  je         0x48fd46

// block 0048fd2c
0048fd2c  cmp        ecx, 0x4000
0048fd32  je         0x48fd3f

// block 0048fd34
0048fd34  cmp        ecx, edi
0048fd36  jne        0x48fd48

// block 0048fd38
0048fd38  or         eax, 0x300
0048fd3d  jmp        0x48fd48

// block 0048fd3f
0048fd3f  or         eax, 0x200
0048fd44  jmp        0x48fd48

// block 0048fd46
0048fd46  or         eax, ebx

// block 0048fd48
0048fd48  and        edx, esi
0048fd4a  sub        edx, 0x40
0048fd4d  je         0x48fd6a

// block 0048fd4f
0048fd4f  sub        edx, 0x7fc0
0048fd55  je         0x48fd63

// block 0048fd57
0048fd57  sub        edx, 0x40
0048fd5a  jne        0x48fd6f

// block 0048fd5c
0048fd5c  or         eax, 0x1000000
0048fd61  jmp        0x48fd6f

// block 0048fd63
0048fd63  or         eax, 0x3000000
0048fd68  jmp        0x48fd6f

// block 0048fd6a
0048fd6a  or         eax, 0x2000000

// block 0048fd6f
0048fd6f  pop        edi
0048fd70  pop        esi
0048fd71  pop        ebx
0048fd72  leave      
0048fd73  ret        
