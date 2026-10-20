
// block 0042ada0
0042ada0  push       ebp
0042ada1  mov        ebp, esp
0042ada3  and        esp, 0xfffffff8
0042ada6  sub        esp, 0x10
0042ada9  push       esi
0042adaa  push       edi
0042adab  mov        edi, ecx
0042adad  mov        eax, dword ptr [edi]
0042adaf  mov        edx, dword ptr [eax]
0042adb1  call       edx

// block 0042adb3
0042adb3  mov        eax, dword ptr [edi + 0x430]
0042adb9  test       eax, eax
0042adbb  je         0x42ae00

// block 0042adbd
0042adbd  test       eax, 0x1000
0042adc2  je         0x42adef

// block 0042adc4
0042adc4  cmp        dword ptr [edi + 0x18c], 0
0042adcb  jg         0x42adda

// block 0042adcd
0042adcd  xor        eax, 0x1000
0042add2  mov        dword ptr [edi + 0x430], eax
0042add8  jmp        0x42adef

// block 0042adda
0042adda  fld        dword ptr [0x4a3da8]
0042ade0  push       ecx
0042ade1  lea        esi, [edi + 0x188]
0042ade7  fstp       dword ptr [esp]
0042adea  call       0x464a20

// block 0042adef
0042adef  mov        eax, dword ptr [edi + 0x44c]
0042adf5  test       eax, eax
0042adf7  je         0x42ae00

// block 0042adf9
0042adf9  dec        eax
0042adfa  mov        dword ptr [edi + 0x44c], eax

// block 0042ae00
0042ae00  fld        dword ptr [edi + 0x6c]
0042ae03  fld        dword ptr [edi + 0x474]
0042ae09  fcompp     
0042ae0b  fnstsw     ax
0042ae0d  test       ah, 0x41
0042ae10  jne        0x42ae41

// block 0042ae12
0042ae12  fld        dword ptr [edi + 0x74]
0042ae15  fmul       dword ptr [0x4b2ed0]
0042ae1b  fadd       dword ptr [edi + 0x6c]
0042ae1e  fstp       dword ptr [esp + 8]
0042ae22  fld        dword ptr [esp + 8]
0042ae26  fst        dword ptr [edi + 0x6c]
0042ae29  fld        dword ptr [edi + 0x474]
0042ae2f  fcompp     
0042ae31  fnstsw     ax
0042ae33  test       ah, 5
0042ae36  jp         0x42ae41

// block 0042ae38
0042ae38  fld        dword ptr [edi + 0x474]
0042ae3e  fstp       dword ptr [edi + 0x6c]

// block 0042ae41
0042ae41  fld        dword ptr [edi + 0x470]
0042ae47  sub        esp, 8
0042ae4a  fmul       dword ptr [0x4b2ed0]
0042ae50  fstp       dword ptr [esp + 0x10]
0042ae54  fld        dword ptr [esp + 0x10]
0042ae58  fstp       dword ptr [esp + 4]
0042ae5c  fld        dword ptr [edi + 0x68]
0042ae5f  fstp       dword ptr [esp]
0042ae62  call       0x464640

// block 0042ae67
0042ae67  test       byte ptr [edi + 0x4a8], 1
0042ae6e  fstp       dword ptr [edi + 0x68]
0042ae71  je         0x42ae99

// block 0042ae73
0042ae73  mov        eax, dword ptr [0x4b43dc]
0042ae78  mov        eax, dword ptr [eax + 0x1c]
0042ae7b  test       eax, eax
0042ae7d  je         0x42ae99

// block 0042ae7f
0042ae7f  mov        ecx, dword ptr [eax + 0x1074]
0042ae85  add        eax, 0x1074
0042ae8a  mov        dword ptr [edi + 0x50], ecx
0042ae8d  mov        edx, dword ptr [eax + 4]
0042ae90  mov        dword ptr [edi + 0x54], edx
0042ae93  mov        eax, dword ptr [eax + 8]
0042ae96  mov        dword ptr [edi + 0x58], eax

// block 0042ae99
0042ae99  fld        dword ptr [0x4b2ed0]
0042ae9f  fld        st(0)
0042aea1  fmul       dword ptr [edi + 0x460]
0042aea7  fstp       dword ptr [esp + 0xc]
0042aeab  fld        dword ptr [edi + 0x464]
0042aeb1  fmul       st(1)
0042aeb3  fstp       dword ptr [esp + 0x10]
0042aeb7  fmul       dword ptr [edi + 0x468]
0042aebd  fstp       dword ptr [esp + 0x14]
0042aec1  fld        dword ptr [edi + 0x50]
0042aec4  fadd       dword ptr [esp + 0xc]
0042aec8  fstp       dword ptr [edi + 0x50]
0042aecb  fld        dword ptr [edi + 0x54]
0042aece  fadd       dword ptr [esp + 0x10]
0042aed2  fstp       dword ptr [edi + 0x54]
0042aed5  fld        dword ptr [edi + 0x58]
0042aed8  fadd       dword ptr [esp + 0x14]
0042aedc  fstp       dword ptr [edi + 0x58]
0042aedf  mov        eax, dword ptr [edi + 0xc]
0042aee2  add        eax, -2
0042aee5  cmp        eax, 3
0042aee8  ja         0x42af9c

// block 0042aeee
0042aeee  jmp        dword ptr [eax*4 + 0x42b0e4]

// block 0042aef5
0042aef5  mov        ecx, dword ptr [edi + 0x18]
0042aef8  cmp        ecx, dword ptr [edi + 0x484]
0042aefe  jl         0x42af9c

// block 0042af04
0042af04  push       0
0042af06  lea        eax, [edi + 0x14]
0042af09  call       0x4067e0

// block 0042af0e
0042af0e  mov        dword ptr [edi + 0xc], 4
0042af15  jmp        0x42af9c

// block 0042af1a
0042af1a  mov        eax, dword ptr [edi + 0x488]
0042af20  cmp        dword ptr [edi + 0x18], eax
0042af23  mov        dword ptr [esp + 8], eax
0042af27  jl         0x42af79

// block 0042af29
0042af29  push       0
0042af2b  lea        eax, [edi + 0x14]
0042af2e  call       0x4067e0

// block 0042af33
0042af33  fld        dword ptr [edi + 0x47c]
0042af39  fstp       dword ptr [edi + 0x70]
0042af3c  mov        dword ptr [edi + 0xc], 2

// block 0042af43
0042af43  mov        edx, dword ptr [edi + 0x18]
0042af46  cmp        edx, dword ptr [edi + 0x48c]
0042af4c  jl         0x42af9c

// block 0042af4e
0042af4e  push       0
0042af50  lea        eax, [edi + 0x14]
0042af53  call       0x4067e0

// block 0042af58
0042af58  mov        dword ptr [edi + 0xc], 5

// block 0042af5f
0042af5f  mov        eax, dword ptr [edi + 0x490]
0042af65  cmp        dword ptr [edi + 0x18], eax
0042af68  mov        dword ptr [esp + 8], eax
0042af6c  jl         0x42af88

// block 0042af6e
0042af6e  mov        eax, 1
0042af73  pop        edi
0042af74  pop        esi
0042af75  mov        esp, ebp
0042af77  pop        ebp
0042af78  ret        

// block 0042af79
0042af79  fld        dword ptr [edi + 0x47c]
0042af7f  fmul       dword ptr [edi + 0x1c]
0042af82  fidiv      dword ptr [esp + 8]
0042af86  jmp        0x42af99

// block 0042af88
0042af88  fld        dword ptr [edi + 0x47c]
0042af8e  fld        dword ptr [edi + 0x1c]
0042af91  fmul       st(1)
0042af93  fidiv      dword ptr [esp + 8]
0042af97  fsubp      st(1)

// block 0042af99
0042af99  fstp       dword ptr [edi + 0x70]

// block 0042af9c
0042af9c  mov        eax, dword ptr [edi + 0xc]
0042af9f  cmp        eax, 4
0042afa2  je         0x42afad

// block 0042afa4
0042afa4  cmp        eax, 2
0042afa7  jne        0x42b089

// block 0042afad
0042afad  fld        dword ptr [0x4a3e68]
0042afb3  fcomp      dword ptr [edi + 0x6c]
0042afb6  fnstsw     ax
0042afb8  test       ah, 5
0042afbb  jp         0x42b089

// block 0042afc1
0042afc1  fld        dword ptr [0x4a3d78]
0042afc7  mov        eax, dword ptr [edi + 0x50]
0042afca  fcomp      dword ptr [edi + 0x70]
0042afcd  mov        ecx, dword ptr [edi + 0x54]
0042afd0  mov        edx, dword ptr [edi + 0x58]
0042afd3  mov        dword ptr [esp + 0xc], eax
0042afd7  mov        dword ptr [esp + 0x10], ecx
0042afdb  fnstsw     ax
0042afdd  mov        dword ptr [esp + 0x14], edx
0042afe1  test       ah, 0x41
0042afe4  jne        0x42aff1

// block 0042afe6
0042afe6  fld        dword ptr [edi + 0x70]
0042afe9  fmul       qword ptr [0x4a3cf8]
0042afef  jmp        0x42b004

// block 0042aff1
0042aff1  fld        dword ptr [edi + 0x70]
0042aff4  fld        qword ptr [0x4a3d68]
0042affa  fadd       st(1)
0042affc  fdiv       qword ptr [0x4a3fd8]
0042b002  fsubp      st(1)

// block 0042b004
0042b004  fld        dword ptr [edi + 0x6c]
0042b007  sub        esp, 0xc
0042b00a  fstp       dword ptr [esp + 8]
0042b00e  lea        eax, [esp + 0x18]
0042b012  fstp       dword ptr [esp + 0x14]
0042b016  fld        dword ptr [esp + 0x14]
0042b01a  fstp       dword ptr [esp + 4]
0042b01e  fld        dword ptr [edi + 0x68]
0042b021  fstp       dword ptr [esp]
0042b024  call       0x437a80

// block 0042b029
0042b029  cmp        eax, 1
0042b02c  jne        0x42b063

// block 0042b02e
0042b02e  fld        dword ptr [0x4a3d78]
0042b034  mov        edx, dword ptr [0x4b4514]
0042b03a  mov        eax, dword ptr [edi]
0042b03c  fst        dword ptr [esp + 0xc]
0042b040  mov        eax, dword ptr [eax + 0x18]
0042b043  fstp       dword ptr [esp + 0x10]
0042b047  fldz       
0042b049  push       1
0042b04b  push       0
0042b04d  fstp       dword ptr [esp + 0x1c]
0042b051  lea        ecx, [esp + 0x14]
0042b055  push       ecx
0042b056  add        edx, 0x97c
0042b05c  push       edx
0042b05d  mov        ecx, edi
0042b05f  call       eax

// block 0042b061
0042b061  jmp        0x42b089

// block 0042b063
0042b063  cmp        eax, 2
0042b066  jne        0x42b089

// block 0042b068
0042b068  mov        eax, dword ptr [edi + 0x18]
0042b06b  cdq        
0042b06c  mov        ecx, 3
0042b071  idiv       ecx
0042b073  test       edx, edx
0042b075  jne        0x42b089

// block 0042b077
0042b077  mov        edx, dword ptr [0x4b4514]
0042b07d  add        edx, 0x97c
0042b083  push       edx
0042b084  call       0x4391c0

// block 0042b089
0042b089  mov        ecx, dword ptr [edi + 0xa54]
0042b08f  fld        dword ptr [edi + 0x70]
0042b092  fdiv       dword ptr [ecx + 0x38]
0042b095  lea        eax, [edi + 0x660]
0042b09b  mov        ecx, 8
0042b0a0  or         dword ptr [eax + 0x47c], ecx
0042b0a6  push       eax
0042b0a7  fstp       dword ptr [eax + 0x40]
0042b0aa  fld        dword ptr [edi + 0x6c]
0042b0ad  mov        edx, dword ptr [edi + 0xa54]
0042b0b3  fdiv       dword ptr [edx + 0x34]
0042b0b6  or         dword ptr [eax + 0x47c], ecx
0042b0bc  fstp       dword ptr [eax + 0x44]
0042b0bf  call       0x455630

// block 0042b0c4
0042b0c4  fldz       
0042b0c6  fcomp      dword ptr [edi + 0x78]
0042b0c9  fnstsw     ax
0042b0cb  test       ah, 0x44
0042b0ce  jp         0x42b0dc

// block 0042b0d0
0042b0d0  add        edi, 0xb14
0042b0d6  push       edi
0042b0d7  call       0x455630

// block 0042b0dc
0042b0dc  pop        edi
0042b0dd  xor        eax, eax
0042b0df  pop        esi
0042b0e0  mov        esp, ebp
0042b0e2  pop        ebp
0042b0e3  ret        
