
// block 00463af0
00463af0  sub        esp, 8
00463af3  push       ebx
00463af4  push       ebp
00463af5  mov        ebp, dword ptr [esp + 0x20]
00463af9  mov        bl, al
00463afb  push       esi
00463afc  push       edi
00463afd  mov        edi, dword ptr [esp + 0x20]
00463b01  mov        eax, edi
00463b03  cdq        
00463b04  idiv       ebp
00463b06  mov        eax, ebp
00463b08  mov        dword ptr [esp + 0x28], edi
00463b0c  mov        ecx, edx
00463b0e  cdq        
00463b0f  and        edx, 3
00463b12  add        eax, edx
00463b14  sar        eax, 2
00463b17  xor        edx, edx
00463b19  cmp        ecx, eax
00463b1b  mov        eax, dword ptr [esp + 0x2c]
00463b1f  setge      dl
00463b22  dec        edx
00463b23  and        edx, ecx
00463b25  cmp        eax, edi
00463b27  mov        dword ptr [esp + 0x20], edx
00463b2b  jg         0x463b31

// block 00463b2d
00463b2d  mov        dword ptr [esp + 0x28], eax

// block 00463b31
00463b31  mov        eax, dword ptr [esp + 0x28]
00463b35  mov        esi, dword ptr [esp + 0x1c]
00463b39  push       eax
00463b3a  call       0x46d04a

// block 00463b3f
00463b3f  add        esp, 4
00463b42  mov        dword ptr [esp + 0x14], eax
00463b46  mov        dword ptr [esp + 0x10], eax
00463b4a  test       eax, eax
00463b4c  jne        0x463b5a

// block 00463b4e
00463b4e  pop        edi
00463b4f  mov        eax, esi
00463b51  pop        esi
00463b52  pop        ebp
00463b53  pop        ebx
00463b54  add        esp, 8
00463b57  ret        0x14

// block 00463b5a
00463b5a  mov        edx, dword ptr [esp + 0x28]
00463b5e  mov        ecx, edi
00463b60  and        ecx, 1
00463b63  add        ecx, dword ptr [esp + 0x20]
00463b67  push       edx
00463b68  sub        edi, ecx
00463b6a  mov        ecx, esi
00463b6c  push       ecx
00463b6d  push       eax
00463b6e  mov        dword ptr [esp + 0x2c], edi
00463b72  call       0x47a330

// block 00463b77
00463b77  add        esp, 0xc
00463b7a  test       edi, edi
00463b7c  jle        0x463bf1

// block 00463b7e
00463b7e  jmp        0x463b84

// block 00463b80
00463b80  mov        edi, dword ptr [esp + 0x20]

// block 00463b84
00463b84  cmp        dword ptr [esp + 0x2c], 0
00463b89  jle        0x463bf1

// block 00463b8b
00463b8b  cmp        edi, ebp
00463b8d  jge        0x463b91

// block 00463b8f
00463b8f  mov        ebp, edi

// block 00463b91
00463b91  mov        edx, dword ptr [esp + 0x10]
00463b95  lea        ecx, [edx + ebp]
00463b98  lea        eax, [ebp + 1]
00463b9b  cdq        
00463b9c  sub        eax, edx
00463b9e  sar        eax, 1
00463ba0  lea        edi, [ecx - 1]
00463ba3  test       eax, eax
00463ba5  jle        0x463bba

// block 00463ba7
00463ba7  mov        dl, byte ptr [edi]
00463ba9  xor        dl, bl
00463bab  add        bl, byte ptr [esp + 0x24]
00463baf  mov        byte ptr [esi], dl
00463bb1  dec        eax
00463bb2  sub        edi, 2
00463bb5  inc        esi
00463bb6  test       eax, eax
00463bb8  jg         0x463ba7

// block 00463bba
00463bba  mov        eax, ebp
00463bbc  cdq        
00463bbd  sub        eax, edx
00463bbf  sar        eax, 1
00463bc1  lea        edi, [ecx - 2]
00463bc4  test       eax, eax
00463bc6  jle        0x463bdb

// block 00463bc8
00463bc8  mov        dl, byte ptr [edi]
00463bca  xor        dl, bl
00463bcc  add        bl, byte ptr [esp + 0x24]
00463bd0  mov        byte ptr [esi], dl
00463bd2  dec        eax
00463bd3  sub        edi, 2
00463bd6  inc        esi
00463bd7  test       eax, eax
00463bd9  jg         0x463bc8

// block 00463bdb
00463bdb  mov        eax, dword ptr [esp + 0x20]
00463bdf  sub        dword ptr [esp + 0x2c], ebp
00463be3  sub        eax, ebp
00463be5  mov        dword ptr [esp + 0x20], eax
00463be9  mov        dword ptr [esp + 0x10], ecx
00463bed  test       eax, eax
00463bef  jg         0x463b80

// block 00463bf1
00463bf1  mov        eax, dword ptr [esp + 0x14]
00463bf5  push       eax
00463bf6  call       0x46c941

// block 00463bfb
00463bfb  mov        eax, dword ptr [esp + 0x20]
00463bff  add        esp, 4
00463c02  pop        edi
00463c03  pop        esi
00463c04  pop        ebp
00463c05  pop        ebx
00463c06  add        esp, 8
00463c09  ret        0x14
