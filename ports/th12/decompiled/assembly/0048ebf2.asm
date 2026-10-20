
// block 0048ebf2
0048ebf2  push       0xc
0048ebf4  push       0x4ab1a8
0048ebf9  call       0x46fcec

// block 0048ebfe
0048ebfe  or         dword ptr [ebp - 0x1c], 0xffffffff
0048ec02  xor        eax, eax
0048ec04  mov        esi, dword ptr [ebp + 8]
0048ec07  xor        edi, edi
0048ec09  cmp        esi, edi
0048ec0b  setne      al
0048ec0e  cmp        eax, edi
0048ec10  jne        0x48ec2f

// block 0048ec12
0048ec12  call       0x46e96f

// block 0048ec17
0048ec17  mov        dword ptr [eax], 0x16
0048ec1d  push       edi
0048ec1e  push       edi
0048ec1f  push       edi
0048ec20  push       edi
0048ec21  push       edi
0048ec22  call       0x470f2d

// block 0048ec27
0048ec27  add        esp, 0x14
0048ec2a  or         eax, 0xffffffff
0048ec2d  jmp        0x48ec3b

// block 0048ec2f
0048ec2f  test       byte ptr [esi + 0xc], 0x40
0048ec33  je         0x48ec41

// block 0048ec35
0048ec35  mov        dword ptr [esi + 0xc], edi

// block 0048ec38
0048ec38  mov        eax, dword ptr [ebp - 0x1c]

// block 0048ec3b
0048ec3b  call       0x46fd31

// block 0048ec40
0048ec40  ret        

// block 0048ec41
0048ec41  push       esi
0048ec42  call       0x47c0fc

// block 0048ec47
0048ec47  pop        ecx
0048ec48  mov        dword ptr [ebp - 4], edi
0048ec4b  push       esi
0048ec4c  call       0x48eb7b

// block 0048ec51
0048ec51  pop        ecx
0048ec52  mov        dword ptr [ebp - 0x1c], eax
0048ec55  mov        dword ptr [ebp - 4], 0xfffffffe
0048ec5c  call       0x48ec66

// block 0048ec61
0048ec61  jmp        0x48ec38
