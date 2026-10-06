
// block 00402a40
00402a40 mov        eax, dword ptr [0x4b0cb0]
00402a45 sub        esp, 0xc
00402a48 push       ebp
00402a49 mov        ebp, dword ptr [esp + 0x14]
00402a4d push       edi
00402a4e mov        dword ptr [ebp + 0x35d4], eax
00402a54 mov        eax, dword ptr [esp + 0x1c]
00402a58 push       ebp
00402a59 mov        dword ptr [0x4b43c0], ebp
00402a5f call       0x4044f0

// block 00402a64
00402a64 test       eax, eax
00402a66 je         0x402a85

// block 00402a68
00402a68 push       0x49f4d8
00402a6d mov        edi, 0x4b0ec8
00402a72 call       0x464300

// block 00402a77
00402a77 add        esp, 4
00402a7a pop        edi
00402a7b or         eax, 0xffffffff
00402a7e pop        ebp
00402a7f add        esp, 0xc
00402a82 ret        8

// block 00402a85
00402a85 fldz       
00402a87 lea        eax, [ebp + 0x360c]
00402a8d fst        dword ptr [esp + 8]
00402a91 push       ebx
00402a92 fst        dword ptr [esp + 0x10]
00402a96 mov        edi, eax
00402a98 fld        dword ptr [0x4a43b8]
00402a9e mov        edx, dword ptr [esp + 0x10]
00402aa2 fstp       dword ptr [esp + 0x14]
00402aa6 push       esi
00402aa7 mov        ecx, 0x46
00402aac mov        esi, 0x4ced1c

// block 00402ab1
00402ab1 rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00402ab3
00402ab3 mov        ecx, dword ptr [esp + 0x10]
00402ab7 mov        dword ptr [eax], ecx
00402ab9 mov        ecx, dword ptr [esp + 0x18]
00402abd fst        dword ptr [esp + 0x10]
00402ac1 fld        dword ptr [0x4a43b4]
00402ac7 mov        dword ptr [eax + 4], edx
00402aca mov        edx, dword ptr [esp + 0x10]
00402ace fstp       dword ptr [esp + 0x14]
00402ad2 fld        dword ptr [0x4a43b0]
00402ad8 mov        dword ptr [eax + 8], ecx
00402adb mov        eax, dword ptr [esp + 0x14]
00402adf fstp       dword ptr [esp + 0x18]
00402ae3 mov        ecx, dword ptr [esp + 0x18]
00402ae7 fst        dword ptr [esp + 0x10]
00402aeb mov        dword ptr [ebp + 0x3618], edx
00402af1 fld1       
00402af3 mov        edx, dword ptr [esp + 0x10]
00402af7 fstp       dword ptr [esp + 0x14]
00402afb mov        dword ptr [ebp + 0x361c], eax
00402b01 mov        eax, dword ptr [esp + 0x14]
00402b05 mov        dword ptr [ebp + 0x3620], ecx
00402b0b fst        dword ptr [esp + 0x18]
00402b0f mov        ecx, dword ptr [esp + 0x18]
00402b13 fst        dword ptr [esp + 0x10]
00402b17 mov        dword ptr [ebp + 0x3624], edx
00402b1d mov        edx, dword ptr [esp + 0x10]
00402b21 fst        dword ptr [esp + 0x14]
00402b25 mov        dword ptr [ebp + 0x3628], eax
00402b2b fstp       dword ptr [esp + 0x18]
00402b2f mov        eax, dword ptr [esp + 0x14]
00402b33 fld        dword ptr [0x4a43ac]
00402b39 mov        dword ptr [ebp + 0x362c], ecx
00402b3f fstp       dword ptr [ebp + 0x276c]
00402b45 mov        ecx, dword ptr [esp + 0x18]
00402b49 mov        dword ptr [ebp + 0x3648], edx
00402b4f mov        dword ptr [ebp + 0x364c], eax
00402b55 push       0x24
00402b57 mov        dword ptr [ebp + 0x3650], ecx
00402b5d call       0x46c9ea

// block 00402b62
00402b62 xor        edi, edi
00402b64 add        esp, 4
00402b67 cmp        eax, edi
00402b69 je         0x402b87

// block 00402b6b
00402b6b and        dword ptr [eax + 4], 0xfffffffe
00402b6f mov        dword ptr [eax + 8], edi
00402b72 mov        dword ptr [eax + 0xc], edi
00402b75 mov        dword ptr [eax + 0x10], edi
00402b78 mov        dword ptr [eax], edi
00402b7a mov        dword ptr [eax + 0x14], eax
00402b7d mov        dword ptr [eax + 0x18], edi
00402b80 mov        dword ptr [eax + 0x1c], edi
00402b83 mov        esi, eax
00402b85 jmp        0x402b89

// block 00402b87
00402b87 xor        esi, esi

// block 00402b89
00402b89 mov        edx, dword ptr [esi + 4]
00402b8c and        edx, 0xfffffffd
00402b8f or         edx, 1
00402b92 mov        ebx, 0xc
00402b97 mov        dword ptr [esi + 8], 0x403ec0
00402b9e mov        dword ptr [esi + 0xc], edi
00402ba1 mov        dword ptr [esi + 0x10], edi
00402ba4 mov        dword ptr [esi + 0x20], ebp
00402ba7 mov        dword ptr [esi + 4], edx
00402baa call       0x462380

// block 00402baf
00402baf push       0x24
00402bb1 mov        dword ptr [ebp + 8], esi
00402bb4 call       0x46c9ea

// block 00402bb9
00402bb9 add        esp, 4
00402bbc cmp        eax, edi
00402bbe je         0x402bdc

// block 00402bc0
00402bc0 and        dword ptr [eax + 4], 0xfffffffe
00402bc4 mov        dword ptr [eax + 8], edi
00402bc7 mov        dword ptr [eax + 0xc], edi
00402bca mov        dword ptr [eax + 0x10], edi
00402bcd mov        dword ptr [eax], edi
00402bcf mov        dword ptr [eax + 0x14], eax
00402bd2 mov        dword ptr [eax + 0x18], edi
00402bd5 mov        dword ptr [eax + 0x1c], edi
00402bd8 mov        esi, eax
00402bda jmp        0x402bde

// block 00402bdc
00402bdc xor        esi, esi

// block 00402bde
00402bde mov        eax, dword ptr [esi + 4]
00402be1 and        eax, 0xfffffffd
00402be4 or         eax, 1
00402be7 mov        ebx, 2
00402bec mov        dword ptr [esi + 8], 0x403ed0
00402bf3 mov        dword ptr [esi + 0xc], edi
00402bf6 mov        dword ptr [esi + 0x10], edi
00402bf9 mov        dword ptr [esi + 0x20], ebp
00402bfc mov        dword ptr [esi + 4], eax
00402bff call       0x462420

// block 00402c04
00402c04 push       0x24
00402c06 mov        dword ptr [ebp + 0xc], esi
00402c09 call       0x46c9ea

// block 00402c0e
00402c0e add        esp, 4
00402c11 cmp        eax, edi
00402c13 je         0x402c31

// block 00402c15
00402c15 and        dword ptr [eax + 4], 0xfffffffe
00402c19 mov        dword ptr [eax + 8], edi
00402c1c mov        dword ptr [eax + 0xc], edi
00402c1f mov        dword ptr [eax + 0x10], edi
00402c22 mov        dword ptr [eax], edi
00402c24 mov        dword ptr [eax + 0x14], eax
00402c27 mov        dword ptr [eax + 0x18], edi
00402c2a mov        dword ptr [eax + 0x1c], edi
00402c2d mov        esi, eax
00402c2f jmp        0x402c33

// block 00402c31
00402c31 xor        esi, esi

// block 00402c33
00402c33 mov        ecx, dword ptr [esi + 4]
00402c36 and        ecx, 0xfffffffd
00402c39 or         ecx, 1
00402c3c mov        ebx, 5
00402c41 mov        dword ptr [esi + 8], 0x403ee0
00402c48 mov        dword ptr [esi + 0xc], edi
00402c4b mov        dword ptr [esi + 0x10], edi
00402c4e mov        dword ptr [esi + 0x20], ebp
00402c51 mov        dword ptr [esi + 4], ecx
00402c54 call       0x462420

// block 00402c59
00402c59 mov        dword ptr [ebp + 0x3600], esi
00402c5f mov        dword ptr [ebp + 0x35d8], edi
00402c65 mov        eax, dword ptr [ebp + 0x48]
00402c68 pop        esi
00402c69 pop        ebx
00402c6a test       al, 1
00402c6c jne        0x402c8a

// block 00402c6e
00402c6e fldz       
00402c70 or         eax, 1
00402c73 fstp       dword ptr [ebp + 0x40]
00402c76 mov        dword ptr [ebp + 0x3c], edi
00402c79 mov        dword ptr [ebp + 0x38], 0xfff0bdc1
00402c80 mov        dword ptr [ebp + 0x44], 0x4b2ed0
00402c87 mov        dword ptr [ebp + 0x48], eax

// block 00402c8a
00402c8a fldz       
00402c8c mov        dword ptr [ebp + 0x3c], edi
00402c8f fstp       dword ptr [ebp + 0x40]
00402c92 mov        dword ptr [ebp + 0x38], 0xffffffff
00402c99 or         dword ptr [ebp + 0x35bc], 1
00402ca0 mov        dword ptr [ebp + 0x94], edi
00402ca6 mov        dword ptr [ebp + 0xe0], edi
00402cac pop        edi
00402cad xor        eax, eax
00402caf pop        ebp
00402cb0 add        esp, 0xc
00402cb3 ret        8
