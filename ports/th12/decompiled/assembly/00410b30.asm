
// block 00410b30
00410b30  push       ebp
00410b31  mov        ebp, dword ptr [0x4b43d8]
00410b37  push       esi
00410b38  mov        byte ptr [0x4d4f38], 0
00410b3f  mov        ecx, eax

// block 00410b41
00410b41  mov        dl, byte ptr [eax]
00410b43  inc        eax
00410b44  test       dl, dl
00410b46  jne        0x410b41

// block 00410b48
00410b48  push       edi
00410b49  mov        edi, 0x4d4f38
00410b4e  sub        eax, ecx
00410b50  mov        esi, ecx
00410b52  dec        edi

// block 00410b53
00410b53  mov        cl, byte ptr [edi + 1]
00410b56  inc        edi
00410b57  test       cl, cl
00410b59  jne        0x410b53

// block 00410b5b
00410b5b  mov        ecx, eax
00410b5d  shr        ecx, 2

// block 00410b60
00410b60  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00410b62
00410b62  mov        ecx, eax
00410b64  push       0
00410b66  and        ecx, 3
00410b69  push       0
00410b6b  mov        eax, 0x4d4f38

// block 00410b70
00410b70  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00410b72
00410b72  call       0x463c10

// block 00410b77
00410b77  mov        esi, eax
00410b79  mov        eax, dword ptr [ebp + 0x14]
00410b7c  pop        edi
00410b7d  test       eax, eax
00410b7f  je         0x410b91

// block 00410b81
00410b81  push       eax
00410b82  call       0x46c941

// block 00410b87
00410b87  add        esp, 4
00410b8a  mov        dword ptr [ebp + 0x14], 0

// block 00410b91
00410b91  mov        dword ptr [ebp + 0x14], esi
00410b94  mov        eax, esi
00410b96  pop        esi
00410b97  pop        ebp
00410b98  ret        
