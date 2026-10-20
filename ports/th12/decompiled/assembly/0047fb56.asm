
// block 0047fb56
0047fb56  mov        edi, edi
0047fb58  push       ebp
0047fb59  mov        ebp, esp
0047fb5b  sub        esp, 0xa0
0047fb61  mov        eax, dword ptr [0x4b4318]
0047fb66  movsx      edx, byte ptr [eax]
0047fb69  push       ebx
0047fb6a  xor        ebx, ebx
0047fb6c  mov        ecx, 0xffff0000
0047fb71  and        dword ptr [ebp - 4], ecx
0047fb74  and        dword ptr [ebp - 0x14], ecx
0047fb77  push       esi
0047fb78  xor        esi, esi
0047fb7a  inc        eax
0047fb7b  cmp        edx, 0x41
0047fb7e  mov        dword ptr [ebp - 8], ebx
0047fb81  mov        dword ptr [ebp - 0x18], ebx
0047fb84  mov        dword ptr [0x4b4318], eax
0047fb89  jg         0x47fcf4

// block 0047fb8f
0047fb8f  je         0x4800c3

// block 0047fb95
0047fb95  cmp        edx, ebx
0047fb97  je         0x47fcdc

// block 0047fb9d
0047fb9d  cmp        edx, 0x2f
0047fba0  jle        0x47ffc2

// block 0047fba6
0047fba6  cmp        edx, 0x31
0047fba9  jle        0x47fbfe

// block 0047fbab
0047fbab  cmp        edx, 0x39
0047fbae  jg         0x47ffc2

// block 0047fbb4
0047fbb4  movsx      eax, byte ptr [eax - 1]
0047fbb8  push       dword ptr [eax*4 + 0x49dba8]

// block 0047fbbf
0047fbbf  lea        ecx, [ebp - 8]
0047fbc2  call       0x47e896

// block 0047fbc7
0047fbc7  cmp        dword ptr [ebp - 8], ebx
0047fbca  je         0x47fbec

// block 0047fbcc
0047fbcc  lea        eax, [ebp - 8]
0047fbcf  push       eax
0047fbd0  lea        eax, [ebp - 0x20]
0047fbd3  push       0x49db88
0047fbd8  push       eax
0047fbd9  call       0x47ec4a

// block 0047fbde
0047fbde  mov        ecx, dword ptr [eax]
0047fbe0  mov        eax, dword ptr [eax + 4]
0047fbe3  add        esp, 0xc
0047fbe6  mov        dword ptr [ebp - 8], ecx
0047fbe9  mov        dword ptr [ebp - 4], eax

// block 0047fbec
0047fbec  mov        ecx, dword ptr [ebp - 8]

// block 0047fbef
0047fbef  mov        eax, dword ptr [ebp + 8]
0047fbf2  mov        dword ptr [eax], ecx
0047fbf4  mov        ecx, dword ptr [ebp - 4]

// block 0047fbf7
0047fbf7  mov        dword ptr [eax + 4], ecx

// block 0047fbfa
0047fbfa  pop        esi
0047fbfb  pop        ebx
0047fbfc  leave      
0047fbfd  ret        

// block 0047fbfe
0047fbfe  and        dword ptr [ebp - 0x14], ecx
0047fc01  mov        dword ptr [ebp - 0x18], ebx
0047fc04  cmp        byte ptr [ebp + 0xc], bl
0047fc07  je         0x47fc77

// block 0047fc09
0047fc09  lea        eax, [ebp - 0x58]
0047fc0c  push       eax
0047fc0d  call       0x47f992

// block 0047fc12
0047fc12  push       eax
0047fc13  lea        eax, [ebp - 0xa0]
0047fc19  push       0x3c
0047fc1b  push       eax
0047fc1c  call       0x47ec02

// block 0047fc21
0047fc21  add        esp, 0x10
0047fc24  push       eax
0047fc25  lea        ecx, [ebp - 0x18]
0047fc28  call       0x47e7bf

// block 0047fc2d
0047fc2d  mov        ecx, dword ptr [ebp - 0x18]
0047fc30  cmp        ecx, ebx
0047fc32  je         0x47fc47

// block 0047fc34
0047fc34  mov        eax, dword ptr [ecx]
0047fc36  call       dword ptr [eax + 4]

// block 0047fc39
0047fc39  cmp        al, 0x3e
0047fc3b  jne        0x47fc47

// block 0047fc3d
0047fc3d  push       0x20
0047fc3f  lea        ecx, [ebp - 0x18]
0047fc42  call       0x47e9f6

// block 0047fc47
0047fc47  push       0x3e
0047fc49  lea        ecx, [ebp - 0x18]
0047fc4c  call       0x47e9f6

// block 0047fc51
0047fc51  mov        eax, dword ptr [ebp + 0x10]
0047fc54  cmp        eax, ebx
0047fc56  je         0x47fc5b

// block 0047fc58
0047fc58  mov        byte ptr [eax], 1

// block 0047fc5b
0047fc5b  mov        eax, dword ptr [0x4b4318]
0047fc60  cmp        byte ptr [eax], bl
0047fc62  jne        0x47fc71

// block 0047fc64
0047fc64  mov        ecx, dword ptr [ebp - 0x18]
0047fc67  mov        eax, dword ptr [ebp + 8]
0047fc6a  mov        dword ptr [eax], ecx
0047fc6c  mov        ecx, dword ptr [ebp - 0x14]
0047fc6f  jmp        0x47fbf7

// block 0047fc71
0047fc71  inc        eax
0047fc72  mov        dword ptr [0x4b4318], eax

// block 0047fc77
0047fc77  push       ebx
0047fc78  mov        esi, eax
0047fc7a  lea        eax, [ebp - 0x38]
0047fc7d  push       ebx
0047fc7e  push       eax
0047fc7f  call       0x48023a

// block 0047fc84
0047fc84  mov        ecx, dword ptr [eax]
0047fc86  mov        eax, dword ptr [eax + 4]
0047fc89  add        esp, 0xc
0047fc8c  mov        dword ptr [ebp - 8], ecx
0047fc8f  mov        dword ptr [ebp - 4], eax
0047fc92  mov        dword ptr [0x4b4318], esi
0047fc98  cmp        ecx, ebx
0047fc9a  je         0x47fcc2

// block 0047fc9c
0047fc9c  cmp        byte ptr [esi - 1], 0x31
0047fca0  jne        0x47fcc2

// block 0047fca2
0047fca2  lea        eax, [ebp - 8]
0047fca5  push       eax
0047fca6  lea        eax, [ebp - 0x88]
0047fcac  push       0x7e
0047fcae  push       eax
0047fcaf  call       0x47ec02

// block 0047fcb4
0047fcb4  mov        ecx, dword ptr [eax]
0047fcb6  mov        eax, dword ptr [eax + 4]
0047fcb9  add        esp, 0xc
0047fcbc  mov        dword ptr [ebp - 8], ecx
0047fcbf  mov        dword ptr [ebp - 4], eax

// block 0047fcc2
0047fcc2  cmp        dword ptr [ebp - 0x18], ebx
0047fcc5  je         0x47fbef

// block 0047fccb
0047fccb  lea        eax, [ebp - 0x18]
0047fcce  push       eax
0047fccf  lea        ecx, [ebp - 8]
0047fcd2  call       0x47e7bf

// block 0047fcd7
0047fcd7  jmp        0x47fbec

// block 0047fcdc
0047fcdc  dec        eax
0047fcdd  mov        dword ptr [0x4b4318], eax

// block 0047fce2
0047fce2  push       1

// block 0047fce4
0047fce4  mov        ecx, dword ptr [ebp + 8]
0047fce7  call       0x47e1ce

// block 0047fcec
0047fcec  mov        eax, dword ptr [ebp + 8]
0047fcef  jmp        0x47fbfa

// block 0047fcf4
0047fcf4  cmp        edx, 0x42
0047fcf7  je         0x4800c0

// block 0047fcfd
0047fcfd  jle        0x47ffc2

// block 0047fd03
0047fd03  cmp        edx, 0x5a
0047fd06  jle        0x4800c3

// block 0047fd0c
0047fd0c  cmp        edx, 0x5f
0047fd0f  jne        0x47ffc2

// block 0047fd15
0047fd15  movsx      ecx, byte ptr [eax]
0047fd18  inc        eax
0047fd19  cmp        ecx, 0x4f
0047fd1c  mov        dword ptr [0x4b4318], eax
0047fd21  jg         0x47fe00

// block 0047fd27
0047fd27  cmp        ecx, 0x44
0047fd2a  jge        0x4800a0

// block 0047fd30
0047fd30  cmp        ecx, 0x39
0047fd33  jg         0x47fda0

// block 0047fd35
0047fd35  je         0x47fd77

// block 0047fd37
0047fd37  cmp        ecx, ebx
0047fd39  je         0x47fcdc

// block 0047fd3b
0047fd3b  cmp        ecx, 0x2f
0047fd3e  jle        0x47ffc2

// block 0047fd44
0047fd44  cmp        ecx, 0x36
0047fd47  jle        0x47fd67

// block 0047fd49
0047fd49  cmp        ecx, 0x38
0047fd4c  jg         0x47ffc2

// block 0047fd52
0047fd52  movsx      eax, byte ptr [eax - 1]
0047fd56  push       dword ptr [eax*4 + 0x49dc38]

// block 0047fd5d
0047fd5d  mov        ecx, dword ptr [ebp + 8]
0047fd60  call       0x47e59a

// block 0047fd65
0047fd65  jmp        0x47fcec

// block 0047fd67
0047fd67  movsx      eax, byte ptr [eax - 1]
0047fd6b  push       dword ptr [eax*4 + 0x49dc38]
0047fd72  jmp        0x47fbbf

// block 0047fd77
0047fd77  movsx      eax, byte ptr [eax - 1]
0047fd7b  push       dword ptr [eax*4 + 0x49dc38]
0047fd82  lea        ecx, [ebp - 0x10]
0047fd85  call       0x47e59a

// block 0047fd8a
0047fd8a  mov        ecx, dword ptr [ebp - 0xc]
0047fd8d  or         ecx, 0x8000

// block 0047fd93
0047fd93  mov        edx, dword ptr [ebp - 0x10]
0047fd96  mov        eax, dword ptr [ebp + 8]
0047fd99  mov        dword ptr [eax], edx
0047fd9b  jmp        0x47fbf7

// block 0047fda0
0047fda0  cmp        ecx, 0x3f
0047fda3  je         0x47fdde

// block 0047fda5
0047fda5  cmp        ecx, 0x40
0047fda8  jle        0x47ffc2

// block 0047fdae
0047fdae  cmp        ecx, 0x42
0047fdb1  jle        0x4800a0

// block 0047fdb7
0047fdb7  cmp        ecx, 0x43
0047fdba  jne        0x47ffc2

// block 0047fdc0
0047fdc0  push       1
0047fdc2  push       0x49dac4

// block 0047fdc7
0047fdc7  lea        eax, [ebp - 0x10]
0047fdca  push       eax
0047fdcb  call       0x47f4da

// block 0047fdd0
0047fdd0  mov        ecx, dword ptr [ebp - 0xc]
0047fdd3  add        esp, 0xc
0047fdd6  or         ecx, 0x1000
0047fddc  jmp        0x47fd93

// block 0047fdde
0047fdde  movsx      ecx, byte ptr [eax]
0047fde1  inc        eax
0047fde2  mov        dword ptr [0x4b4318], eax
0047fde7  cmp        ecx, ebx
0047fde9  je         0x47fcdc

// block 0047fdef
0047fdef  cmp        ecx, 0x30
0047fdf2  jne        0x47ffc2

// block 0047fdf8
0047fdf8  push       ebx
0047fdf9  push       0x49dedc
0047fdfe  jmp        0x47fdc7

// block 0047fe00
0047fe00  cmp        ecx, 0x54
0047fe03  jg         0x47ffdd

// block 0047fe09
0047fe09  cmp        ecx, 0x53
0047fe0c  jge        0x4800a0

// block 0047fe12
0047fe12  sub        ecx, 0x50
0047fe15  je         0x47ff8b

// block 0047fe1b
0047fe1b  dec        ecx
0047fe1c  je         0x47fbec

// block 0047fe22
0047fe22  dec        ecx
0047fe23  jne        0x47ffc2

// block 0047fe29
0047fe29  movsx      eax, byte ptr [eax - 1]
0047fe2d  push       dword ptr [eax*4 + 0x49dc1c]
0047fe34  lea        ecx, [ebp - 8]
0047fe37  call       0x47e896

// block 0047fe3c
0047fe3c  mov        eax, dword ptr [0x4b4318]
0047fe41  mov        al, byte ptr [eax]
0047fe43  cmp        al, bl
0047fe45  jne        0x47fe59

// block 0047fe47
0047fe47  push       1
0047fe49  push       dword ptr [ebp + 8]
0047fe4c  lea        ecx, [ebp - 8]
0047fe4f  call       0x47e79b

// block 0047fe54
0047fe54  jmp        0x47fcec

// block 0047fe59
0047fe59  movsx      eax, al
0047fe5c  sub        eax, 0x30
0047fe5f  js         0x47ffc2

// block 0047fe65
0047fe65  cmp        eax, 5
0047fe68  jae        0x47ffc2

// block 0047fe6e
0047fe6e  push       dword ptr [eax*4 + 0x49dc5c]
0047fe75  lea        ecx, [ebp - 0x18]
0047fe78  call       0x47e896

// block 0047fe7d
0047fe7d  mov        eax, dword ptr [0x4b4318]
0047fe82  movsx      eax, byte ptr [eax]
0047fe85  inc        dword ptr [0x4b4318]
0047fe8b  cmp        eax, 0x30
0047fe8e  je         0x47ff57

// block 0047fe94
0047fe94  cmp        eax, 0x31
0047fe97  je         0x47feb0

// block 0047fe99
0047fe99  add        eax, -0x32
0047fe9c  cmp        eax, 2
0047fe9f  jbe        0x47ffc9

// block 0047fea5
0047fea5  dec        dword ptr [0x4b4318]
0047feab  jmp        0x47fce2

// block 0047feb0
0047feb0  lea        eax, [ebp - 0x18]
0047feb3  push       eax
0047feb4  lea        eax, [ebp - 0x10]
0047feb7  push       eax
0047feb8  lea        ecx, [ebp - 8]
0047febb  call       0x47e9ae

// block 0047fec0
0047fec0  push       0x2c
0047fec2  lea        eax, [ebp - 0x48]
0047fec5  push       eax
0047fec6  lea        eax, [ebp - 0x78]
0047fec9  push       eax
0047feca  call       0x47f57e

// block 0047fecf
0047fecf  pop        ecx
0047fed0  mov        ecx, eax
0047fed2  call       0x47ec6e

// block 0047fed7
0047fed7  push       eax
0047fed8  lea        ecx, [ebp - 0x10]
0047fedb  call       0x47e7bf

// block 0047fee0
0047fee0  push       0x2c
0047fee2  lea        eax, [ebp - 0x28]
0047fee5  push       eax
0047fee6  lea        eax, [ebp - 0x98]
0047feec  push       eax
0047feed  call       0x47f57e

// block 0047fef2
0047fef2  pop        ecx
0047fef3  mov        ecx, eax
0047fef5  call       0x47ec6e

// block 0047fefa
0047fefa  push       eax
0047fefb  lea        ecx, [ebp - 0x10]
0047fefe  call       0x47e7bf

// block 0047ff03
0047ff03  push       0x2c
0047ff05  lea        eax, [ebp - 0x68]
0047ff08  push       eax
0047ff09  lea        eax, [ebp - 0x30]
0047ff0c  push       eax
0047ff0d  call       0x47f57e

// block 0047ff12
0047ff12  pop        ecx
0047ff13  mov        ecx, eax
0047ff15  call       0x47ec6e

// block 0047ff1a
0047ff1a  push       eax
0047ff1b  lea        ecx, [ebp - 0x10]
0047ff1e  call       0x47e7bf

// block 0047ff23
0047ff23  push       0x29
0047ff25  lea        eax, [ebp - 0x40]
0047ff28  push       eax
0047ff29  lea        eax, [ebp - 0x50]
0047ff2c  push       ebx
0047ff2d  push       eax
0047ff2e  call       0x47ecb6

// block 0047ff33
0047ff33  pop        ecx
0047ff34  pop        ecx
0047ff35  mov        ecx, eax
0047ff37  call       0x47ec6e

// block 0047ff3c
0047ff3c  push       eax
0047ff3d  lea        ecx, [ebp - 0x10]
0047ff40  call       0x47e7bf

// block 0047ff45
0047ff45  push       0x27
0047ff47  push       dword ptr [ebp + 8]
0047ff4a  lea        ecx, [ebp - 0x10]
0047ff4d  call       0x47ec6e

// block 0047ff52
0047ff52  jmp        0x47fcec

// block 0047ff57
0047ff57  lea        eax, [ebp - 0x10]
0047ff5a  push       ebx
0047ff5b  push       eax
0047ff5c  call       0x4827df

// block 0047ff61
0047ff61  pop        ecx
0047ff62  pop        ecx
0047ff63  lea        eax, [ebp - 0x18]
0047ff66  push       eax
0047ff67  push       dword ptr [ebp + 8]
0047ff6a  lea        eax, [ebp - 8]
0047ff6d  push       eax
0047ff6e  lea        eax, [ebp - 0x60]
0047ff71  push       eax
0047ff72  push       0x20
0047ff74  lea        eax, [ebp - 0x70]
0047ff77  push       eax
0047ff78  lea        ecx, [ebp - 0x10]
0047ff7b  call       0x47ec6e

// block 0047ff80
0047ff80  mov        ecx, eax
0047ff82  call       0x47e9ae

// block 0047ff87
0047ff87  mov        ecx, eax
0047ff89  jmp        0x47ffd3

// block 0047ff8b
0047ff8b  movsx      eax, byte ptr [eax - 1]
0047ff8f  push       dword ptr [eax*4 + 0x49dc1c]
0047ff96  lea        ecx, [ebp - 8]
0047ff99  call       0x47e896

// block 0047ff9e
0047ff9e  push       ebx
0047ff9f  lea        eax, [ebp - 0x80]
0047ffa2  push       ebx
0047ffa3  push       eax
0047ffa4  call       0x47fb56

// block 0047ffa9
0047ffa9  mov        ecx, dword ptr [eax]
0047ffab  mov        eax, dword ptr [eax + 4]
0047ffae  add        esp, 0xc
0047ffb1  mov        dword ptr [ebp - 0x18], ecx
0047ffb4  mov        dword ptr [ebp - 0x14], eax
0047ffb7  cmp        ecx, ebx
0047ffb9  je         0x47ffc9

// block 0047ffbb
0047ffbb  test       eax, 0x400
0047ffc0  je         0x47ffc9

// block 0047ffc2
0047ffc2  push       2
0047ffc4  jmp        0x47fce4

// block 0047ffc9
0047ffc9  lea        eax, [ebp - 0x18]
0047ffcc  push       eax
0047ffcd  push       dword ptr [ebp + 8]
0047ffd0  lea        ecx, [ebp - 8]

// block 0047ffd3
0047ffd3  call       0x47e9ae

// block 0047ffd8
0047ffd8  jmp        0x47fcec

// block 0047ffdd
0047ffdd  cmp        ecx, 0x55
0047ffe0  jl         0x47ffc2

// block 0047ffe2
0047ffe2  cmp        ecx, 0x56
0047ffe5  jle        0x4800b0

// block 0047ffeb
0047ffeb  cmp        ecx, 0x57
0047ffee  jle        0x47ffc2

// block 0047fff0
0047fff0  cmp        ecx, 0x59
0047fff3  jle        0x4800a0

// block 0047fff9
0047fff9  cmp        ecx, 0x5f
0047fffc  jne        0x47ffc2

// block 0047fffe
0047fffe  movsx      ecx, byte ptr [eax]
00480001  inc        eax
00480002  cmp        ecx, 0x41
00480005  mov        dword ptr [0x4b4318], eax
0048000a  jl         0x47ffc2

// block 0048000c
0048000c  cmp        ecx, 0x44
0048000f  jle        0x48001b

// block 00480011
00480011  cmp        ecx, 0x46
00480014  jle        0x48002b

// block 00480016
00480016  cmp        ecx, 0x4a
00480019  jg         0x47ffc2

// block 0048001b
0048001b  movsx      eax, byte ptr [eax - 1]
0048001f  push       dword ptr [eax*4 + 0x49dc80]
00480026  jmp        0x47fd5d

// block 0048002b
0048002b  movsx      eax, byte ptr [eax - 1]
0048002f  push       dword ptr [eax*4 + 0x49dc80]
00480036  lea        ecx, [ebp - 0x10]
00480039  call       0x47e59a

// block 0048003e
0048003e  mov        eax, dword ptr [0x4b4318]
00480043  cmp        byte ptr [eax], 0x3f
00480046  jne        0x480070

// block 00480048
00480048  lea        eax, [ebp - 0x90]
0048004e  push       eax
0048004f  call       0x481298

// block 00480054
00480054  pop        ecx
00480055  push       eax
00480056  lea        ecx, [ebp - 0x10]
00480059  call       0x47e7bf

// block 0048005e
0048005e  mov        eax, dword ptr [0x4b4318]
00480063  cmp        byte ptr [eax], 0x40
00480066  jne        0x480083

// block 00480068
00480068  inc        dword ptr [0x4b4318]
0048006e  jmp        0x480083

// block 00480070
00480070  lea        eax, [ebp - 0x20]
00480073  push       eax
00480074  call       0x48061b

// block 00480079
00480079  pop        ecx
0048007a  push       eax
0048007b  lea        ecx, [ebp - 0x10]
0048007e  call       0x47e7bf

// block 00480083
00480083  push       0x49ded8
00480088  lea        ecx, [ebp - 0x10]
0048008b  call       0x47ea48

// block 00480090
00480090  mov        ecx, dword ptr [ebp - 0x10]
00480093  mov        eax, dword ptr [ebp + 8]
00480096  mov        dword ptr [eax], ecx
00480098  mov        ecx, dword ptr [ebp - 0xc]
0048009b  jmp        0x47fbf7

// block 004800a0
004800a0  movsx      eax, byte ptr [eax - 1]
004800a4  push       dword ptr [eax*4 + 0x49dc1c]
004800ab  jmp        0x47fd5d

// block 004800b0
004800b0  movsx      eax, byte ptr [eax - 1]
004800b4  push       dword ptr [eax*4 + 0x49dc1c]
004800bb  jmp        0x47fbbf

// block 004800c0
004800c0  xor        esi, esi
004800c2  inc        esi

// block 004800c3
004800c3  movsx      eax, byte ptr [eax - 1]
004800c7  push       dword ptr [eax*4 + 0x49db8c]
004800ce  lea        ecx, [ebp - 8]
004800d1  call       0x47e896

// block 004800d6
004800d6  cmp        esi, ebx
004800d8  je         0x47fbc7

// block 004800de
004800de  cmp        dword ptr [ebp - 8], ebx
004800e1  je         0x47fbec

// block 004800e7
004800e7  or         dword ptr [ebp - 4], 0x200
004800ee  jmp        0x47fbec
