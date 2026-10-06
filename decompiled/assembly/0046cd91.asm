
// block 0046cd91
0046cd91  mov        edi, edi
0046cd93  push       ebp
0046cd94  mov        ebp, esp
0046cd96  push       esi
0046cd97  mov        esi, ecx
0046cd99  call       0x46cd81

// block 0046cd9e
0046cd9e  test       byte ptr [ebp + 8], 1
0046cda2  je         0x46cdab

// block 0046cda4
0046cda4  push       esi
0046cda5  call       0x46ca4f

// block 0046cdaa
0046cdaa  pop        ecx

// block 0046cdab
0046cdab  mov        eax, esi
0046cdad  pop        esi
0046cdae  pop        ebp
0046cdaf  ret        4
