
// block 00434900
00434900  sub        esp, 0x58
00434903  mov        eax, dword ptr [0x4ad138]
00434908  xor        eax, esp
0043490a  mov        dword ptr [esp + 0x54], eax
0043490e  push       ebp
0043490f  mov        ebp, dword ptr [esp + 0x60]
00434913  mov        eax, dword ptr [ebp + 4]
00434916  add        eax, -0x14
00434919  cmp        eax, 6
0043491c  ja         0x43557a

// block 00434922
00434922  push       ebx
00434923  push       esi
00434924  push       edi
00434925  jmp        dword ptr [eax*4 + 0x43558c]

// block 0043492c
0043492c  cmp        dword ptr [ebp + 0x14], 0xa
00434930  jl         0x435577

// block 00434936
00434936  test       dword ptr [0x4d48c4], 0x80001
00434940  je         0x435577

// block 00434946
00434946  mov        edx, 7
0043494b  call       0x453d90

// block 00434950
00434950  mov        eax, dword ptr [ebp + 0x1e8]
00434956  push       eax
00434957  mov        ebx, 2
0043495c  mov        dword ptr [ebp + 4], 0x15
00434963  call       0x461970

// block 00434968
00434968  push       0
0043496a  lea        eax, [ebp + 0x10]
0043496d  call       0x4067e0

// block 00434972
00434972  pop        edi
00434973  pop        esi
00434974  pop        ebx
00434975  pop        ebp
00434976  mov        ecx, dword ptr [esp + 0x54]
0043497a  xor        ecx, esp
0043497c  call       0x46c932

// block 00434981
00434981  add        esp, 0x58
00434984  ret        4

// block 00434987
00434987  cmp        dword ptr [ebp + 0x14], 0xa
0043498b  jl         0x435577

// block 00434991
00434991  call       0x431b30

// block 00434996
00434996  test       byte ptr [0x4b0ce0], 0x10
0043499d  je         0x434a71

// block 004349a3
004349a3  mov        edx, dword ptr [0x4b0c90]
004349a9  mov        ecx, dword ptr [0x4b0c94]
004349af  mov        eax, dword ptr [0x4b0ca8]
004349b4  lea        ecx, [ecx + edx*2]
004349b7  imul       ecx, ecx, 0x45f4
004349bd  add        ecx, dword ptr [0x4b451c]
004349c3  lea        edx, [eax + eax*2]
004349c6  mov        eax, dword ptr [0x4b0cb0]
004349cb  lea        edx, [eax + edx*2]
004349ce  lea        eax, [ecx + edx*8 + 0x5a4]
004349d5  mov        ecx, dword ptr [0x4b0c44]
004349db  cmp        ecx, dword ptr [eax]
004349dd  jle        0x4349e1

// block 004349df
004349df  mov        dword ptr [eax], ecx

// block 004349e1
004349e1  mov        eax, dword ptr [ebp + 0x1f4]
004349e7  mov        edi, dword ptr [ebp + 0x2dc]
004349ed  neg        eax
004349ef  sbb        eax, eax
004349f1  add        eax, 0x75
004349f4  push       eax
004349f5  lea        ecx, [esp + 0x24]
004349f9  push       ecx
004349fa  call       0x4357c0

// block 004349ff
004349ff  mov        edx, dword ptr [eax]
00434a01  mov        ebx, 3
00434a06  mov        eax, ebx
00434a08  mov        dword ptr [ebp + 0x1e8], edx
00434a0e  mov        dword ptr [ebp + 4], 0x16
00434a15  mov        dword ptr [ebp + 0x40], ebx
00434a18  mov        dword ptr [ebp + 0x108], 1
00434a22  test       eax, eax
00434a24  je         0x434a35

// block 00434a26
00434a26  jg         0x434a2e

// block 00434a28
00434a28  dec        eax
00434a29  mov        dword ptr [ebp + 0x38], eax
00434a2c  jmp        0x434a3c

// block 00434a2e
00434a2e  xor        eax, eax
00434a30  mov        dword ptr [ebp + 0x38], eax
00434a33  jmp        0x434a3c

// block 00434a35
00434a35  mov        dword ptr [ebp + 0x38], 0

// block 00434a3c
00434a3c  mov        eax, dword ptr [ebp + 0x1e8]
00434a42  push       eax
00434a43  call       0x461970

// block 00434a48
00434a48  movzx      ebx, word ptr [ebp + 0x38]
00434a4c  mov        ecx, dword ptr [ebp + 0x1e8]
00434a52  add        bx, 7
00434a56  push       ecx
00434a57  call       0x461970

// block 00434a5c
00434a5c  pop        edi
00434a5d  pop        esi
00434a5e  pop        ebx
00434a5f  pop        ebp
00434a60  mov        ecx, dword ptr [esp + 0x54]
00434a64  xor        ecx, esp
00434a66  call       0x46c932

// block 00434a6b
00434a6b  add        esp, 0x58
00434a6e  ret        4

// block 00434a71
00434a71  mov        esi, ebp
00434a73  call       0x433a30

// block 00434a78
00434a78  push       0
00434a7a  lea        eax, [ebp + 0x10]
00434a7d  call       0x4067e0

// block 00434a82
00434a82  cmp        dword ptr [ebp + 0x1f8], 0
00434a89  jne        0x434ab2

// block 00434a8b
00434a8b  lea        eax, [ebp + 0x1e8]
00434a91  mov        dword ptr [ebp + 4], 0x19
00434a98  call       0x461d80

// block 00434a9d
00434a9d  pop        edi
00434a9e  pop        esi
00434a9f  pop        ebx
00434aa0  pop        ebp
00434aa1  mov        ecx, dword ptr [esp + 0x54]
00434aa5  xor        ecx, esp
00434aa7  call       0x46c932

// block 00434aac
00434aac  add        esp, 0x58
00434aaf  ret        4

// block 00434ab2
00434ab2  mov        edi, dword ptr [ebp + 0x2dc]
00434ab8  push       0x6f
00434aba  lea        edx, [esp + 0x18]
00434abe  push       edx
00434abf  call       0x4357c0

// block 00434ac4
00434ac4  mov        eax, dword ptr [eax]
00434ac6  mov        ebx, 3
00434acb  mov        dword ptr [ebp + 0x1e8], eax
00434ad1  mov        eax, ebx
00434ad3  mov        dword ptr [ebp + 4], 0xe
00434ada  mov        dword ptr [ebp + 0x40], ebx
00434add  mov        dword ptr [ebp + 0x108], 1
00434ae7  test       eax, eax
00434ae9  je         0x434afa

// block 00434aeb
00434aeb  jg         0x434af3

// block 00434aed
00434aed  dec        eax
00434aee  mov        dword ptr [ebp + 0x38], eax
00434af1  jmp        0x434b01

// block 00434af3
00434af3  xor        eax, eax
00434af5  mov        dword ptr [ebp + 0x38], eax
00434af8  jmp        0x434b01

// block 00434afa
00434afa  mov        dword ptr [ebp + 0x38], 0

// block 00434b01
00434b01  mov        ecx, dword ptr [ebp + 0x1e8]
00434b07  push       ecx
00434b08  call       0x461970

// block 00434b0d
00434b0d  movzx      ebx, word ptr [ebp + 0x38]
00434b11  mov        edx, dword ptr [ebp + 0x1e8]
00434b17  add        bx, 7
00434b1b  push       edx
00434b1c  call       0x461970

// block 00434b21
00434b21  pop        edi
00434b22  pop        esi
00434b23  pop        ebx
00434b24  pop        ebp
00434b25  mov        ecx, dword ptr [esp + 0x54]
00434b29  xor        ecx, esp
00434b2b  call       0x46c932

// block 00434b30
00434b30  add        esp, 0x58
00434b33  ret        4

// block 00434b36
00434b36  mov        eax, dword ptr [ebp + 0x38]
00434b39  lea        esi, [ebp + 0x38]
00434b3c  mov        bl, 0x10
00434b3e  mov        dword ptr [esi + 4], eax
00434b41  test       byte ptr [0x4d48c4], bl
00434b47  jne        0x434b51

// block 00434b49
00434b49  test       byte ptr [0x4d48c0], bl
00434b4f  je         0x434b5a

// block 00434b51
00434b51  push       -1
00434b53  mov        eax, esi
00434b55  call       0x464970

// block 00434b5a
00434b5a  test       byte ptr [0x4d48c4], 0x20
00434b61  jne        0x434b6c

// block 00434b63
00434b63  test       byte ptr [0x4d48c0], 0x20
00434b6a  je         0x434b75

// block 00434b6c
00434b6c  push       1
00434b6e  mov        eax, esi
00434b70  call       0x464970

// block 00434b75
00434b75  mov        ecx, dword ptr [esi + 4]
00434b78  cmp        ecx, dword ptr [esi]
00434b7a  je         0x434b99

// block 00434b7c
00434b7c  movzx      ebx, word ptr [esi]
00434b7f  mov        edx, dword ptr [ebp + 0x1e8]
00434b85  add        bx, 7
00434b89  push       edx
00434b8a  call       0x461970

// block 00434b8f
00434b8f  mov        edx, 0xa
00434b94  call       0x453d90

// block 00434b99
00434b99  test       dword ptr [0x4d48c4], 0x80001
00434ba3  je         0x434c10

// block 00434ba5
00434ba5  mov        eax, dword ptr [ebp + 0x38]
00434ba8  lea        esi, [ebp + 0x1e8]
00434bae  lea        edi, [eax + 0x6b]
00434bb1  lea        ebx, [esp + 0x18]
00434bb5  call       0x462060

// block 00434bba
00434bba  mov        eax, dword ptr [eax]
00434bbc  push       eax
00434bbd  mov        ebx, 6
00434bc2  call       0x461970

// block 00434bc7
00434bc7  lea        edx, [ebx + 1]
00434bca  call       0x453d90

// block 00434bcf
00434bcf  lea        edi, [ebp + 0x10]
00434bd2  mov        ebx, 0x1a
00434bd7  push       0
00434bd9  mov        eax, edi
00434bdb  mov        dword ptr [ebp + 4], ebx
00434bde  call       0x4067e0

// block 00434be3
00434be3  call       0x431b30

// block 00434be8
00434be8  mov        eax, dword ptr [ebp + 0x38]
00434beb  sub        eax, 0
00434bee  je         0x434bfa

// block 00434bf0
00434bf0  sub        eax, 1
00434bf3  je         0x434c4e

// block 00434bf5
00434bf5  sub        eax, 1
00434bf8  jne        0x434c10

// block 00434bfa
00434bfa  push       0
00434bfc  mov        eax, edi
00434bfe  mov        dword ptr [ebp + 4], ebx
00434c01  call       0x4067e0

// block 00434c06
00434c06  mov        edx, 7
00434c0b  call       0x453d90

// block 00434c10
00434c10  test       dword ptr [0x4d48c4], 0x102
00434c1a  je         0x435577

// block 00434c20
00434c20  mov        edx, 9
00434c25  call       0x453d90

// block 00434c2a
00434c2a  cmp        dword ptr [ebp + 0x38], 0
00434c2e  je         0x435577

// block 00434c34
00434c34  mov        eax, dword ptr [ebp + 0x40]
00434c37  test       eax, eax
00434c39  je         0x434cd7

// block 00434c3f
00434c3f  jg         0x434cd0

// block 00434c45
00434c45  dec        eax
00434c46  mov        dword ptr [ebp + 0x38], eax
00434c49  jmp        0x434cde

// block 00434c4e
00434c4e  push       0
00434c50  mov        eax, edi
00434c52  mov        dword ptr [ebp + 4], 0x17
00434c59  call       0x4067e0

// block 00434c5e
00434c5e  mov        eax, esi
00434c60  call       0x461d80

// block 00434c65
00434c65  lea        eax, [ebp + 0x38]
00434c68  call       0x464900

// block 00434c6d
00434c6d  mov        dword ptr [ebp + 0x40], 0x19
00434c74  mov        dword ptr [ebp + 0x108], 1
00434c7e  mov        ecx, dword ptr [eax + 8]
00434c81  test       ecx, ecx
00434c83  je         0x434c92

// block 00434c85
00434c85  jg         0x434c8c

// block 00434c87
00434c87  dec        ecx
00434c88  mov        dword ptr [eax], ecx
00434c8a  jmp        0x434c98

// block 00434c8c
00434c8c  xor        ecx, ecx
00434c8e  mov        dword ptr [eax], ecx
00434c90  jmp        0x434c98

// block 00434c92
00434c92  mov        dword ptr [eax], 0

// block 00434c98
00434c98  mov        esi, 1
00434c9d  lea        edi, [ebp + 0x204]

// block 00434ca3
00434ca3  push       esi
00434ca4  lea        ecx, [esp + 0x28]
00434ca8  push       0x4a1024
00434cad  push       ecx
00434cae  call       0x46cbbb

// block 00434cb3
00434cb3  add        esp, 0xc
00434cb6  lea        edx, [esp + 0x24]
00434cba  push       edx
00434cbb  call       0x43b6f0

// block 00434cc0
00434cc0  mov        dword ptr [edi], eax
00434cc2  inc        esi
00434cc3  add        edi, 4
00434cc6  cmp        esi, 0x19
00434cc9  jle        0x434ca3

// block 00434ccb
00434ccb  jmp        0x434c10

// block 00434cd0
00434cd0  xor        eax, eax
00434cd2  mov        dword ptr [ebp + 0x38], eax
00434cd5  jmp        0x434cde

// block 00434cd7
00434cd7  mov        dword ptr [ebp + 0x38], 0

// block 00434cde
00434cde  movzx      ebx, word ptr [ebp + 0x38]
00434ce2  mov        eax, dword ptr [ebp + 0x1e8]
00434ce8  add        bx, 7
00434cec  push       eax
00434ced  call       0x461970

// block 00434cf2
00434cf2  pop        edi
00434cf3  pop        esi
00434cf4  pop        ebx
00434cf5  pop        ebp
00434cf6  mov        ecx, dword ptr [esp + 0x54]
00434cfa  xor        ecx, esp
00434cfc  call       0x46c932

// block 00434d01
00434d01  add        esp, 0x58
00434d04  ret        4

// block 00434d07
00434d07  cmp        dword ptr [ebp + 0x14], 0xa
00434d0b  jl         0x435577

// block 00434d11
00434d11  mov        ecx, dword ptr [ebp + 0x38]
00434d14  lea        esi, [ebp + 0x38]
00434d17  mov        bl, 0x10
00434d19  mov        dword ptr [esi + 4], ecx
00434d1c  test       byte ptr [0x4d48c4], bl
00434d22  jne        0x434d2c

// block 00434d24
00434d24  test       byte ptr [0x4d48c0], bl
00434d2a  je         0x434d35

// block 00434d2c
00434d2c  push       -1
00434d2e  mov        eax, esi
00434d30  call       0x464970

// block 00434d35
00434d35  test       byte ptr [0x4d48c4], 0x20
00434d3c  jne        0x434d47

// block 00434d3e
00434d3e  test       byte ptr [0x4d48c0], 0x20
00434d45  je         0x434d50

// block 00434d47
00434d47  push       1
00434d49  mov        eax, esi
00434d4b  call       0x464970

// block 00434d50
00434d50  mov        edx, dword ptr [esi + 4]
00434d53  cmp        edx, dword ptr [esi]
00434d55  je         0x434d61

// block 00434d57
00434d57  mov        edx, 0xa
00434d5c  call       0x453d90

// block 00434d61
00434d61  mov        eax, dword ptr [0x4d48c4]
00434d66  test       eax, 0x80001
00434d6b  je         0x434eb4

// block 00434d71
00434d71  push       0
00434d73  lea        eax, [ebp + 0x10]
00434d76  mov        dword ptr [ebp + 4], 0x18
00434d7d  call       0x4067e0

// block 00434d82
00434d82  mov        eax, dword ptr [ebp + 0x118]
00434d88  lea        edi, [ebp + 0x110]
00434d8e  test       eax, eax
00434d90  je         0x434d9f

// block 00434d92
00434d92  jg         0x434d99

// block 00434d94
00434d94  dec        eax
00434d95  mov        dword ptr [edi], eax
00434d97  jmp        0x434da5

// block 00434d99
00434d99  xor        eax, eax
00434d9b  mov        dword ptr [edi], eax
00434d9d  jmp        0x434da5

// block 00434d9f
00434d9f  mov        dword ptr [edi], 0

// block 00434da5
00434da5  cmp        dword ptr [ebp + 0x1f4], 0
00434dac  mov        dword ptr [ebp + 0x118], 0x5b
00434db6  mov        dword ptr [ebp + 0x1e0], 1
00434dc0  je         0x434dea

// block 00434dc2
00434dc2  test       byte ptr [0x4b0ce0], bl
00434dc8  jne        0x434dea

// block 00434dca
00434dca  mov        esi, dword ptr [0x4b4518]
00434dd0  mov        eax, dword ptr [esi + 0x1c]
00434dd3  add        esi, 0x1c
00434dd6  add        eax, 0xc
00434dd9  push       eax
00434dda  call       0x46d5b7

// block 00434ddf
00434ddf  mov        ecx, dword ptr [esi]
00434de1  mov        dword ptr [ecx + 0x68], 8
00434de8  jmp        0x434e0a

// block 00434dea
00434dea  mov        esi, dword ptr [0x4b4518]
00434df0  mov        edx, dword ptr [esi + 0x1c]
00434df3  add        esi, 0x1c
00434df6  add        edx, 0xc
00434df9  push       edx
00434dfa  call       0x46d5b7

// block 00434dff
00434dff  mov        eax, dword ptr [esi]
00434e01  mov        ecx, dword ptr [0x4b0cb0]
00434e07  mov        dword ptr [eax + 0x68], ecx

// block 00434e0a
00434e0a  mov        eax, dword ptr [0x4b451c]
00434e0f  lea        esi, [ebp + 0x2cc]
00434e15  add        eax, 0x1e9c0
00434e1a  mov        edx, esi
00434e1c  add        esp, 4
00434e1f  sub        edx, eax

// block 00434e21
00434e21  mov        cl, byte ptr [eax]
00434e23  mov        byte ptr [edx + eax], cl
00434e26  inc        eax
00434e27  test       cl, cl
00434e29  jne        0x434e21

// block 00434e2b
00434e2b  mov        dword ptr [ebp + 0x1f0], 0
00434e35  mov        ecx, 0x4a0ee4
00434e3a  mov        eax, esi
00434e3c  lea        esp, [esp]

// block 00434e40
00434e40  mov        dl, byte ptr [eax]
00434e42  cmp        dl, byte ptr [ecx]
00434e44  jne        0x434e60

// block 00434e46
00434e46  test       dl, dl
00434e48  je         0x434e5c

// block 00434e4a
00434e4a  mov        dl, byte ptr [eax + 1]
00434e4d  cmp        dl, byte ptr [ecx + 1]
00434e50  jne        0x434e60

// block 00434e52
00434e52  add        eax, 2
00434e55  add        ecx, 2
00434e58  test       dl, dl
00434e5a  jne        0x434e40

// block 00434e5c
00434e5c  xor        eax, eax
00434e5e  jmp        0x434e65

// block 00434e60
00434e60  sbb        eax, eax
00434e62  sbb        eax, -1

// block 00434e65
00434e65  test       eax, eax
00434e67  je         0x434e72

// block 00434e69
00434e69  push       -1
00434e6b  mov        eax, edi
00434e6d  call       0x464970

// block 00434e72
00434e72  mov        eax, 8
00434e77  jmp        0x434e80

// block 00434e80
00434e80  cmp        byte ptr [eax + ebp + 0x2cb], 0x20
00434e88  jne        0x434e8f

// block 00434e8a
00434e8a  dec        eax
00434e8b  test       eax, eax
00434e8d  jg         0x434e80

// block 00434e8f
00434e8f  mov        dword ptr [ebp + 0x1f0], eax

// block 00434e95
00434e95  mov        edx, 7
00434e9a  call       0x453d90

// block 00434e9f
00434e9f  pop        edi
00434ea0  pop        esi
00434ea1  pop        ebx
00434ea2  pop        ebp
00434ea3  mov        ecx, dword ptr [esp + 0x54]
00434ea7  xor        ecx, esp
00434ea9  call       0x46c932

// block 00434eae
00434eae  add        esp, 0x58
00434eb1  ret        4

// block 00434eb4
00434eb4  test       eax, 0x102
00434eb9  je         0x435577

// block 00434ebf
00434ebf  lea        eax, [ebp + 0x10]
00434ec2  push       0
00434ec4  mov        dword ptr [ebp + 4], 0x16
00434ecb  mov        dword ptr [esp + 0x14], eax
00434ecf  call       0x4067e0

// block 00434ed4
00434ed4  lea        edi, [ebp + 0x1e8]
00434eda  mov        eax, edi
00434edc  call       0x461d40

// block 00434ee1
00434ee1  mov        eax, esi
00434ee3  call       0x464940

// block 00434ee8
00434ee8  movzx      ebx, word ptr [esi]
00434eeb  mov        edx, dword ptr [edi]
00434eed  add        bx, 7
00434ef1  push       edx
00434ef2  mov        dword ptr [ebp + 0x40], 3
00434ef9  mov        dword ptr [ebp + 0x108], 1
00434f03  call       0x461970

// block 00434f08
00434f08  lea        esi, [ebp + 0x204]
00434f0e  mov        ebx, 0x19

// block 00434f13
00434f13  mov        edi, dword ptr [esi]
00434f15  test       edi, edi
00434f17  je         0x434f28

// block 00434f19
00434f19  push       edi
00434f1a  call       0x43b450

// block 00434f1f
00434f1f  push       edi
00434f20  call       0x46ca4f

// block 00434f25
00434f25  add        esp, 4

// block 00434f28
00434f28  mov        dword ptr [esi], 0
00434f2e  add        esi, 4
00434f31  sub        ebx, 1
00434f34  jne        0x434f13

// block 00434f36
00434f36  mov        eax, dword ptr [esp + 0x10]
00434f3a  push       ebx
00434f3b  mov        dword ptr [ebp + 4], 0x16
00434f42  call       0x4067e0

// block 00434f47
00434f47  lea        edx, [ebx + 9]
00434f4a  call       0x453d90

// block 00434f4f
00434f4f  pop        edi
00434f50  pop        esi
00434f51  pop        ebx
00434f52  pop        ebp
00434f53  mov        ecx, dword ptr [esp + 0x54]
00434f57  xor        ecx, esp
00434f59  call       0x46c932

// block 00434f5e
00434f5e  add        esp, 0x58
00434f61  ret        4

// block 00434f64
00434f64  cmp        dword ptr [ebp + 0x14], 0xa
00434f68  jl         0x435577

// block 00434f6e
00434f6e  mov        eax, dword ptr [ebp + 0x110]
00434f74  lea        esi, [ebp + 0x110]
00434f7a  mov        bl, 0x10
00434f7c  mov        dword ptr [esi + 4], eax
00434f7f  test       byte ptr [0x4d48c4], bl
00434f85  jne        0x434f8f

// block 00434f87
00434f87  test       byte ptr [0x4d48c0], bl
00434f8d  je         0x434f98

// block 00434f8f
00434f8f  push       -0xd
00434f91  mov        eax, esi
00434f93  call       0x464970

// block 00434f98
00434f98  test       byte ptr [0x4d48c4], 0x20
00434f9f  jne        0x434faa

// block 00434fa1
00434fa1  test       byte ptr [0x4d48c0], 0x20
00434fa8  je         0x434fb3

// block 00434faa
00434faa  push       0xd
00434fac  mov        eax, esi
00434fae  call       0x464970

// block 00434fb3
00434fb3  mov        al, 0x40
00434fb5  test       byte ptr [0x4d48c4], al
00434fbb  jne        0x434fc5

// block 00434fbd
00434fbd  test       byte ptr [0x4d48c0], al
00434fc3  je         0x434fec

// block 00434fc5
00434fc5  mov        ecx, dword ptr [esi]
00434fc7  mov        eax, 0x4ec4ec4f
00434fcc  imul       ecx
00434fce  sar        edx, 2
00434fd1  mov        eax, edx
00434fd3  shr        eax, 0x1f
00434fd6  add        eax, edx
00434fd8  imul       eax, eax, 0xd
00434fdb  sub        ecx, eax
00434fdd  mov        eax, esi
00434fdf  je         0x434fe5

// block 00434fe1
00434fe1  push       -1
00434fe3  jmp        0x434fe7

// block 00434fe5
00434fe5  push       0xc

// block 00434fe7
00434fe7  call       0x464970

// block 00434fec
00434fec  mov        al, 0x80
00434fee  mov        ebx, 1
00434ff3  test       byte ptr [0x4d48c4], al
00434ff9  jne        0x435003

// block 00434ffb
00434ffb  test       byte ptr [0x4d48c0], al
00435001  je         0x43502c

// block 00435003
00435003  mov        ecx, dword ptr [esi]
00435005  mov        eax, 0x4ec4ec4f
0043500a  imul       ecx
0043500c  sar        edx, 2
0043500f  mov        eax, edx
00435011  shr        eax, 0x1f
00435014  add        eax, edx
00435016  imul       eax, eax, 0xd
00435019  sub        ecx, eax
0043501b  mov        eax, esi
0043501d  cmp        ecx, 0xc
00435020  je         0x435025

// block 00435022
00435022  push       ebx
00435023  jmp        0x435027

// block 00435025
00435025  push       -0xc

// block 00435027
00435027  call       0x464970

// block 0043502c
0043502c  mov        ecx, dword ptr [esi + 4]
0043502f  cmp        ecx, dword ptr [esi]
00435031  je         0x43503d

// block 00435033
00435033  mov        edx, 0xa
00435038  call       0x453d90

// block 0043503d
0043503d  test       dword ptr [0x4d48c4], 0x80001
00435047  je         0x435091

// block 00435049
00435049  mov        eax, dword ptr [esi]
0043504b  cmp        eax, 0x58
0043504e  jge        0x4350ed

// block 00435054
00435054  mov        ecx, dword ptr [ebp + 0x1f0]
0043505a  cmp        ecx, 8
0043505d  jge        0x4350de

// block 0043505f
0043505f  mov        dl, byte ptr [eax + 0x4a0e88]
00435065  mov        byte ptr [ecx + ebp + 0x2cc], dl

// block 0043506c
0043506c  add        dword ptr [ebp + 0x1f0], ebx
00435072  cmp        dword ptr [ebp + 0x1f0], 8
00435079  jl         0x435087

// block 0043507b
0043507b  mov        ecx, 0x5a
00435080  mov        edx, esi
00435082  call       0x40f790

// block 00435087
00435087  mov        edx, 7
0043508c  call       0x453d90

// block 00435091
00435091  test       dword ptr [0x4d48c4], 0x102
0043509b  je         0x435577

// block 004350a1
004350a1  mov        edx, 9
004350a6  call       0x453d90

// block 004350ab
004350ab  mov        eax, dword ptr [ebp + 0x1f0]
004350b1  test       eax, eax
004350b3  jne        0x4351f5

// block 004350b9
004350b9  push       eax
004350ba  lea        eax, [ebp + 0x10]
004350bd  mov        dword ptr [ebp + 4], 0x17
004350c4  call       0x4067e0

// block 004350c9
004350c9  pop        edi
004350ca  pop        esi
004350cb  pop        ebx
004350cc  pop        ebp
004350cd  mov        ecx, dword ptr [esp + 0x54]
004350d1  xor        ecx, esp
004350d3  call       0x46c932

// block 004350d8
004350d8  add        esp, 0x58
004350db  ret        4

// block 004350de
004350de  mov        al, byte ptr [eax + 0x4a0e88]
004350e4  mov        byte ptr [ecx + ebp + 0x2cb], al
004350eb  jmp        0x435087

// block 004350ed
004350ed  jne        0x435114

// block 004350ef
004350ef  mov        eax, dword ptr [ebp + 0x1f0]
004350f5  cmp        eax, 8
004350f8  jge        0x435107

// block 004350fa
004350fa  mov        byte ptr [eax + ebp + 0x2cc], 0x20
00435102  jmp        0x43506c

// block 00435107
00435107  mov        byte ptr [eax + ebp + 0x2cb], 0x20
0043510f  jmp        0x435087

// block 00435114
00435114  cmp        eax, 0x59
00435117  jne        0x435155

// block 00435119
00435119  mov        eax, dword ptr [ebp + 0x1f0]
0043511f  test       eax, eax
00435121  je         0x435577

// block 00435127
00435127  dec        eax
00435128  mov        dword ptr [ebp + 0x1f0], eax
0043512e  mov        edx, 9
00435133  mov        byte ptr [eax + ebp + 0x2cc], 0x20
0043513b  call       0x453d90

// block 00435140
00435140  pop        edi
00435141  pop        esi
00435142  pop        ebx
00435143  pop        ebp
00435144  mov        ecx, dword ptr [esp + 0x54]
00435148  xor        ecx, esp
0043514a  call       0x46c932

// block 0043514f
0043514f  add        esp, 0x58
00435152  ret        4

// block 00435155
00435155  cmp        eax, 0x5a
00435158  jne        0x435087

// block 0043515e
0043515e  lea        edx, [eax - 0x48]
00435161  call       0x453d90

// block 00435166
00435166  mov        ecx, dword ptr [ebp + 0x38]
00435169  add        ecx, ebx
0043516b  push       ecx
0043516c  lea        edx, [esp + 0x28]
00435170  push       0x4a1024
00435175  push       edx
00435176  call       0x46cbbb

// block 0043517b
0043517b  mov        eax, dword ptr [ebp + 0x38]
0043517e  mov        esi, dword ptr [ebp + eax*4 + 0x204]
00435185  add        esp, 0xc
00435188  call       0x43b7c0

// block 0043518d
0043518d  lea        esi, [ebp + 0x2cc]
00435193  push       0
00435195  mov        edx, esi
00435197  lea        ecx, [esp + 0x28]
0043519b  call       0x43bc10

// block 004351a0
004351a0  mov        edi, dword ptr [ebp + 0x38]
004351a3  lea        ecx, [esp + 0x24]
004351a7  push       ecx
004351a8  call       0x43b6f0

// block 004351ad
004351ad  mov        dword ptr [ebp + edi*4 + 0x204], eax
004351b4  push       0
004351b6  lea        eax, [ebp + 0x10]
004351b9  mov        dword ptr [ebp + 4], 0x17
004351c0  call       0x4067e0

// block 004351c5
004351c5  mov        edx, dword ptr [0x4b451c]
004351cb  add        edx, 0x1e9c0
004351d1  mov        eax, esi
004351d3  sub        edx, esi

// block 004351d5
004351d5  mov        cl, byte ptr [eax]
004351d7  mov        byte ptr [eax + edx], cl
004351da  add        eax, ebx
004351dc  test       cl, cl
004351de  jne        0x4351d5

// block 004351e0
004351e0  pop        edi
004351e1  pop        esi
004351e2  pop        ebx
004351e3  pop        ebp
004351e4  mov        ecx, dword ptr [esp + 0x54]
004351e8  xor        ecx, esp
004351ea  call       0x46c932

// block 004351ef
004351ef  add        esp, 0x58
004351f2  ret        4

// block 004351f5
004351f5  pop        edi
004351f6  dec        eax
004351f7  pop        esi
004351f8  mov        dword ptr [ebp + 0x1f0], eax
004351fe  pop        ebx
004351ff  mov        byte ptr [eax + ebp + 0x2cc], 0x20
00435207  pop        ebp
00435208  mov        ecx, dword ptr [esp + 0x54]
0043520c  xor        ecx, esp
0043520e  call       0x46c932

// block 00435213
00435213  add        esp, 0x58
00435216  ret        4

// block 00435219
00435219  cmp        dword ptr [ebp + 0x14], 0xa
0043521d  jl         0x435577

// block 00435223
00435223  cmp        dword ptr [ebp + 0x1f8], 0
0043522a  jne        0x4352fb

// block 00435230
00435230  mov        edx, dword ptr [ebp + 0x110]
00435236  lea        esi, [ebp + 0x110]
0043523c  mov        bl, 0x10
0043523e  mov        dword ptr [esi + 4], edx
00435241  test       byte ptr [0x4d48c4], bl
00435247  jne        0x435251

// block 00435249
00435249  test       byte ptr [0x4d48c0], bl
0043524f  je         0x43525a

// block 00435251
00435251  push       -0xd
00435253  mov        eax, esi
00435255  call       0x464970

// block 0043525a
0043525a  test       byte ptr [0x4d48c4], 0x20
00435261  jne        0x43526c

// block 00435263
00435263  test       byte ptr [0x4d48c0], 0x20
0043526a  je         0x435275

// block 0043526c
0043526c  push       0xd
0043526e  mov        eax, esi
00435270  call       0x464970

// block 00435275
00435275  mov        al, 0x40
00435277  test       byte ptr [0x4d48c4], al
0043527d  jne        0x435287

// block 0043527f
0043527f  test       byte ptr [0x4d48c0], al
00435285  je         0x4352ae

// block 00435287
00435287  mov        ecx, dword ptr [esi]
00435289  mov        eax, 0x4ec4ec4f
0043528e  imul       ecx
00435290  sar        edx, 2
00435293  mov        eax, edx
00435295  shr        eax, 0x1f
00435298  add        eax, edx
0043529a  imul       eax, eax, 0xd
0043529d  sub        ecx, eax
0043529f  mov        eax, esi
004352a1  je         0x4352a7

// block 004352a3
004352a3  push       -1
004352a5  jmp        0x4352a9

// block 004352a7
004352a7  push       0xc

// block 004352a9
004352a9  call       0x464970

// block 004352ae
004352ae  mov        al, 0x80
004352b0  test       byte ptr [0x4d48c4], al
004352b6  jne        0x4352c0

// block 004352b8
004352b8  test       byte ptr [0x4d48c0], al
004352be  je         0x4352ea

// block 004352c0
004352c0  mov        ecx, dword ptr [esi]
004352c2  mov        eax, 0x4ec4ec4f
004352c7  imul       ecx
004352c9  sar        edx, 2
004352cc  mov        eax, edx
004352ce  shr        eax, 0x1f
004352d1  add        eax, edx
004352d3  imul       eax, eax, 0xd
004352d6  sub        ecx, eax
004352d8  mov        eax, esi
004352da  cmp        ecx, 0xc
004352dd  je         0x4352e3

// block 004352df
004352df  push       1
004352e1  jmp        0x4352e5

// block 004352e3
004352e3  push       -0xc

// block 004352e5
004352e5  call       0x464970

// block 004352ea
004352ea  mov        ecx, dword ptr [esi + 4]
004352ed  cmp        ecx, dword ptr [esi]
004352ef  je         0x4352fb

// block 004352f1
004352f1  mov        edx, 0xa
004352f6  call       0x453d90

// block 004352fb
004352fb  mov        eax, dword ptr [0x4d48c4]
00435300  test       eax, 0x80001
00435305  je         0x435476

// block 0043530b
0043530b  cmp        dword ptr [ebp + 0x1f8], 0
00435312  jne        0x43542c

// block 00435318
00435318  mov        eax, dword ptr [ebp + 0x110]
0043531e  cmp        eax, 0x58
00435321  lea        edx, [ebp + 0x110]
00435327  jge        0x435375

// block 00435329
00435329  mov        ecx, dword ptr [ebp + 0x1f0]
0043532f  cmp        ecx, 8
00435332  jge        0x435363

// block 00435334
00435334  mov        al, byte ptr [eax + 0x4a0e88]
0043533a  mov        byte ptr [ecx + ebp + 0x2cc], al

// block 00435341
00435341  inc        dword ptr [ebp + 0x1f0]
00435347  cmp        dword ptr [ebp + 0x1f0], 8
0043534e  jl         0x434e95

// block 00435354
00435354  mov        ecx, 0x5a
00435359  call       0x40f790

// block 0043535e
0043535e  jmp        0x434e95

// block 00435363
00435363  mov        dl, byte ptr [eax + 0x4a0e88]
00435369  mov        byte ptr [ecx + ebp + 0x2cb], dl
00435370  jmp        0x434e95

// block 00435375
00435375  jne        0x435399

// block 00435377
00435377  mov        eax, dword ptr [ebp + 0x1f0]
0043537d  cmp        eax, 8
00435380  jge        0x43538c

// block 00435382
00435382  mov        byte ptr [eax + ebp + 0x2cc], 0x20
0043538a  jmp        0x435341

// block 0043538c
0043538c  mov        byte ptr [eax + ebp + 0x2cb], 0x20
00435394  jmp        0x434e95

// block 00435399
00435399  cmp        eax, 0x59
0043539c  jne        0x4353c0

// block 0043539e
0043539e  mov        eax, dword ptr [ebp + 0x1f0]
004353a4  test       eax, eax
004353a6  je         0x435577

// block 004353ac
004353ac  dec        eax
004353ad  mov        dword ptr [ebp + 0x1f0], eax
004353b3  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004353bb  jmp        0x434e95

// block 004353c0
004353c0  cmp        eax, 0x5a
004353c3  jne        0x434e95

// block 004353c9
004353c9  mov        ecx, dword ptr [0x4b0ca8]
004353cf  mov        esi, dword ptr [ebp + 0x38]
004353d2  mov        edi, dword ptr [0x4b0c90]
004353d8  lea        ecx, [ecx + ecx*4]
004353db  lea        ecx, [esi + ecx*2]
004353de  lea        esi, [ecx*8]
004353e5  sub        esi, ecx
004353e7  mov        ecx, dword ptr [0x4b0c94]
004353ed  lea        ecx, [ecx + edi*2]
004353f0  mov        edi, dword ptr [0x4b451c]
004353f6  imul       ecx, ecx, 0x45f4
004353fc  lea        eax, [ebp + 0x2cc]
00435402  add        ecx, edi
00435404  mov        edx, eax
00435406  lea        esi, [ecx + esi*4 + 0x1e]
0043540a  lea        ebx, [ebx]

// block 00435410
00435410  mov        cl, byte ptr [edx]
00435412  mov        byte ptr [esi], cl
00435414  inc        edx
00435415  inc        esi
00435416  test       cl, cl
00435418  jne        0x435410

// block 0043541a
0043541a  lea        edx, [edi + 0x1e9c0]
00435420  sub        edx, eax

// block 00435422
00435422  mov        cl, byte ptr [eax]
00435424  mov        byte ptr [eax + edx], cl
00435427  inc        eax
00435428  test       cl, cl
0043542a  jne        0x435422

// block 0043542c
0043542c  mov        edi, dword ptr [ebp + 0x2dc]
00435432  push       0x74
00435434  lea        edx, [esp + 0x20]
00435438  push       edx
00435439  call       0x4357c0

// block 0043543e
0043543e  mov        eax, dword ptr [eax]
00435440  lea        esi, [ebp + 0x1e8]
00435446  mov        dword ptr [esi], eax
00435448  mov        eax, esi
0043544a  call       0x461d40

// block 0043544f
0043544f  mov        ebx, 3
00435454  mov        eax, ebx
00435456  mov        dword ptr [ebp + 4], 0x16
0043545d  mov        dword ptr [ebp + 0x40], ebx
00435460  mov        dword ptr [ebp + 0x108], 1
0043546a  test       eax, eax
0043546c  je         0x4354ce

// block 0043546e
0043546e  jg         0x4354c7

// block 00435470
00435470  dec        eax
00435471  mov        dword ptr [ebp + 0x38], eax
00435474  jmp        0x4354d5

// block 00435476
00435476  test       eax, 0x102
0043547b  je         0x435577

// block 00435481
00435481  cmp        dword ptr [ebp + 0x38], 0
00435485  jl         0x43542c

// block 00435487
00435487  cmp        dword ptr [ebp + 0x1f0], 0
0043548e  je         0x435577

// block 00435494
00435494  mov        edx, 9
00435499  call       0x453d90

// block 0043549e
0043549e  dec        dword ptr [ebp + 0x1f0]
004354a4  mov        eax, dword ptr [ebp + 0x1f0]
004354aa  pop        edi
004354ab  pop        esi
004354ac  pop        ebx
004354ad  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004354b5  pop        ebp
004354b6  mov        ecx, dword ptr [esp + 0x54]
004354ba  xor        ecx, esp
004354bc  call       0x46c932

// block 004354c1
004354c1  add        esp, 0x58
004354c4  ret        4

// block 004354c7
004354c7  xor        eax, eax
004354c9  mov        dword ptr [ebp + 0x38], eax
004354cc  jmp        0x4354d5

// block 004354ce
004354ce  mov        dword ptr [ebp + 0x38], 0

// block 004354d5
004354d5  mov        ecx, dword ptr [esi]
004354d7  push       ecx
004354d8  call       0x461970

// block 004354dd
004354dd  movzx      ebx, word ptr [ebp + 0x38]
004354e1  mov        edx, dword ptr [esi]
004354e3  add        bx, 7
004354e7  push       edx
004354e8  call       0x461970

// block 004354ed
004354ed  pop        edi
004354ee  pop        esi
004354ef  pop        ebx
004354f0  pop        ebp
004354f1  mov        ecx, dword ptr [esp + 0x54]
004354f5  xor        ecx, esp
004354f7  call       0x46c932

// block 004354fc
004354fc  add        esp, 0x58
004354ff  ret        4

// block 00435502
00435502  cmp        dword ptr [ebp + 0x14], 0xc
00435506  jl         0x435577

// block 00435508
00435508  mov        esi, ebp
0043550a  call       0x4339c0

// block 0043550f
0043550f  mov        dword ptr [ebp + 4], 0x1b
00435516  mov        ebp, dword ptr [ebp + 0x38]
00435519  sub        ebp, 0
0043551c  je         0x435568

// block 0043551e
0043551e  sub        ebp, 1
00435521  je         0x435568

// block 00435523
00435523  sub        ebp, 1
00435526  jne        0x435577

// block 00435528
00435528  mov        eax, 7
0043552d  cmp        dword ptr [0x4b0cb0], eax
00435533  jl         0x435549

// block 00435535
00435535  mov        dword ptr [0x4b452c], 0x4aedb0
0043553f  mov        dword ptr [0x4b0cb0], eax
00435544  mov        dword ptr [0x4b0cb4], eax

// block 00435549
00435549  pop        edi
0043554a  pop        esi
0043554b  pop        ebx
0043554c  mov        dword ptr [0x4cee40], 0xa
00435556  pop        ebp
00435557  mov        ecx, dword ptr [esp + 0x54]
0043555b  xor        ecx, esp
0043555d  call       0x46c932

// block 00435562
00435562  add        esp, 0x58
00435565  ret        4

// block 00435568
00435568  mov        ecx, 4
0043556d  mov        eax, 0x4ce8e8
00435572  call       0x40f720

// block 00435577
00435577  pop        edi
00435578  pop        esi
00435579  pop        ebx

// block 0043557a
0043557a  mov        ecx, dword ptr [esp + 0x58]
0043557e  pop        ebp
0043557f  xor        ecx, esp
00435581  call       0x46c932

// block 00435586
00435586  add        esp, 0x58
00435589  ret        4
