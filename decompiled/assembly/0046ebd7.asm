
// block 0046ebd7
0046ebd7  push       0xc
0046ebd9  push       0x4aab20
0046ebde  call       0x46fcec

// block 0046ebe3
0046ebe3  xor        edi, edi
0046ebe5  inc        edi
0046ebe6  mov        dword ptr [ebp - 0x1c], edi
0046ebe9  xor        ebx, ebx
0046ebeb  cmp        dword ptr [0x4b3c04], ebx
0046ebf1  jne        0x46ec0b

// block 0046ebf3
0046ebf3  call       0x47315e

// block 0046ebf8
0046ebf8  push       0x1e
0046ebfa  call       0x472f8d

// block 0046ebff
0046ebff  push       0xff
0046ec04  call       0x472c61

// block 0046ec0b
0046ec0b  mov        esi, dword ptr [ebp + 8]
0046ec0e  lea        esi, [esi*8 + 0x4ad2b8]
0046ec15  cmp        dword ptr [esi], ebx
0046ec17  je         0x46ec1d

// block 0046ec19
0046ec19  mov        eax, edi
0046ec1b  jmp        0x46ec8b

// block 0046ec1d
0046ec1d  push       0x18
0046ec1f  call       0x4777ce

// block 0046ec24
0046ec24  pop        ecx
0046ec25  mov        edi, eax
0046ec27  cmp        edi, ebx
0046ec29  jne        0x46ec3a

// block 0046ec2b
0046ec2b  call       0x46e96f

// block 0046ec30
0046ec30  mov        dword ptr [eax], 0xc
0046ec36  xor        eax, eax
0046ec38  jmp        0x46ec8b

// block 0046ec3a
0046ec3a  push       0xa
0046ec3c  call       0x46ec9a

// block 0046ec41
0046ec41  pop        ecx
0046ec42  mov        dword ptr [ebp - 4], ebx
0046ec45  cmp        dword ptr [esi], ebx
0046ec47  jne        0x46ec75

// block 0046ec49
0046ec49  push       0xfa0
0046ec4e  push       edi
0046ec4f  call       0x47b416

// block 0046ec54
0046ec54  pop        ecx
0046ec55  pop        ecx
0046ec56  test       eax, eax
0046ec58  jne        0x46ec71

// block 0046ec5a
0046ec5a  push       edi
0046ec5b  call       0x46c941

// block 0046ec60
0046ec60  pop        ecx
0046ec61  call       0x46e96f

// block 0046ec66
0046ec66  mov        dword ptr [eax], 0xc
0046ec6c  mov        dword ptr [ebp - 0x1c], ebx
0046ec6f  jmp        0x46ec7c

// block 0046ec71
0046ec71  mov        dword ptr [esi], edi
0046ec73  jmp        0x46ec7c

// block 0046ec75
0046ec75  push       edi
0046ec76  call       0x46c941

// block 0046ec7b
0046ec7b  pop        ecx

// block 0046ec7c
0046ec7c  mov        dword ptr [ebp - 4], 0xfffffffe
0046ec83  call       0x46ec91

// block 0046ec88
0046ec88  mov        eax, dword ptr [ebp - 0x1c]

// block 0046ec8b
0046ec8b  call       0x46fd31

// block 0046ec90
0046ec90  ret        
