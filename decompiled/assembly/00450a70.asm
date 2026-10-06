
// block 00450a70
00450a70  sub        esp, 0x30
00450a73  xor        eax, eax
00450a75  push       ebp
00450a76  push       esi
00450a77  push       edi
00450a78  push       4
00450a7a  mov        dword ptr [esp + 0x14], eax
00450a7e  mov        dword ptr [esp + 0x18], eax
00450a82  mov        dword ptr [esp + 0x1c], eax
00450a86  mov        dword ptr [esp + 0x20], eax
00450a8a  mov        dword ptr [esp + 0x24], eax
00450a8e  mov        dword ptr [esp + 0x28], eax
00450a92  mov        dword ptr [esp + 0x2c], eax
00450a96  mov        dword ptr [esp + 0x30], eax
00450a9a  mov        dword ptr [esp + 0x34], eax
00450a9e  mov        dword ptr [esp + 0x38], eax
00450aa2  call       dword ptr [0x498010]

// block 00450aa8
00450aa8  push       0x7f00
00450aad  push       0
00450aaf  mov        dword ptr [esp + 0x34], eax
00450ab3  call       dword ptr [0x49827c]

// block 00450ab9
00450ab9  mov        dword ptr [esp + 0x28], eax
00450abd  lea        eax, [esp + 0x10]
00450ac1  push       eax
00450ac2  mov        dword ptr [esp + 0x24], ebx
00450ac6  mov        dword ptr [esp + 0x18], 0x44fe00
00450ace  mov        dword ptr [0x4cf3fc], 1
00450ad8  mov        dword ptr [0x4cf400], 0
00450ae2  mov        dword ptr [esp + 0x38], 0x4a2694
00450aea  call       dword ptr [0x498210]

// block 00450af0
00450af0  movzx      ecx, byte ptr [0x4ceacd]
00450af7  mov        eax, dword ptr [0x4cf428]
00450afc  add        ecx, ecx
00450afe  add        ecx, ecx
00450b00  xor        ecx, eax
00450b02  mov        edx, 0xc
00450b07  and        ecx, edx
00450b09  xor        eax, ecx
00450b0b  test       dl, al
00450b0d  mov        ecx, 0
00450b12  setne      cl
00450b15  cmp        byte ptr [0x4ceace], 0
00450b1c  mov        dword ptr [0x4ce9fc], ecx
00450b22  jne        0x450b32

// block 00450b24
00450b24  cmp        byte ptr [0x4cead3], 2
00450b2b  jne        0x450b32

// block 00450b2d
00450b2d  or         eax, 0x10
00450b30  jmp        0x450b35

// block 00450b32
00450b32  and        eax, 0xffffffef

// block 00450b35
00450b35  xor        edi, edi
00450b37  mov        esi, 0xf
00450b3c  mov        ebp, 8
00450b41  mov        dword ptr [0x4cf428], eax
00450b46  mov        dword ptr [0x4cf46c], esi
00450b4c  mov        dword ptr [0x4cf470], esi
00450b52  mov        dword ptr [0x4cf474], edi
00450b58  mov        dword ptr [0x4cf478], edx
00450b5e  mov        dword ptr [0x4cf47c], edx
00450b64  mov        dword ptr [0x4cf480], edi
00450b6a  mov        dword ptr [0x4cf484], edx
00450b70  mov        dword ptr [0x4cf488], edx
00450b76  mov        dword ptr [0x4cf48c], edi
00450b7c  mov        dword ptr [0x4cf490], ebp
00450b82  mov        dword ptr [0x4cf494], ebp
00450b88  mov        dword ptr [0x4cf498], edi
00450b8e  mov        dword ptr [0x4cf468], edi
00450b94  cmp        ecx, edi
00450b96  jne        0x450bc3

// block 00450b98
00450b98  push       edi
00450b99  push       ebx
00450b9a  push       edi
00450b9b  push       edi
00450b9c  push       0x1e0
00450ba1  push       0x280
00450ba6  push       edi
00450ba7  push       edi
00450ba8  push       0x90000000
00450bad  push       0x4a269c
00450bb2  push       0x4a2694
00450bb7  push       edi
00450bb8  call       dword ptr [0x498250]

// block 00450bbe
00450bbe  jmp        0x450c4d

// block 00450bc3
00450bc3  mov        edi, dword ptr [0x498278]
00450bc9  shr        eax, 2
00450bcc  and        eax, 3
00450bcf  push       7
00450bd1  cmp        eax, 3
00450bd4  jne        0x450beb

// block 00450bd6
00450bd6  call       edi

// block 00450bd8
00450bd8  push       ebp
00450bd9  lea        esi, [eax + eax + 0x500]
00450be0  call       edi

// block 00450be2
00450be2  lea        ebp, [eax + eax + 0x3c0]
00450be9  jmp        0x450c18

// block 00450beb
00450beb  cmp        eax, 2
00450bee  jne        0x450c05

// block 00450bf0
00450bf0  call       edi

// block 00450bf2
00450bf2  push       ebp
00450bf3  lea        esi, [eax + eax + 0x3c0]
00450bfa  call       edi

// block 00450bfc
00450bfc  lea        ebp, [eax + eax + 0x2d0]
00450c03  jmp        0x450c18

// block 00450c05
00450c05  call       edi

// block 00450c07
00450c07  push       ebp
00450c08  lea        esi, [eax + eax + 0x280]
00450c0f  call       edi

// block 00450c11
00450c11  lea        ebp, [eax + eax + 0x1e0]

// block 00450c18
00450c18  push       4
00450c1a  call       edi

// block 00450c1c
00450c1c  mov        edx, dword ptr [0x4ceadc]
00450c22  push       0
00450c24  push       ebx
00450c25  push       0
00450c27  push       0
00450c29  add        eax, ebp
00450c2b  push       eax
00450c2c  mov        eax, dword ptr [0x4cead8]
00450c31  push       esi
00450c32  push       edx
00450c33  push       eax
00450c34  push       0x100b0000
00450c39  push       0x4a269c
00450c3e  push       0x4a2694
00450c43  push       0
00450c45  call       dword ptr [0x498250]

// block 00450c4b
00450c4b  xor        edi, edi

// block 00450c4d
00450c4d  push       0x4ce8f8
00450c52  push       eax
00450c53  mov        dword ptr [0x4cf3f0], eax
00450c58  call       dword ptr [0x498230]

// block 00450c5e
00450c5e  mov        eax, dword ptr [0x4cf3f0]
00450c63  mov        dword ptr [0x4ce940], eax
00450c68  cmp        eax, edi
00450c6a  jne        0x450c78

// block 00450c6c
00450c6c  mov        eax, 1
00450c71  pop        edi
00450c72  pop        esi
00450c73  pop        ebp
00450c74  add        esp, 0x30
00450c77  ret        

// block 00450c78
00450c78  mov        esi, dword ptr [0x498238]
00450c7e  push       edi
00450c7f  push       0xf020
00450c84  push       0x112
00450c89  push       eax
00450c8a  call       esi

// block 00450c8c
00450c8c  push       0x10
00450c8e  call       dword ptr [0x498084]

// block 00450c94
00450c94  mov        ecx, dword ptr [0x4cf3f0]
00450c9a  push       edi
00450c9b  push       0xf120
00450ca0  push       0x112
00450ca5  push       ecx
00450ca6  call       esi

// block 00450ca8
00450ca8  pop        edi
00450ca9  pop        esi
00450caa  xor        eax, eax
00450cac  pop        ebp
00450cad  add        esp, 0x30
00450cb0  ret        
