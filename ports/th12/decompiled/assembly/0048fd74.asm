
// block 0048fd74
0048fd74  mov        edi, edi
0048fd76  push       ebp
0048fd77  mov        ebp, esp
0048fd79  mov        edx, dword ptr [ebp + 8]
0048fd7c  test       edx, edx
0048fd7e  je         0x48fdbc

// block 0048fd80
0048fd80  wait       
0048fd81  fnstsw     word ptr [ebp + 8]
0048fd84  mov        al, byte ptr [ebp + 8]
0048fd87  xor        ecx, ecx
0048fd89  test       al, 0x3f
0048fd8b  je         0x48fdba

// block 0048fd8d
0048fd8d  test       al, 1
0048fd8f  je         0x48fd94

// block 0048fd91
0048fd91  push       0x10
0048fd93  pop        ecx

// block 0048fd94
0048fd94  test       al, 4
0048fd96  je         0x48fd9b

// block 0048fd98
0048fd98  or         ecx, 8

// block 0048fd9b
0048fd9b  test       al, 8
0048fd9d  je         0x48fda2

// block 0048fd9f
0048fd9f  or         ecx, 4

// block 0048fda2
0048fda2  test       al, 0x10
0048fda4  je         0x48fda9

// block 0048fda6
0048fda6  or         ecx, 2

// block 0048fda9
0048fda9  test       al, 0x20
0048fdab  je         0x48fdb0

// block 0048fdad
0048fdad  or         ecx, 1

// block 0048fdb0
0048fdb0  test       al, 2
0048fdb2  je         0x48fdba

// block 0048fdb4
0048fdb4  or         ecx, 0x80000

// block 0048fdba
0048fdba  mov        dword ptr [edx], ecx

// block 0048fdbc
0048fdbc  push       esi
0048fdbd  mov        esi, dword ptr [ebp + 0xc]
0048fdc0  test       esi, esi
0048fdc2  je         0x48fdcb

// block 0048fdc4
0048fdc4  call       0x48fb73

// block 0048fdc9
0048fdc9  mov        dword ptr [esi], eax

// block 0048fdcb
0048fdcb  pop        esi
0048fdcc  pop        ebp
0048fdcd  ret        
