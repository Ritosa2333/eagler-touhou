
// block 00424b50
00424b50  push       esi
00424b51  push       edi

// block 00424b52
00424b52  mov        cl, byte ptr [eax]
00424b54  cmp        cl, 0x20
00424b57  je         0x424b68

// block 00424b59
00424b59  cmp        cl, 9
00424b5c  je         0x424b68

// block 00424b5e
00424b5e  cmp        cl, 0xa
00424b61  je         0x424b68

// block 00424b63
00424b63  cmp        cl, 0xd
00424b66  jne        0x424b86

// block 00424b68
00424b68  mov        ecx, eax
00424b6a  lea        esi, [ecx + 1]
00424b6d  lea        ecx, [ecx]

// block 00424b70
00424b70  mov        dl, byte ptr [ecx]
00424b72  inc        ecx
00424b73  test       dl, dl
00424b75  jne        0x424b70

// block 00424b77
00424b77  sub        ecx, esi
00424b79  test       ecx, ecx
00424b7b  jle        0x424b52

// block 00424b7d
00424b7d  lea        esi, [eax + 1]
00424b80  mov        edi, eax

// block 00424b82
00424b82  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00424b84
00424b84  jmp        0x424b52

// block 00424b86
00424b86  mov        ecx, eax
00424b88  lea        esi, [ecx + 1]
00424b8b  jmp        0x424b90

// block 00424b90
00424b90  mov        dl, byte ptr [ecx]
00424b92  inc        ecx
00424b93  test       dl, dl
00424b95  jne        0x424b90

// block 00424b97
00424b97  sub        ecx, esi
00424b99  lea        edx, [ecx - 1]
00424b9c  pop        edi
00424b9d  pop        esi
00424b9e  test       edx, edx
00424ba0  jl         0x424bc3

// block 00424ba2
00424ba2  mov        cl, byte ptr [edx + eax]
00424ba5  cmp        cl, 0x20
00424ba8  je         0x424bb9

// block 00424baa
00424baa  cmp        cl, 9
00424bad  je         0x424bb9

// block 00424baf
00424baf  cmp        cl, 0xa
00424bb2  je         0x424bb9

// block 00424bb4
00424bb4  cmp        cl, 0xd
00424bb7  jne        0x424bc3

// block 00424bb9
00424bb9  sub        edx, 1
00424bbc  mov        byte ptr [edx + eax + 1], 0
00424bc1  jns        0x424ba2

// block 00424bc3
00424bc3  ret        
