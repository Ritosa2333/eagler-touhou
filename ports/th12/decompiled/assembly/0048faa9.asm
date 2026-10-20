
// block 0048faa9
0048faa9  mov        edi, edi
0048faab  push       ebp
0048faac  mov        ebp, esp
0048faae  push       ecx
0048faaf  push       ecx
0048fab0  push       esi
0048fab1  wait       
0048fab2  fnstsw     word ptr [ebp - 4]
0048fab5  mov        al, byte ptr [ebp - 4]
0048fab8  xor        edx, edx
0048faba  mov        esi, 0x80000
0048fabf  test       al, 0x3f
0048fac1  je         0x48faec

// block 0048fac3
0048fac3  test       al, 1
0048fac5  je         0x48faca

// block 0048fac7
0048fac7  push       0x10
0048fac9  pop        edx

// block 0048faca
0048faca  test       al, 4
0048facc  je         0x48fad1

// block 0048face
0048face  or         edx, 8

// block 0048fad1
0048fad1  test       al, 8
0048fad3  je         0x48fad8

// block 0048fad5
0048fad5  or         edx, 4

// block 0048fad8
0048fad8  test       al, 0x10
0048fada  je         0x48fadf

// block 0048fadc
0048fadc  or         edx, 2

// block 0048fadf
0048fadf  test       al, 0x20
0048fae1  je         0x48fae6

// block 0048fae3
0048fae3  or         edx, 1

// block 0048fae6
0048fae6  test       al, 2
0048fae8  je         0x48faec

// block 0048faea
0048faea  or         edx, esi

// block 0048faec
0048faec  cmp        dword ptr [0x4d52dc], 0
0048faf3  je         0x48fb36

// block 0048faf5
0048faf5  stmxcsr    dword ptr [ebp - 8]
0048faf9  mov        cl, byte ptr [ebp - 8]
0048fafc  xor        eax, eax
0048fafe  test       cl, 0x3f
0048fb01  je         0x48fb32

// block 0048fb03
0048fb03  test       cl, 1
0048fb06  je         0x48fb0b

// block 0048fb08
0048fb08  push       0x10
0048fb0a  pop        eax

// block 0048fb0b
0048fb0b  test       cl, 4
0048fb0e  je         0x48fb13

// block 0048fb10
0048fb10  or         eax, 8

// block 0048fb13
0048fb13  test       cl, 8
0048fb16  je         0x48fb1b

// block 0048fb18
0048fb18  or         eax, 4

// block 0048fb1b
0048fb1b  test       cl, 0x10
0048fb1e  je         0x48fb23

// block 0048fb20
0048fb20  or         eax, 2

// block 0048fb23
0048fb23  test       cl, 0x20
0048fb26  je         0x48fb2b

// block 0048fb28
0048fb28  or         eax, 1

// block 0048fb2b
0048fb2b  test       cl, 2
0048fb2e  je         0x48fb32

// block 0048fb30
0048fb30  or         eax, esi

// block 0048fb32
0048fb32  or         eax, edx
0048fb34  jmp        0x48fb38

// block 0048fb36
0048fb36  mov        eax, edx

// block 0048fb38
0048fb38  pop        esi
0048fb39  leave      
0048fb3a  ret        
