
// block 0048fbae
0048fbae  mov        edi, edi
0048fbb0  push       ebp
0048fbb1  mov        ebp, esp
0048fbb3  push       ecx
0048fbb4  stmxcsr    dword ptr [ebp - 4]
0048fbb8  and        dword ptr [ebp - 4], 0xffffffc0
0048fbbc  ldmxcsr    dword ptr [ebp - 4]
0048fbc0  mov        cl, byte ptr [ebp - 4]
0048fbc3  xor        eax, eax
0048fbc5  test       cl, 0x3f
0048fbc8  je         0x48fbfc

// block 0048fbca
0048fbca  test       cl, 1
0048fbcd  je         0x48fbd2

// block 0048fbcf
0048fbcf  push       0x10
0048fbd1  pop        eax

// block 0048fbd2
0048fbd2  test       cl, 4
0048fbd5  je         0x48fbda

// block 0048fbd7
0048fbd7  or         eax, 8

// block 0048fbda
0048fbda  test       cl, 8
0048fbdd  je         0x48fbe2

// block 0048fbdf
0048fbdf  or         eax, 4

// block 0048fbe2
0048fbe2  test       cl, 0x10
0048fbe5  je         0x48fbea

// block 0048fbe7
0048fbe7  or         eax, 2

// block 0048fbea
0048fbea  test       cl, 0x20
0048fbed  je         0x48fbf2

// block 0048fbef
0048fbef  or         eax, 1

// block 0048fbf2
0048fbf2  test       cl, 2
0048fbf5  je         0x48fbfc

// block 0048fbf7
0048fbf7  or         eax, 0x80000

// block 0048fbfc
0048fbfc  leave      
0048fbfd  ret        
