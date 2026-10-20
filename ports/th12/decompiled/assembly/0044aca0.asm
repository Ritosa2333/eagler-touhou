
// block 0044aca0
0044aca0  fld1       
0044aca2  push       ebx
0044aca3  push       esi
0044aca4  mov        esi, dword ptr [0x4b4534]
0044acaa  dec        eax
0044acab  push       edi
0044acac  mov        edi, 1
0044acb1  cmp        eax, 0xb
0044acb4  ja         0x44ad63

// block 0044acba
0044acba  movzx      eax, byte ptr [eax + 0x44aeb0]
0044acc1  jmp        dword ptr [eax*4 + 0x44aea0]

// block 0044acc8
0044acc8  mov        ecx, dword ptr [esi + 0x30]
0044accb  cmp        ecx, -1
0044acce  jne        0x44acd5

// block 0044acd0
0044acd0  add        dword ptr [esi + 0x4c], edi
0044acd3  jmp        0x44acd8

// block 0044acd5
0044acd5  add        dword ptr [esi + 0x48], edi

// block 0044acd8
0044acd8  fcom       dword ptr [esi + 0x50]
0044acdb  fnstsw     ax
0044acdd  test       ah, 0x41
0044ace0  jnp        0x44ae98

// block 0044ace6
0044ace6  lea        eax, [ecx + 1]
0044ace9  cmp        eax, 4
0044acec  ja         0x44ad63

// block 0044acee
0044acee  jmp        dword ptr [eax*4 + 0x44aebc]

// block 0044acf5
0044acf5  fild       dword ptr [0x4b0cb0]
0044acfb  fmul       qword ptr [0x4a3fa0]
0044ad01  fsubr      qword ptr [0x4a4350]
0044ad07  fadd       dword ptr [esi + 0x50]
0044ad0a  jmp        0x44ad60

// block 0044ad0c
0044ad0c  mov        ecx, dword ptr [esi + 0x30]
0044ad0f  cmp        ecx, -1
0044ad12  jne        0x44ad19

// block 0044ad14
0044ad14  add        dword ptr [esi + 0x48], edi
0044ad17  jmp        0x44ad1c

// block 0044ad19
0044ad19  add        dword ptr [esi + 0x4c], edi

// block 0044ad1c
0044ad1c  fcom       dword ptr [esi + 0x50]
0044ad1f  fnstsw     ax
0044ad21  test       ah, 0x41
0044ad24  jnp        0x44ae98

// block 0044ad2a
0044ad2a  lea        eax, [ecx + 1]
0044ad2d  cmp        eax, 4
0044ad30  ja         0x44ad63

// block 0044ad32
0044ad32  jmp        dword ptr [eax*4 + 0x44aed0]

// block 0044ad39
0044ad39  fcom       dword ptr [esi + 0x50]
0044ad3c  fnstsw     ax
0044ad3e  test       ah, 0x41
0044ad41  jnp        0x44ae98

// block 0044ad47
0044ad47  mov        eax, dword ptr [esi + 0x30]
0044ad4a  inc        eax
0044ad4b  cmp        eax, 4
0044ad4e  ja         0x44ad63

// block 0044ad50
0044ad50  jmp        dword ptr [eax*4 + 0x44aee4]

// block 0044ad57
0044ad57  fld        dword ptr [esi + 0x50]
0044ad5a  fadd       qword ptr [0x4a3e28]

// block 0044ad60
0044ad60  fstp       dword ptr [esi + 0x50]

// block 0044ad63
0044ad63  fcom       dword ptr [esi + 0x50]
0044ad66  fnstsw     ax
0044ad68  test       ah, 0x41
0044ad6b  jp         0x44ae98

// block 0044ad71
0044ad71  mov        ecx, dword ptr [esi + 0x34]
0044ad74  fstp       dword ptr [esi + 0x50]
0044ad77  fld        dword ptr [ecx + 0x1074]
0044ad7d  push       ecx
0044ad7e  mov        eax, 0x30
0044ad83  fstp       dword ptr [esp]
0044ad86  call       0x453e20

// block 0044ad8b
0044ad8b  mov        edx, dword ptr [esi + 0x3c]
0044ad8e  push       edx
0044ad8f  mov        ebx, 2
0044ad94  call       0x461970

// block 0044ad99
0044ad99  mov        eax, dword ptr [esi + 0x30]
0044ad9c  cmp        eax, -1
0044ad9f  je         0x44ae03

// block 0044ada1
0044ada1  cmp        eax, edi
0044ada3  je         0x44add8

// block 0044ada5
0044ada5  cmp        eax, 3
0044ada8  jne        0x44ae9a

// block 0044adae
0044adae  fld        dword ptr [0x4a42c0]
0044adb4  mov        eax, dword ptr [esi + 0x34]
0044adb7  sub        esp, 8
0044adba  fstp       dword ptr [esp + 4]
0044adbe  add        eax, 0x1074
0044adc3  fld        dword ptr [0x4a4060]
0044adc9  fstp       dword ptr [esp]
0044adcc  push       eax
0044adcd  push       7
0044adcf  call       0x4273f0

// block 0044add4
0044add4  pop        edi
0044add5  pop        esi
0044add6  pop        ebx
0044add7  ret        

// block 0044add8
0044add8  fld        dword ptr [0x4a42c0]
0044adde  mov        ecx, dword ptr [esi + 0x34]
0044ade1  sub        esp, 8
0044ade4  fstp       dword ptr [esp + 4]
0044ade8  add        ecx, 0x1074
0044adee  fld        dword ptr [0x4a4060]
0044adf4  fstp       dword ptr [esp]
0044adf7  push       ecx
0044adf8  push       4
0044adfa  call       0x4273f0

// block 0044adff
0044adff  pop        edi
0044ae00  pop        esi
0044ae01  pop        ebx
0044ae02  ret        

// block 0044ae03
0044ae03  mov        eax, dword ptr [0x4b0c54]
0044ae08  sub        eax, edi
0044ae0a  je         0x44ae6d

// block 0044ae0c
0044ae0c  sub        eax, edi
0044ae0e  je         0x44ae43

// block 0044ae10
0044ae10  sub        eax, edi
0044ae12  jne        0x44ae9a

// block 0044ae18
0044ae18  fld        dword ptr [0x4a42c0]
0044ae1e  mov        edx, dword ptr [esi + 0x34]
0044ae21  sub        esp, 8
0044ae24  fstp       dword ptr [esp + 4]
0044ae28  add        edx, 0x1074
0044ae2e  fld        dword ptr [0x4a4060]
0044ae34  fstp       dword ptr [esp]
0044ae37  push       edx
0044ae38  push       0xf
0044ae3a  call       0x4273f0

// block 0044ae3f
0044ae3f  pop        edi
0044ae40  pop        esi
0044ae41  pop        ebx
0044ae42  ret        

// block 0044ae43
0044ae43  fld        dword ptr [0x4a42c0]
0044ae49  mov        eax, dword ptr [esi + 0x34]
0044ae4c  sub        esp, 8
0044ae4f  fstp       dword ptr [esp + 4]
0044ae53  add        eax, 0x1074
0044ae58  fld        dword ptr [0x4a4060]
0044ae5e  fstp       dword ptr [esp]
0044ae61  push       eax
0044ae62  push       0xe
0044ae64  call       0x4273f0

// block 0044ae69
0044ae69  pop        edi
0044ae6a  pop        esi
0044ae6b  pop        ebx
0044ae6c  ret        

// block 0044ae6d
0044ae6d  fld        dword ptr [0x4a42c0]
0044ae73  mov        ecx, dword ptr [esi + 0x34]
0044ae76  sub        esp, 8
0044ae79  fstp       dword ptr [esp + 4]
0044ae7d  add        ecx, 0x1074
0044ae83  fld        dword ptr [0x4a4060]
0044ae89  fstp       dword ptr [esp]
0044ae8c  push       ecx
0044ae8d  push       0xd
0044ae8f  call       0x4273f0

// block 0044ae94
0044ae94  pop        edi
0044ae95  pop        esi
0044ae96  pop        ebx
0044ae97  ret        

// block 0044ae98
0044ae98  fstp       st(0)

// block 0044ae9a
0044ae9a  pop        edi
0044ae9b  pop        esi
0044ae9c  pop        ebx
0044ae9d  ret        
