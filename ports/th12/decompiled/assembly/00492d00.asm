
// block 00492d00
00492d00  mov        edi, edi
00492d02  push       ebp
00492d03  mov        ebp, esp
00492d05  sub        esp, 0x2c
00492d08  mov        ecx, dword ptr [ebp + 0xc]
00492d0b  push       ebx
00492d0c  mov        ebx, dword ptr [ebp + 0x18]
00492d0f  mov        eax, dword ptr [ebx + 4]
00492d12  cmp        eax, 0x80
00492d17  push       esi
00492d18  push       edi
00492d19  mov        byte ptr [ebp - 1], 0
00492d1d  jg         0x492d25

// block 00492d1f
00492d1f  movsx      ecx, byte ptr [ecx + 8]
00492d23  jmp        0x492d28

// block 00492d25
00492d25  mov        ecx, dword ptr [ecx + 8]

// block 00492d28
00492d28  cmp        ecx, -1
00492d2b  mov        dword ptr [ebp - 8], ecx
00492d2e  jl         0x492d34

// block 00492d30
00492d30  cmp        ecx, eax
00492d32  jl         0x492d39

// block 00492d34
00492d34  call       0x472b94

// block 00492d39
00492d39  mov        esi, dword ptr [ebp + 8]
00492d3c  mov        edi, 0xe06d7363
00492d41  cmp        dword ptr [esi], edi
00492d43  jne        0x493003

// block 00492d49
00492d49  cmp        dword ptr [esi + 0x10], 3
00492d4d  mov        ebx, 0x19930520
00492d52  jne        0x492e70

// block 00492d58
00492d58  mov        eax, dword ptr [esi + 0x14]
00492d5b  cmp        eax, ebx
00492d5d  je         0x492d71

// block 00492d5f
00492d5f  cmp        eax, 0x19930521
00492d64  je         0x492d71

// block 00492d66
00492d66  cmp        eax, 0x19930522
00492d6b  jne        0x492e70

// block 00492d71
00492d71  cmp        dword ptr [esi + 0x1c], 0
00492d75  jne        0x492e70

// block 00492d7b
00492d7b  call       0x475467

// block 00492d80
00492d80  cmp        dword ptr [eax + 0x88], 0
00492d87  je         0x493042

// block 00492d8d
00492d8d  call       0x475467

// block 00492d92
00492d92  mov        esi, dword ptr [eax + 0x88]
00492d98  mov        dword ptr [ebp + 8], esi
00492d9b  call       0x475467

// block 00492da0
00492da0  mov        eax, dword ptr [eax + 0x8c]
00492da6  push       1
00492da8  push       esi
00492da9  mov        dword ptr [ebp + 0x10], eax
00492dac  call       0x49319c

// block 00492db1
00492db1  pop        ecx
00492db2  pop        ecx
00492db3  test       eax, eax
00492db5  jne        0x492dbc

// block 00492db7
00492db7  call       0x472b94

// block 00492dbc
00492dbc  cmp        dword ptr [esi], edi
00492dbe  jne        0x492de6

// block 00492dc0
00492dc0  cmp        dword ptr [esi + 0x10], 3
00492dc4  jne        0x492de6

// block 00492dc6
00492dc6  mov        eax, dword ptr [esi + 0x14]
00492dc9  cmp        eax, ebx
00492dcb  je         0x492ddb

// block 00492dcd
00492dcd  cmp        eax, 0x19930521
00492dd2  je         0x492ddb

// block 00492dd4
00492dd4  cmp        eax, 0x19930522
00492dd9  jne        0x492de6

// block 00492ddb
00492ddb  cmp        dword ptr [esi + 0x1c], 0
00492ddf  jne        0x492de6

// block 00492de1
00492de1  call       0x472b94

// block 00492de6
00492de6  call       0x475467

// block 00492deb
00492deb  cmp        dword ptr [eax + 0x94], 0
00492df2  je         0x492e70

// block 00492df4
00492df4  call       0x475467

// block 00492df9
00492df9  mov        edi, dword ptr [eax + 0x94]
00492dff  call       0x475467

// block 00492e04
00492e04  push       dword ptr [ebp + 8]
00492e07  xor        esi, esi
00492e09  mov        dword ptr [eax + 0x94], esi
00492e0f  call       0x492530

// block 00492e14
00492e14  pop        ecx
00492e15  test       al, al
00492e17  jne        0x492e68

// block 00492e19
00492e19  xor        ebx, ebx
00492e1b  cmp        dword ptr [edi], ebx
00492e1d  jle        0x492e3c

// block 00492e1f
00492e1f  mov        eax, dword ptr [edi + 4]
00492e22  mov        ecx, dword ptr [ebx + eax + 4]
00492e26  push       0x4ae4d8
00492e2b  call       0x46cdd0

// block 00492e30
00492e30  test       al, al
00492e32  jne        0x492e41

// block 00492e34
00492e34  inc        esi
00492e35  add        ebx, 0x10
00492e38  cmp        esi, dword ptr [edi]
00492e3a  jl         0x492e1f

// block 00492e3c
00492e3c  call       0x472b48

// block 00492e41
00492e41  push       1
00492e43  push       dword ptr [ebp + 8]
00492e46  call       0x492181

// block 00492e4b
00492e4b  pop        ecx
00492e4c  pop        ecx
00492e4d  push       0x49f400
00492e52  lea        ecx, [ebp - 0x2c]
00492e55  call       0x491f63

// block 00492e5a
00492e5a  push       0x4ab554
00492e5f  lea        eax, [ebp - 0x2c]
00492e62  push       eax
00492e63  call       0x46ff8f

// block 00492e68
00492e68  mov        esi, dword ptr [ebp + 8]
00492e6b  mov        edi, 0xe06d7363

// block 00492e70
00492e70  cmp        dword ptr [esi], edi
00492e72  jne        0x493000

// block 00492e78
00492e78  cmp        dword ptr [esi + 0x10], 3
00492e7c  jne        0x493000

// block 00492e82
00492e82  mov        eax, dword ptr [esi + 0x14]
00492e85  cmp        eax, ebx
00492e87  je         0x492e9b

// block 00492e89
00492e89  cmp        eax, 0x19930521
00492e8e  je         0x492e9b

// block 00492e90
00492e90  cmp        eax, 0x19930522
00492e95  jne        0x493000

// block 00492e9b
00492e9b  mov        edi, dword ptr [ebp + 0x18]
00492e9e  cmp        dword ptr [edi + 0xc], 0
00492ea2  jbe        0x492f67

// block 00492ea8
00492ea8  lea        eax, [ebp - 0x1c]
00492eab  push       eax
00492eac  lea        eax, [ebp - 0x10]
00492eaf  push       eax
00492eb0  push       dword ptr [ebp - 8]
00492eb3  push       dword ptr [ebp + 0x20]
00492eb6  push       edi
00492eb7  call       0x491cdf

// block 00492ebc
00492ebc  add        esp, 0x14
00492ebf  mov        edi, eax

// block 00492ec1
00492ec1  mov        eax, dword ptr [ebp - 0x10]
00492ec4  cmp        eax, dword ptr [ebp - 0x1c]
00492ec7  jae        0x492f64

// block 00492ecd
00492ecd  mov        eax, dword ptr [ebp - 8]
00492ed0  cmp        dword ptr [edi], eax
00492ed2  jg         0x492f59

// block 00492ed8
00492ed8  cmp        eax, dword ptr [edi + 4]
00492edb  jg         0x492f59

// block 00492edd
00492edd  mov        eax, dword ptr [edi + 0x10]
00492ee0  mov        dword ptr [ebp - 0xc], eax
00492ee3  mov        eax, dword ptr [edi + 0xc]
00492ee6  mov        dword ptr [ebp - 0x18], eax
00492ee9  test       eax, eax
00492eeb  jle        0x492f59

// block 00492eed
00492eed  mov        eax, dword ptr [esi + 0x1c]
00492ef0  mov        eax, dword ptr [eax + 0xc]
00492ef3  lea        ebx, [eax + 4]
00492ef6  mov        eax, dword ptr [eax]
00492ef8  mov        dword ptr [ebp - 0x14], eax
00492efb  test       eax, eax
00492efd  jle        0x492f22

// block 00492eff
00492eff  push       dword ptr [esi + 0x1c]
00492f02  mov        eax, dword ptr [ebx]
00492f04  push       eax
00492f05  push       dword ptr [ebp - 0xc]
00492f08  mov        dword ptr [ebp - 0x20], eax
00492f0b  call       0x491fb3

// block 00492f10
00492f10  add        esp, 0xc
00492f13  test       eax, eax
00492f15  jne        0x492f31

// block 00492f17
00492f17  dec        dword ptr [ebp - 0x14]
00492f1a  add        ebx, 4
00492f1d  cmp        dword ptr [ebp - 0x14], eax
00492f20  jg         0x492eff

// block 00492f22
00492f22  dec        dword ptr [ebp - 0x18]
00492f25  add        dword ptr [ebp - 0xc], 0x10
00492f29  cmp        dword ptr [ebp - 0x18], 0
00492f2d  jg         0x492eed

// block 00492f2f
00492f2f  jmp        0x492f59

// block 00492f31
00492f31  push       dword ptr [ebp + 0x24]
00492f34  mov        ebx, dword ptr [ebp - 0xc]
00492f37  push       dword ptr [ebp + 0x20]
00492f3a  mov        byte ptr [ebp - 1], 1
00492f3e  push       dword ptr [ebp - 0x20]
00492f41  push       dword ptr [ebp + 0x18]
00492f44  push       dword ptr [ebp + 0x14]
00492f47  push       dword ptr [ebp + 0x10]
00492f4a  push       esi
00492f4b  mov        esi, dword ptr [ebp + 0xc]
00492f4e  call       0x492b9e

// block 00492f53
00492f53  mov        esi, dword ptr [ebp + 8]
00492f56  add        esp, 0x1c

// block 00492f59
00492f59  inc        dword ptr [ebp - 0x10]
00492f5c  add        edi, 0x14
00492f5f  jmp        0x492ec1

// block 00492f64
00492f64  mov        edi, dword ptr [ebp + 0x18]

// block 00492f67
00492f67  cmp        byte ptr [ebp + 0x1c], 0
00492f6b  je         0x492f77

// block 00492f6d
00492f6d  push       1
00492f6f  push       esi
00492f70  call       0x492181

// block 00492f75
00492f75  pop        ecx
00492f76  pop        ecx

// block 00492f77
00492f77  cmp        byte ptr [ebp - 1], 0
00492f7b  jne        0x49302f

// block 00492f81
00492f81  mov        eax, dword ptr [edi]
00492f83  and        eax, 0x1fffffff
00492f88  cmp        eax, 0x19930521
00492f8d  jb         0x49302f

// block 00492f93
00492f93  mov        edi, dword ptr [edi + 0x1c]
00492f96  test       edi, edi
00492f98  je         0x49302f

// block 00492f9e
00492f9e  push       esi
00492f9f  call       0x492530

// block 00492fa4
00492fa4  pop        ecx
00492fa5  test       al, al
00492fa7  jne        0x49302f

// block 00492fad
00492fad  call       0x475467

// block 00492fb2
00492fb2  call       0x475467

// block 00492fb7
00492fb7  call       0x475467

// block 00492fbc
00492fbc  mov        dword ptr [eax + 0x88], esi
00492fc2  call       0x475467

// block 00492fc7
00492fc7  cmp        dword ptr [ebp + 0x24], 0
00492fcb  mov        ecx, dword ptr [ebp + 0x10]
00492fce  mov        dword ptr [eax + 0x8c], ecx
00492fd4  push       esi
00492fd5  jne        0x492fdc

// block 00492fd7
00492fd7  push       dword ptr [ebp + 0xc]
00492fda  jmp        0x492fdf

// block 00492fdc
00492fdc  push       dword ptr [ebp + 0x24]

// block 00492fdf
00492fdf  call       0x491a21

// block 00492fe4
00492fe4  mov        esi, dword ptr [ebp + 0x18]
00492fe7  push       -1
00492fe9  push       esi
00492fea  push       dword ptr [ebp + 0x14]
00492fed  push       dword ptr [ebp + 0xc]
00492ff0  call       0x49205b

// block 00492ff5
00492ff5  add        esp, 0x10
00492ff8  push       dword ptr [esi + 0x1c]
00492ffb  call       0x4925ab

// block 00493000
00493000  mov        ebx, dword ptr [ebp + 0x18]

// block 00493003
00493003  cmp        dword ptr [ebx + 0xc], 0
00493007  jbe        0x49302f

// block 00493009
00493009  cmp        byte ptr [ebp + 0x1c], 0
0049300d  jne        0x492e3c

// block 00493013
00493013  push       dword ptr [ebp + 0x24]
00493016  push       dword ptr [ebp + 0x20]
00493019  push       dword ptr [ebp - 8]
0049301c  push       ebx
0049301d  push       dword ptr [ebp + 0x14]
00493020  push       dword ptr [ebp + 0x10]
00493023  push       dword ptr [ebp + 0xc]
00493026  push       esi
00493027  call       0x492c0c

// block 0049302c
0049302c  add        esp, 0x20

// block 0049302f
0049302f  call       0x475467

// block 00493034
00493034  cmp        dword ptr [eax + 0x94], 0
0049303b  je         0x493042

// block 0049303d
0049303d  call       0x472b94

// block 00493042
00493042  pop        edi
00493043  pop        esi
00493044  pop        ebx
00493045  leave      
00493046  ret        
