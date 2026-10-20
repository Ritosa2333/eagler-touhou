
// block 00481a74
00481a74  mov        edi, edi
00481a76  push       ebp
00481a77  mov        ebp, esp
00481a79  sub        esp, 0x54
00481a7c  mov        eax, dword ptr [0x4b4318]
00481a81  mov        al, byte ptr [eax]
00481a83  push       ebx
00481a84  push       esi
00481a85  mov        esi, 0xffff0000
00481a8a  and        dword ptr [ebp - 0x20], esi
00481a8d  push       edi
00481a8e  xor        edi, edi
00481a90  mov        dword ptr [ebp - 0x24], edi
00481a93  mov        byte ptr [ebp - 1], 0
00481a97  test       al, al
00481a99  je         0x481ecd

// block 00481a9f
00481a9f  cmp        al, 0x24
00481aa1  jne        0x481ad1

// block 00481aa3
00481aa3  push       dword ptr [ebp + 0x18]
00481aa6  lea        eax, [ebp - 1]
00481aa9  push       eax
00481aaa  lea        eax, [ebp + 0x10]
00481aad  push       eax
00481aae  lea        eax, [ebp - 0x1c]
00481ab1  push       eax
00481ab2  call       0x47f037

// block 00481ab7
00481ab7  mov        ecx, dword ptr [ebp - 0x1c]
00481aba  add        esp, 0x10
00481abd  cmp        ecx, edi
00481abf  je         0x481ad1

// block 00481ac1
00481ac1  mov        eax, dword ptr [ebp + 8]
00481ac4  mov        dword ptr [eax], ecx
00481ac6  mov        ecx, dword ptr [ebp - 0x18]

// block 00481ac9
00481ac9  mov        dword ptr [eax + 4], ecx
00481acc  jmp        0x481f36

// block 00481ad1
00481ad1  mov        ecx, dword ptr [0x4b4318]
00481ad7  mov        al, byte ptr [ecx]
00481ad9  xor        edx, edx
00481adb  cmp        al, 0x41
00481add  setl       dl
00481ae0  and        dword ptr [ebp - 0x10], esi
00481ae3  movsx      ebx, al
00481ae6  push       0x20
00481ae8  mov        dword ptr [ebp - 0x14], edi
00481aeb  dec        edx
00481aec  and        edx, 0x2b
00481aef  add        edx, 0x16
00481af2  sub        ebx, edx
00481af4  and        dword ptr [ebp - 0x18], esi
00481af7  mov        dword ptr [ebp - 0x1c], edi
00481afa  pop        edi

// block 00481afb
00481afb  mov        eax, ebx
00481afd  sub        eax, 4
00481b00  je         0x481ba3

// block 00481b06
00481b06  dec        eax
00481b07  je         0x481b60

// block 00481b09
00481b09  sub        eax, 3
00481b0c  jne        0x481c3c

// block 00481b12
00481b12  mov        eax, dword ptr [0x4b4328]
00481b17  shr        eax, 1
00481b19  not        eax
00481b1b  test       al, 1
00481b1d  je         0x481be1

// block 00481b23
00481b23  push       8
00481b25  call       0x47dc0b

// block 00481b2a
00481b2a  add        esp, 4
00481b2d  cmp        dword ptr [ebp - 0x14], 0
00481b31  lea        ecx, [ebp - 0x14]
00481b34  push       eax
00481b35  je         0x481bdc

// block 00481b3b
00481b3b  lea        eax, [ebp - 0x2c]
00481b3e  push       eax
00481b3f  lea        eax, [ebp - 0x34]

// block 00481b42
00481b42  push       edi
00481b43  push       eax
00481b44  call       0x47ec6e

// block 00481b49
00481b49  mov        ecx, eax
00481b4b  call       0x47ec92

// block 00481b50
00481b50  mov        ecx, dword ptr [eax]
00481b52  mov        eax, dword ptr [eax + 4]
00481b55  mov        dword ptr [ebp - 0x14], ecx
00481b58  mov        dword ptr [ebp - 0x10], eax
00481b5b  jmp        0x481be1

// block 00481b60
00481b60  mov        eax, dword ptr [0x4b4328]
00481b65  shr        eax, 1
00481b67  not        eax
00481b69  test       al, 1
00481b6b  je         0x481be1

// block 00481b6d
00481b6d  push       9
00481b6f  call       0x47dc0b

// block 00481b74
00481b74  add        esp, 4
00481b77  cmp        dword ptr [ebp - 0x1c], 0
00481b7b  lea        ecx, [ebp - 0x1c]
00481b7e  push       eax
00481b7f  je         0x481bdc

// block 00481b81
00481b81  lea        eax, [ebp - 0x3c]
00481b84  push       eax
00481b85  push       edi
00481b86  lea        eax, [ebp - 0x44]
00481b89  push       eax
00481b8a  call       0x47ec6e

// block 00481b8f
00481b8f  mov        ecx, eax
00481b91  call       0x47ec92

// block 00481b96
00481b96  mov        ecx, dword ptr [eax]
00481b98  mov        eax, dword ptr [eax + 4]
00481b9b  mov        dword ptr [ebp - 0x1c], ecx
00481b9e  mov        dword ptr [ebp - 0x18], eax
00481ba1  jmp        0x481be1

// block 00481ba3
00481ba3  mov        eax, dword ptr [0x4b4328]
00481ba8  mov        ecx, eax
00481baa  shr        ecx, 1
00481bac  not        ecx
00481bae  test       cl, 1
00481bb1  je         0x481be1

// block 00481bb3
00481bb3  shr        eax, 0x11
00481bb6  not        eax
00481bb8  test       al, 1
00481bba  je         0x481be1

// block 00481bbc
00481bbc  push       7
00481bbe  call       0x47dc0b

// block 00481bc3
00481bc3  add        esp, 4
00481bc6  cmp        dword ptr [ebp - 0x14], 0
00481bca  lea        ecx, [ebp - 0x14]
00481bcd  push       eax
00481bce  je         0x481bdc

// block 00481bd0
00481bd0  lea        eax, [ebp - 0x4c]
00481bd3  push       eax
00481bd4  lea        eax, [ebp - 0x54]
00481bd7  jmp        0x481b42

// block 00481bdc
00481bdc  call       0x47e896

// block 00481be1
00481be1  inc        dword ptr [0x4b4318]
00481be7  mov        eax, dword ptr [0x4b4318]
00481bec  cmp        byte ptr [eax], 0x24
00481bef  jne        0x481c0f

// block 00481bf1
00481bf1  push       dword ptr [ebp + 0x18]
00481bf4  lea        eax, [ebp - 1]
00481bf7  push       eax
00481bf8  lea        eax, [ebp + 0x10]
00481bfb  push       eax
00481bfc  lea        eax, [ebp - 0xc]
00481bff  push       eax
00481c00  call       0x47f037

// block 00481c05
00481c05  mov        ecx, dword ptr [ebp - 0xc]
00481c08  add        esp, 0x10
00481c0b  test       ecx, ecx
00481c0d  jne        0x481c2f

// block 00481c0f
00481c0f  mov        ecx, dword ptr [0x4b4318]
00481c15  mov        al, byte ptr [ecx]
00481c17  xor        edx, edx
00481c19  cmp        al, 0x41
00481c1b  setl       dl
00481c1e  movsx      ebx, al
00481c21  dec        edx
00481c22  and        edx, 0x2b
00481c25  add        edx, 0x16
00481c28  sub        ebx, edx
00481c2a  jmp        0x481afb

// block 00481c2f
00481c2f  mov        eax, dword ptr [ebp + 8]
00481c32  mov        dword ptr [eax], ecx
00481c34  mov        ecx, dword ptr [ebp - 8]
00481c37  jmp        0x481ac9

// block 00481c3c
00481c3c  cmp        byte ptr [ecx], 0
00481c3f  je         0x481c48

// block 00481c41
00481c41  inc        ecx
00481c42  mov        dword ptr [0x4b4318], ecx

// block 00481c48
00481c48  cmp        ebx, 0x1f
00481c4b  ja         0x481cdf

// block 00481c51
00481c51  push       dword ptr [ebp + 0x10]
00481c54  lea        ecx, [ebp - 0xc]
00481c57  call       0x47e56d

// block 00481c5c
00481c5c  lea        eax, [ebp - 0xc]
00481c5f  push       eax
00481c60  lea        eax, [ebp - 0x54]
00481c63  push       eax
00481c64  lea        ecx, [ebp - 0x24]
00481c67  call       0x47e9ae

// block 00481c6c
00481c6c  mov        ecx, dword ptr [eax]
00481c6e  mov        eax, dword ptr [eax + 4]
00481c71  xor        esi, esi
00481c73  mov        dword ptr [ebp - 0xc], ecx
00481c76  mov        dword ptr [ebp - 8], eax
00481c79  cmp        dword ptr [ebp - 0x14], esi
00481c7c  je         0x481ca5

// block 00481c7e
00481c7e  lea        eax, [ebp - 0x14]
00481c81  push       eax
00481c82  lea        eax, [ebp - 0x54]
00481c85  push       eax
00481c86  push       edi
00481c87  lea        eax, [ebp - 0x4c]
00481c8a  push       eax
00481c8b  lea        ecx, [ebp - 0xc]
00481c8e  call       0x47ec6e

// block 00481c93
00481c93  mov        ecx, eax
00481c95  call       0x47e9ae

// block 00481c9a
00481c9a  mov        ecx, dword ptr [eax]
00481c9c  mov        eax, dword ptr [eax + 4]
00481c9f  mov        dword ptr [ebp - 0xc], ecx
00481ca2  mov        dword ptr [ebp - 8], eax

// block 00481ca5
00481ca5  cmp        dword ptr [ebp - 0x1c], esi
00481ca8  je         0x481cd1

// block 00481caa
00481caa  lea        eax, [ebp - 0xc]
00481cad  push       eax
00481cae  lea        eax, [ebp - 0x54]
00481cb1  push       eax
00481cb2  push       edi
00481cb3  lea        eax, [ebp - 0x4c]
00481cb6  push       eax
00481cb7  lea        ecx, [ebp - 0x1c]
00481cba  call       0x47ec6e

// block 00481cbf
00481cbf  mov        ecx, eax
00481cc1  call       0x47e9ae

// block 00481cc6
00481cc6  mov        ecx, dword ptr [eax]
00481cc8  mov        eax, dword ptr [eax + 4]
00481ccb  mov        dword ptr [ebp - 0xc], ecx
00481cce  mov        dword ptr [ebp - 8], eax

// block 00481cd1
00481cd1  test       bl, 0x10
00481cd4  je         0x481d8b

// block 00481cda
00481cda  cmp        dword ptr [ebp + 0x18], esi
00481cdd  je         0x481ce6

// block 00481cdf
00481cdf  push       2
00481ce1  jmp        0x481f2b

// block 00481ce6
00481ce6  cmp        byte ptr [ebp + 0x10], 0
00481cea  je         0x481d49

// block 00481cec
00481cec  lea        eax, [ebp - 0xc]
00481cef  push       eax
00481cf0  lea        eax, [ebp - 0x54]
00481cf3  push       0x49df1c
00481cf8  push       eax
00481cf9  call       0x47ec4a

// block 00481cfe
00481cfe  mov        ecx, dword ptr [eax]
00481d00  mov        eax, dword ptr [eax + 4]
00481d03  mov        dword ptr [ebp - 8], eax
00481d06  mov        eax, dword ptr [0x4b4318]
00481d0b  add        esp, 0xc
00481d0e  cmp        byte ptr [eax], 0
00481d11  lea        eax, [ebp - 0xc]
00481d14  push       eax
00481d15  mov        dword ptr [ebp - 0xc], ecx
00481d18  lea        eax, [ebp - 0x54]
00481d1b  je         0x481d31

// block 00481d1d
00481d1d  push       eax
00481d1e  lea        eax, [ebp - 0x4c]
00481d21  push       eax
00481d22  call       0x4814b0

// block 00481d27
00481d27  pop        ecx
00481d28  mov        ecx, eax
00481d2a  call       0x47e9ae

// block 00481d2f
00481d2f  jmp        0x481d3c

// block 00481d31
00481d31  push       1
00481d33  push       eax
00481d34  call       0x47ec26

// block 00481d39
00481d39  add        esp, 0xc

// block 00481d3c
00481d3c  mov        ecx, dword ptr [eax]
00481d3e  mov        eax, dword ptr [eax + 4]
00481d41  mov        dword ptr [ebp - 0xc], ecx
00481d44  mov        dword ptr [ebp - 8], eax
00481d47  jmp        0x481d66

// block 00481d49
00481d49  mov        eax, dword ptr [0x4b4318]
00481d4e  cmp        byte ptr [eax], 0
00481d51  je         0x481d71

// block 00481d53
00481d53  lea        eax, [ebp - 0x54]
00481d56  push       eax
00481d57  call       0x4814b0

// block 00481d5c
00481d5c  pop        ecx
00481d5d  push       eax
00481d5e  lea        ecx, [ebp - 0xc]
00481d61  call       0x47ddc1

// block 00481d66
00481d66  mov        eax, dword ptr [0x4b4318]
00481d6b  mov        al, byte ptr [eax]
00481d6d  test       al, al
00481d6f  jne        0x481d7d

// block 00481d71
00481d71  push       1
00481d73  lea        ecx, [ebp - 0xc]
00481d76  call       0x47e4af

// block 00481d7b
00481d7b  jmp        0x481d8b

// block 00481d7d
00481d7d  inc        dword ptr [0x4b4318]
00481d83  cmp        al, 0x40
00481d85  jne        0x481cdf

// block 00481d8b
00481d8b  mov        eax, dword ptr [0x4b4328]
00481d90  shr        eax, 1
00481d92  not        eax
00481d94  test       al, 1
00481d96  mov        eax, ebx
00481d98  je         0x481dd0

// block 00481d9a
00481d9a  and        eax, 0xc
00481d9d  cmp        al, 0xc
00481d9f  jne        0x481dea

// block 00481da1
00481da1  cmp        dword ptr [ebp + 0x18], esi
00481da4  jne        0x481cdf

// block 00481daa
00481daa  lea        eax, [ebp - 0xc]
00481dad  push       eax
00481dae  lea        eax, [ebp - 0x54]
00481db1  push       eax
00481db2  lea        eax, [ebp - 0x4c]
00481db5  push       eax
00481db6  call       0x480665

// block 00481dbb
00481dbb  pop        ecx
00481dbc  mov        ecx, eax
00481dbe  call       0x47e9ae

// block 00481dc3
00481dc3  mov        ecx, dword ptr [eax]
00481dc5  mov        eax, dword ptr [eax + 4]
00481dc8  mov        dword ptr [ebp - 0xc], ecx
00481dcb  mov        dword ptr [ebp - 8], eax
00481dce  jmp        0x481dea

// block 00481dd0
00481dd0  and        eax, 0xc
00481dd3  cmp        al, 0xc
00481dd5  jne        0x481dea

// block 00481dd7
00481dd7  lea        eax, [ebp - 0x54]
00481dda  push       eax
00481ddb  call       0x480665

// block 00481de0
00481de0  pop        ecx
00481de1  push       eax
00481de2  lea        ecx, [ebp - 0xc]
00481de5  call       0x47ddc1

// block 00481dea
00481dea  test       bl, 2
00481ded  je         0x481e0f

// block 00481def
00481def  lea        eax, [ebp - 0xc]
00481df2  push       eax
00481df3  lea        eax, [ebp - 0x54]
00481df6  push       0x49e080
00481dfb  push       eax
00481dfc  call       0x47ec4a

// block 00481e01
00481e01  mov        ecx, dword ptr [eax]
00481e03  mov        eax, dword ptr [eax + 4]
00481e06  add        esp, 0xc
00481e09  mov        dword ptr [ebp - 0xc], ecx
00481e0c  mov        dword ptr [ebp - 8], eax

// block 00481e0f
00481e0f  test       bl, 1
00481e12  je         0x481e34

// block 00481e14
00481e14  lea        eax, [ebp - 0xc]
00481e17  push       eax
00481e18  lea        eax, [ebp - 0x54]
00481e1b  push       0x49e078
00481e20  push       eax
00481e21  call       0x47ec4a

// block 00481e26
00481e26  mov        ecx, dword ptr [eax]
00481e28  mov        eax, dword ptr [eax + 4]
00481e2b  add        esp, 0xc
00481e2e  mov        dword ptr [ebp - 0xc], ecx
00481e31  mov        dword ptr [ebp - 8], eax

// block 00481e34
00481e34  xor        ebx, ebx
00481e36  mov        esi, 0x100
00481e3b  cmp        dword ptr [ebp + 0x18], ebx
00481e3e  jne        0x481eaf

// block 00481e40
00481e40  mov        eax, dword ptr [ebp + 0xc]
00481e43  cmp        dword ptr [eax], ebx
00481e45  je         0x481e91

// block 00481e47
00481e47  mov        edx, dword ptr [eax + 4]
00481e4a  test       esi, edx
00481e4c  jne        0x481e7d

// block 00481e4e
00481e4e  mov        ecx, dword ptr [ebp + 0x14]
00481e51  cmp        dword ptr [ecx], ebx
00481e53  je         0x481e7d

// block 00481e55
00481e55  push       eax
00481e56  lea        eax, [ebp - 0x54]
00481e59  push       eax
00481e5a  push       edi
00481e5b  lea        eax, [ebp - 0x4c]
00481e5e  push       eax
00481e5f  push       ecx
00481e60  lea        eax, [ebp - 0x44]
00481e63  push       edi
00481e64  push       eax
00481e65  call       0x47ec02

// block 00481e6a
00481e6a  add        esp, 0xc
00481e6d  mov        ecx, eax
00481e6f  call       0x47ec6e

// block 00481e74
00481e74  mov        ecx, eax
00481e76  call       0x47e9ae

// block 00481e7b
00481e7b  jmp        0x481ea6

// block 00481e7d
00481e7d  test       edx, 0x800
00481e83  je         0x481e98

// block 00481e85
00481e85  mov        ecx, dword ptr [eax]
00481e87  mov        eax, edx
00481e89  mov        dword ptr [ebp - 0xc], ecx
00481e8c  mov        dword ptr [ebp - 8], eax
00481e8f  jmp        0x481eaf

// block 00481e91
00481e91  mov        eax, dword ptr [ebp + 0x14]
00481e94  cmp        dword ptr [eax], ebx
00481e96  je         0x481eaf

// block 00481e98
00481e98  push       eax
00481e99  lea        eax, [ebp - 0x54]
00481e9c  push       edi
00481e9d  push       eax
00481e9e  call       0x47ec02

// block 00481ea3
00481ea3  add        esp, 0xc

// block 00481ea6
00481ea6  push       eax
00481ea7  lea        ecx, [ebp - 0xc]
00481eaa  call       0x47e7bf

// block 00481eaf
00481eaf  mov        ecx, dword ptr [ebp - 8]
00481eb2  or         ecx, esi
00481eb4  cmp        byte ptr [ebp - 1], 0
00481eb8  je         0x481ec0

// block 00481eba
00481eba  or         ecx, 0x2000

// block 00481ec0
00481ec0  mov        eax, dword ptr [ebp + 8]
00481ec3  mov        edx, dword ptr [ebp - 0xc]
00481ec6  mov        dword ptr [eax], edx
00481ec8  jmp        0x481ac9

// block 00481ecd
00481ecd  cmp        dword ptr [ebp + 0x18], edi
00481ed0  jne        0x481f29

// block 00481ed2
00481ed2  mov        eax, dword ptr [ebp + 0xc]
00481ed5  cmp        dword ptr [eax], edi
00481ed7  je         0x481f22

// block 00481ed9
00481ed9  test       dword ptr [eax + 4], 0x100
00481ee0  jne        0x481f12

// block 00481ee2
00481ee2  mov        ecx, dword ptr [ebp + 0x14]
00481ee5  cmp        dword ptr [ecx], edi
00481ee7  je         0x481f12

// block 00481ee9
00481ee9  push       eax
00481eea  push       dword ptr [ebp + 8]
00481eed  lea        eax, [ebp - 0x54]
00481ef0  push       0x20
00481ef2  push       eax
00481ef3  push       ecx
00481ef4  lea        eax, [ebp - 0x4c]
00481ef7  push       1
00481ef9  push       eax
00481efa  call       0x47ec26

// block 00481eff
00481eff  add        esp, 0xc
00481f02  mov        ecx, eax
00481f04  call       0x47ec6e

// block 00481f09
00481f09  mov        ecx, eax
00481f0b  call       0x47e9ae

// block 00481f10
00481f10  jmp        0x481f33

// block 00481f12
00481f12  push       eax
00481f13  push       1
00481f15  push       dword ptr [ebp + 8]
00481f18  call       0x47ec26

// block 00481f1d
00481f1d  add        esp, 0xc
00481f20  jmp        0x481f33

// block 00481f22
00481f22  mov        eax, dword ptr [ebp + 0x14]
00481f25  cmp        dword ptr [eax], edi
00481f27  jne        0x481f12

// block 00481f29
00481f29  push       1

// block 00481f2b
00481f2b  mov        ecx, dword ptr [ebp + 8]
00481f2e  call       0x47e1ce

// block 00481f33
00481f33  mov        eax, dword ptr [ebp + 8]

// block 00481f36
00481f36  pop        edi
00481f37  pop        esi
00481f38  pop        ebx
00481f39  leave      
00481f3a  ret        
