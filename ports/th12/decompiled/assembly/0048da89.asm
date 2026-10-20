
// block 0048da89
0048da89  mov        edi, edi
0048da8b  push       ebp
0048da8c  mov        ebp, esp
0048da8e  cmp        dword ptr [ebp + 0x14], 0xa
0048da92  mov        eax, dword ptr [ebp + 8]
0048da95  jne        0x48daa1

// block 0048da97
0048da97  test       eax, eax
0048da99  jge        0x48daa1

// block 0048da9b
0048da9b  push       1
0048da9d  push       0xa
0048da9f  jmp        0x48daa6

// block 0048daa1
0048daa1  push       0
0048daa3  push       dword ptr [ebp + 0x14]

// block 0048daa6
0048daa6  push       dword ptr [ebp + 0x10]
0048daa9  mov        ecx, dword ptr [ebp + 0xc]
0048daac  call       0x48d9ac

// block 0048dab1
0048dab1  pop        ebp
0048dab2  ret        
