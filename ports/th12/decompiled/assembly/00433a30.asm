
// block 00433a30
00433a30  push       edi
00433a31  mov        edi, 7
00433a36  cmp        dword ptr [0x4b0cb0], edi
00433a3c  jne        0x433a60

// block 00433a3e
00433a3e  cmp        dword ptr [esi + 0x1f4], 0
00433a45  je         0x433a60

// block 00433a47
00433a47  mov        eax, 8
00433a4c  mov        dword ptr [0x4b452c], 0x4aedf0
00433a56  mov        dword ptr [0x4b0cb0], eax
00433a5b  mov        dword ptr [0x4b0cb4], eax

// block 00433a60
00433a60  mov        eax, dword ptr [0x4b0c90]
00433a65  mov        ecx, dword ptr [0x4b0c94]
00433a6b  lea        edx, [ecx + eax*2]
00433a6e  mov        eax, dword ptr [0x4b451c]
00433a73  imul       edx, edx, 0x45f4
00433a79  lea        eax, [edx + eax + 8]
00433a7d  push       eax
00433a7e  call       0x431b90

// block 00433a83
00433a83  cmp        dword ptr [0x4b0cb0], edi
00433a89  jne        0x433aaa

// block 00433a8b
00433a8b  cmp        dword ptr [esi + 0x1f4], 0
00433a92  je         0x433aaa

// block 00433a94
00433a94  mov        dword ptr [0x4b452c], 0x4aedb0
00433a9e  mov        dword ptr [0x4b0cb0], edi
00433aa4  mov        dword ptr [0x4b0cb4], edi

// block 00433aaa
00433aaa  test       eax, eax
00433aac  jl         0x433ba0

// block 00433ab2
00433ab2  mov        edx, 1
00433ab7  mov        dword ptr [esi + 0x40], 0x19
00433abe  mov        dword ptr [esi + 0x108], edx
00433ac4  mov        ecx, dword ptr [esi + 0x40]
00433ac7  test       ecx, ecx
00433ac9  je         0x433ad2

// block 00433acb
00433acb  cmp        eax, ecx
00433acd  jl         0x433ad2

// block 00433acf
00433acf  lea        eax, [ecx - 1]

// block 00433ad2
00433ad2  push       ebp
00433ad3  lea        ebp, [esi + 0x110]
00433ad9  mov        dword ptr [esi + 0x38], eax
00433adc  mov        eax, dword ptr [ebp + 8]
00433adf  test       eax, eax
00433ae1  je         0x433af2

// block 00433ae3
00433ae3  jg         0x433aeb

// block 00433ae5
00433ae5  dec        eax
00433ae6  mov        dword ptr [ebp], eax
00433ae9  jmp        0x433af9

// block 00433aeb
00433aeb  xor        eax, eax
00433aed  mov        dword ptr [ebp], eax
00433af0  jmp        0x433af9

// block 00433af2
00433af2  mov        dword ptr [ebp], 0

// block 00433af9
00433af9  mov        ecx, dword ptr [0x4b451c]
00433aff  lea        edi, [esi + 0x2cc]
00433b05  add        ecx, 0x1e9c0
00433b0b  mov        eax, edi
00433b0d  mov        dword ptr [esi + 0x118], 0x5b
00433b17  mov        dword ptr [esi + 0x1e0], edx
00433b1d  sub        eax, ecx
00433b1f  nop        

// block 00433b20
00433b20  mov        dl, byte ptr [ecx]
00433b22  mov        byte ptr [eax + ecx], dl
00433b25  inc        ecx
00433b26  test       dl, dl
00433b28  jne        0x433b20

// block 00433b2a
00433b2a  mov        dword ptr [esi + 0x1f0], 0
00433b34  mov        edx, 0x4a0ee4
00433b39  mov        ecx, edi
00433b3b  jmp        0x433b40

// block 00433b40
00433b40  mov        al, byte ptr [ecx]
00433b42  cmp        al, byte ptr [edx]
00433b44  jne        0x433b60

// block 00433b46
00433b46  test       al, al
00433b48  je         0x433b5c

// block 00433b4a
00433b4a  mov        al, byte ptr [ecx + 1]
00433b4d  cmp        al, byte ptr [edx + 1]
00433b50  jne        0x433b60

// block 00433b52
00433b52  add        ecx, 2
00433b55  add        edx, 2
00433b58  test       al, al
00433b5a  jne        0x433b40

// block 00433b5c
00433b5c  xor        eax, eax
00433b5e  jmp        0x433b65

// block 00433b60
00433b60  sbb        eax, eax
00433b62  sbb        eax, -1

// block 00433b65
00433b65  test       eax, eax
00433b67  je         0x433b72

// block 00433b69
00433b69  push       -1
00433b6b  mov        eax, ebp
00433b6d  call       0x464970

// block 00433b72
00433b72  mov        eax, 8
00433b77  mov        cl, 0x20
00433b79  pop        ebp
00433b7a  lea        ebx, [ebx]

// block 00433b80
00433b80  cmp        byte ptr [esi + eax + 0x2cb], cl
00433b87  jne        0x433b8e

// block 00433b89
00433b89  dec        eax
00433b8a  test       eax, eax
00433b8c  jg         0x433b80

// block 00433b8e
00433b8e  mov        dword ptr [esi + 0x1f0], eax
00433b94  mov        dword ptr [esi + 0x1f8], 0
00433b9e  pop        edi
00433b9f  ret        

// block 00433ba0
00433ba0  mov        dword ptr [esi + 0x1f8], 1
00433baa  pop        edi
00433bab  ret        
