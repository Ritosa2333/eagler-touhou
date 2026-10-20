
// block 00419a20
00419a20  mov        eax, dword ptr [esp + 4]
00419a24  add        eax, 0x2710
00419a29  push       esi
00419a2a  cmp        eax, 0x46
00419a2d  ja         0x419dcc

// block 00419a33
00419a33  jmp        dword ptr [eax*4 + 0x419dd4]

// block 00419a3a
00419a3a  mov        esi, 0x4ce568
00419a3f  call       0x464440

// block 00419a44
00419a44  pop        esi
00419a45  ret        4

// block 00419a48
00419a48  mov        esi, 0x4ce568
00419a4d  call       0x4645b0

// block 00419a52
00419a52  call       0x4931e0

// block 00419a57
00419a57  pop        esi
00419a58  ret        4

// block 00419a5b
00419a5b  mov        esi, 0x4ce568
00419a60  call       0x4645e0

// block 00419a65
00419a65  call       0x4931e0

// block 00419a6a
00419a6a  pop        esi
00419a6b  ret        4

// block 00419a6e
00419a6e  fld        dword ptr [ecx + 0x1074]
00419a74  call       0x4931e0

// block 00419a79
00419a79  pop        esi
00419a7a  ret        4

// block 00419a7d
00419a7d  fld        dword ptr [ecx + 0x1078]
00419a83  call       0x4931e0

// block 00419a88
00419a88  pop        esi
00419a89  ret        4

// block 00419a8c
00419a8c  fld        dword ptr [ecx + 0x10a8]
00419a92  call       0x4931e0

// block 00419a97
00419a97  pop        esi
00419a98  ret        4

// block 00419a9b
00419a9b  fld        dword ptr [ecx + 0x10ac]
00419aa1  call       0x4931e0

// block 00419aa6
00419aa6  pop        esi
00419aa7  ret        4

// block 00419aaa
00419aaa  fld        dword ptr [ecx + 0x10dc]
00419ab0  call       0x4931e0

// block 00419ab5
00419ab5  pop        esi
00419ab6  ret        4

// block 00419ab9
00419ab9  fld        dword ptr [ecx + 0x10e0]
00419abf  call       0x4931e0

// block 00419ac4
00419ac4  pop        esi
00419ac5  ret        4

// block 00419ac8
00419ac8  mov        eax, dword ptr [0x4b4514]
00419acd  fld        dword ptr [eax + 0x97c]
00419ad3  call       0x4931e0

// block 00419ad8
00419ad8  pop        esi
00419ad9  ret        4

// block 00419adc
00419adc  mov        ecx, dword ptr [0x4b4514]
00419ae2  fld        dword ptr [ecx + 0x980]
00419ae8  call       0x4931e0

// block 00419aed
00419aed  pop        esi
00419aee  ret        4

// block 00419af1
00419af1  mov        eax, dword ptr [ecx + 0x12b0]
00419af7  pop        esi
00419af8  ret        4

// block 00419afb
00419afb  mov        eax, dword ptr [ecx + 0x26f8]
00419b01  shr        eax, 0x17
00419b04  and        eax, 1
00419b07  pop        esi
00419b08  ret        4

// block 00419b0b
00419b0b  mov        eax, dword ptr [ecx + 0x127c]
00419b11  pop        esi
00419b12  ret        4

// block 00419b15
00419b15  mov        eax, dword ptr [ecx + 0x1280]
00419b1b  pop        esi
00419b1c  ret        4

// block 00419b1f
00419b1f  mov        eax, dword ptr [ecx + 0x1288]
00419b25  pop        esi
00419b26  ret        4

// block 00419b29
00419b29  fld        dword ptr [ecx + 0x128c]
00419b2f  call       0x4931e0

// block 00419b34
00419b34  pop        esi
00419b35  ret        4

// block 00419b38
00419b38  fld        dword ptr [ecx + 0x1294]
00419b3e  call       0x4931e0

// block 00419b43
00419b43  pop        esi
00419b44  ret        4

// block 00419b47
00419b47  fld        dword ptr [ecx + 0x1298]
00419b4d  call       0x4931e0

// block 00419b52
00419b52  pop        esi
00419b53  ret        4

// block 00419b56
00419b56  mov        edx, dword ptr [0x4b43dc]
00419b5c  mov        eax, dword ptr [edx + 0x1c]
00419b5f  mov        eax, dword ptr [eax + 0x127c]
00419b65  pop        esi
00419b66  ret        4

// block 00419b69
00419b69  mov        ecx, dword ptr [0x4b43dc]
00419b6f  mov        edx, dword ptr [ecx + 0x1c]
00419b72  mov        eax, dword ptr [edx + 0x1280]
00419b78  pop        esi
00419b79  ret        4

// block 00419b7c
00419b7c  mov        eax, dword ptr [0x4b43dc]
00419b81  mov        ecx, dword ptr [eax + 0x1c]

// block 00419b84
00419b84  mov        eax, dword ptr [ecx + 0x1284]
00419b8a  pop        esi
00419b8b  ret        4

// block 00419b8e
00419b8e  mov        edx, dword ptr [0x4b43dc]
00419b94  mov        eax, dword ptr [edx + 0x1c]
00419b97  mov        eax, dword ptr [eax + 0x1288]
00419b9d  pop        esi
00419b9e  ret        4

// block 00419ba1
00419ba1  mov        ecx, dword ptr [0x4b43dc]
00419ba7  mov        edx, dword ptr [ecx + 0x1c]
00419baa  fld        dword ptr [edx + 0x128c]
00419bb0  call       0x4931e0

// block 00419bb5
00419bb5  pop        esi
00419bb6  ret        4

// block 00419bb9
00419bb9  mov        eax, dword ptr [0x4b43dc]
00419bbe  mov        ecx, dword ptr [eax + 0x1c]

// block 00419bc1
00419bc1  fld        dword ptr [ecx + 0x1290]
00419bc7  call       0x4931e0

// block 00419bcc
00419bcc  pop        esi
00419bcd  ret        4

// block 00419bd0
00419bd0  mov        edx, dword ptr [0x4b43dc]
00419bd6  mov        eax, dword ptr [edx + 0x1c]
00419bd9  fld        dword ptr [eax + 0x1294]
00419bdf  call       0x4931e0

// block 00419be4
00419be4  pop        esi
00419be5  ret        4

// block 00419be8
00419be8  mov        ecx, dword ptr [0x4b43dc]
00419bee  mov        edx, dword ptr [ecx + 0x1c]
00419bf1  fld        dword ptr [edx + 0x1298]
00419bf7  call       0x4931e0

// block 00419bfc
00419bfc  pop        esi
00419bfd  ret        4

// block 00419c00
00419c00  fld        dword ptr [ecx + 0x10c4]
00419c06  call       0x4931e0

// block 00419c0b
00419c0b  pop        esi
00419c0c  ret        4

// block 00419c0f
00419c0f  fld        dword ptr [ecx + 0x10f8]
00419c15  call       0x4931e0

// block 00419c1a
00419c1a  pop        esi
00419c1b  ret        4

// block 00419c1e
00419c1e  lea        eax, [ecx + 0x1074]
00419c24  call       0x41c530

// block 00419c29
00419c29  call       0x4931e0

// block 00419c2e
00419c2e  pop        esi
00419c2f  ret        4

// block 00419c32
00419c32  fld        dword ptr [ecx + 0x10c0]
00419c38  call       0x4931e0

// block 00419c3d
00419c3d  pop        esi
00419c3e  ret        4

// block 00419c41
00419c41  fld        dword ptr [ecx + 0x10f4]
00419c47  call       0x4931e0

// block 00419c4c
00419c4c  pop        esi
00419c4d  ret        4

// block 00419c50
00419c50  fld        dword ptr [ecx + 0x10c8]
00419c56  call       0x4931e0

// block 00419c5b
00419c5b  pop        esi
00419c5c  ret        4

// block 00419c5f
00419c5f  fld        dword ptr [ecx + 0x10fc]
00419c65  call       0x4931e0

// block 00419c6a
00419c6a  pop        esi
00419c6b  ret        4

// block 00419c6e
00419c6e  mov        edx, dword ptr [0x4b43dc]
00419c74  mov        eax, dword ptr [edx + 0x1c]
00419c77  fld        dword ptr [eax + 0x1074]
00419c7d  call       0x4931e0

// block 00419c82
00419c82  pop        esi
00419c83  ret        4

// block 00419c86
00419c86  mov        ecx, dword ptr [0x4b43dc]
00419c8c  mov        edx, dword ptr [ecx + 0x1c]
00419c8f  fld        dword ptr [edx + 0x1078]
00419c95  call       0x4931e0

// block 00419c9a
00419c9a  pop        esi
00419c9b  ret        4

// block 00419c9e
00419c9e  lea        esi, [ecx + 0x1120]
00419ca4  call       0x461c50

// block 00419ca9
00419ca9  movsx      eax, word ptr [eax + 0x3ea]
00419cb0  pop        esi
00419cb1  ret        4

// block 00419cb4
00419cb4  mov        eax, dword ptr [0x4b0ccc]
00419cb9  pop        esi
00419cba  ret        4

// block 00419cbd
00419cbd  mov        eax, dword ptr [0x4b0ca8]
00419cc2  pop        esi
00419cc3  ret        4

// block 00419cc6
00419cc6  mov        eax, dword ptr [ecx + 0x2648]
00419ccc  pop        esi
00419ccd  ret        4

// block 00419cd0
00419cd0  xor        eax, eax
00419cd2  cmp        dword ptr [0x4b0ca8], eax
00419cd8  pop        esi
00419cd9  sete       al
00419cdc  ret        4

// block 00419cdf
00419cdf  xor        eax, eax
00419ce1  cmp        dword ptr [0x4b0ca8], 1
00419ce8  pop        esi
00419ce9  sete       al
00419cec  ret        4

// block 00419cef
00419cef  xor        eax, eax
00419cf1  cmp        dword ptr [0x4b0ca8], 2
00419cf8  pop        esi
00419cf9  sete       al
00419cfc  ret        4

// block 00419cff
00419cff  xor        eax, eax
00419d01  cmp        dword ptr [0x4b0ca8], 3
00419d08  pop        esi
00419d09  sete       al
00419d0c  ret        4

// block 00419d0f
00419d0f  mov        eax, dword ptr [0x4b43dc]
00419d14  mov        eax, dword ptr [eax + 0x10]
00419d17  pop        esi
00419d18  ret        4

// block 00419d1b
00419d1b  mov        ecx, dword ptr [0x4b43dc]
00419d21  mov        eax, dword ptr [ecx + 0x14]
00419d24  pop        esi
00419d25  ret        4

// block 00419d28
00419d28  mov        edx, dword ptr [0x4b43dc]
00419d2e  mov        eax, dword ptr [edx + 0x18]
00419d31  pop        esi
00419d32  ret        4

// block 00419d35
00419d35  mov        eax, dword ptr [0x4b43dc]
00419d3a  mov        eax, dword ptr [eax + 0x70]
00419d3d  pop        esi
00419d3e  ret        4

// block 00419d41
00419d41  mov        ecx, dword ptr [0x4b0c94]
00419d47  mov        edx, dword ptr [0x4b0c90]
00419d4d  lea        eax, [ecx + edx*2]
00419d50  pop        esi
00419d51  ret        4

// block 00419d54
00419d54  mov        eax, dword ptr [0x4b4514]
00419d59  add        eax, 0x97c
00419d5e  add        ecx, 0x1074
00419d64  call       0x41c4c0

// block 00419d69
00419d69  call       0x4931e0

// block 00419d6e
00419d6e  pop        esi
00419d6f  ret        4

// block 00419d72
00419d72  fld        dword ptr [ecx + 0x129c]
00419d78  call       0x4931e0

// block 00419d7d
00419d7d  pop        esi
00419d7e  ret        4

// block 00419d81
00419d81  fld        dword ptr [ecx + 0x12a0]
00419d87  call       0x4931e0

// block 00419d8c
00419d8c  pop        esi
00419d8d  ret        4

// block 00419d90
00419d90  fld        dword ptr [ecx + 0x12a4]
00419d96  call       0x4931e0

// block 00419d9b
00419d9b  pop        esi
00419d9c  ret        4

// block 00419d9f
00419d9f  fld        dword ptr [ecx + 0x12a8]
00419da5  call       0x4931e0

// block 00419daa
00419daa  pop        esi
00419dab  ret        4

// block 00419dae
00419dae  mov        eax, dword ptr [0x4b43dc]
00419db3  mov        eax, dword ptr [eax + 0x78]
00419db6  pop        esi
00419db7  ret        4

// block 00419dba
00419dba  mov        eax, dword ptr [0x4b0c48]
00419dbf  pop        esi
00419dc0  ret        4

// block 00419dc3
00419dc3  mov        eax, 1
00419dc8  pop        esi
00419dc9  ret        4

// block 00419dcc
00419dcc  xor        eax, eax
00419dce  pop        esi
00419dcf  ret        4
