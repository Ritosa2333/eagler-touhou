
// block 00458e40
00458e40  sub        esp, 0x28
00458e43  mov        eax, dword ptr [edi + 0x34]
00458e46  push       esi
00458e47  mov        dword ptr [esp + 4], eax
00458e4b  test       eax, eax
00458e4d  jle        0x458eca

// block 00458e4f
00458e4f  lea        esi, [edi + 0x20]
00458e52  call       0x464a80

// block 00458e57
00458e57  mov        ecx, dword ptr [edi + 0x34]
00458e5a  cmp        dword ptr [edi + 0x24], ecx
00458e5d  mov        dword ptr [esp + 4], ecx
00458e61  jl         0x458eca

// block 00458e63
00458e63  mov        eax, dword ptr [esi + 0x10]
00458e66  mov        dword ptr [esp + 4], ecx
00458e6a  test       al, 1
00458e6c  jne        0x458e8d

// block 00458e6e
00458e6e  fldz       
00458e70  or         eax, 1
00458e73  fstp       dword ptr [esi + 8]
00458e76  mov        dword ptr [esi + 4], 0
00458e7d  mov        dword ptr [esi], 0xfff0bdc1
00458e83  mov        dword ptr [esi + 0xc], 0x4b2ed0
00458e8a  mov        dword ptr [esi + 0x10], eax

// block 00458e8d
00458e8d  fild       dword ptr [esp + 4]
00458e91  mov        dword ptr [esi + 4], ecx
00458e94  dec        ecx
00458e95  mov        dword ptr [esi], ecx
00458e97  fstp       dword ptr [esi + 8]
00458e9a  cmp        dword ptr [edi + 0x38], 7
00458e9e  mov        dword ptr [edi + 0x34], 0
00458ea5  je         0x458eb9

// block 00458ea7
00458ea7  mov        eax, dword ptr [edi + 8]
00458eaa  mov        ecx, dword ptr [edi + 0xc]
00458ead  mov        dword ptr [ebx], eax
00458eaf  mov        dword ptr [ebx + 4], ecx
00458eb2  mov        eax, ebx
00458eb4  pop        esi
00458eb5  add        esp, 0x28
00458eb8  ret        

// block 00458eb9
00458eb9  mov        eax, dword ptr [edi + 4]
00458ebc  mov        edx, dword ptr [edi]
00458ebe  mov        dword ptr [ebx + 4], eax
00458ec1  mov        dword ptr [ebx], edx
00458ec3  mov        eax, ebx
00458ec5  pop        esi
00458ec6  add        esp, 0x28
00458ec9  ret        

// block 00458eca
00458eca  mov        eax, dword ptr [edi + 0x38]
00458ecd  cmp        eax, 7
00458ed0  jne        0x458f0e

// block 00458ed2
00458ed2  mov        ecx, dword ptr [edi]
00458ed4  fld        dword ptr [edi + 8]
00458ed7  mov        edx, dword ptr [edi + 4]
00458eda  mov        dword ptr [esp + 0x1c], ecx
00458ede  fadd       dword ptr [esp + 0x1c]
00458ee2  mov        dword ptr [esp + 0x20], edx
00458ee6  fstp       dword ptr [esp + 0x14]
00458eea  mov        eax, dword ptr [esp + 0x14]
00458eee  fld        dword ptr [edi + 0xc]
00458ef1  mov        dword ptr [edi], eax
00458ef3  fadd       dword ptr [esp + 0x20]
00458ef7  mov        dword ptr [ebx], eax
00458ef9  mov        eax, ebx
00458efb  fstp       dword ptr [esp + 0x18]
00458eff  mov        ecx, dword ptr [esp + 0x18]
00458f03  mov        dword ptr [edi + 4], ecx
00458f06  mov        dword ptr [ebx + 4], ecx
00458f09  pop        esi
00458f0a  add        esp, 0x28
00458f0d  ret        

// block 00458f0e
00458f0e  cmp        eax, 0x11
00458f11  jne        0x458f71

// block 00458f13
00458f13  fld        dword ptr [edi + 0x18]
00458f16  mov        eax, dword ptr [edi]
00458f18  mov        ecx, dword ptr [edi + 4]
00458f1b  mov        dword ptr [esp + 0x1c], eax
00458f1f  fadd       dword ptr [esp + 0x1c]
00458f23  mov        dword ptr [esp + 0x20], ecx
00458f27  fstp       dword ptr [esp + 0x14]
00458f2b  mov        eax, dword ptr [esp + 0x14]
00458f2f  fld        dword ptr [edi + 0x1c]
00458f32  mov        dword ptr [edi], eax
00458f34  fadd       dword ptr [esp + 0x20]
00458f38  mov        dword ptr [ebx], eax
00458f3a  mov        eax, ebx
00458f3c  fstp       dword ptr [esp + 0x18]
00458f40  mov        ecx, dword ptr [esp + 0x18]
00458f44  mov        dword ptr [edi + 4], ecx
00458f47  fld        dword ptr [edi + 0x18]
00458f4a  fadd       dword ptr [edi + 8]
00458f4d  mov        dword ptr [ebx + 4], ecx
00458f50  fstp       dword ptr [esp + 0x1c]
00458f54  mov        edx, dword ptr [esp + 0x1c]
00458f58  fld        dword ptr [edi + 0x1c]
00458f5b  fadd       dword ptr [edi + 0xc]
00458f5e  mov        dword ptr [edi + 0x18], edx
00458f61  fstp       dword ptr [esp + 0x20]
00458f65  mov        edx, dword ptr [esp + 0x20]
00458f69  mov        dword ptr [edi + 0x1c], edx
00458f6c  pop        esi
00458f6d  add        esp, 0x28
00458f70  ret        

// block 00458f71
00458f71  cmp        eax, 8
00458f74  jne        0x459080

// block 00458f7a
00458f7a  fld        dword ptr [edi + 0x28]
00458f7d  fidiv      dword ptr [esp + 4]
00458f81  fstp       dword ptr [esp + 4]
00458f85  fld        dword ptr [esp + 4]
00458f89  fld        st(0)
00458f8b  fld1       
00458f8d  fsub       st(1), st(0)
00458f8f  fld        st(1)
00458f91  fld        st(3)
00458f93  fadd       st(0), st(0)
00458f95  fld        st(1)
00458f97  fmulp      st(2)
00458f99  fld        st(0)
00458f9b  fadd       st(3)
00458f9d  fmulp      st(2)
00458f9f  fxch       st(1)
00458fa1  fstp       dword ptr [esp + 0x1c]
00458fa5  fld        st(3)
00458fa7  fmul       st(0), st(0)
00458fa9  fld        qword ptr [0x4a3fd8]
00458faf  fsubrp     st(2)
00458fb1  fmulp      st(1)
00458fb3  fstp       dword ptr [esp + 0x14]
00458fb7  fsub       st(2)
00458fb9  fmul       st(0), st(0)
00458fbb  fmul       st(2)
00458fbd  fstp       dword ptr [esp + 0xc]
00458fc1  fmul       st(1)
00458fc3  fmulp      st(1)
00458fc5  fstp       dword ptr [esp + 4]
00458fc9  fld        dword ptr [esp + 4]
00458fcd  fld        st(0)
00458fcf  fmul       dword ptr [edi + 0x18]
00458fd2  fstp       dword ptr [esp + 0x24]
00458fd6  fmul       dword ptr [edi + 0x1c]
00458fd9  fstp       dword ptr [esp + 0x28]
00458fdd  fld        dword ptr [edi + 0x10]
00458fe0  fld        dword ptr [esp + 0xc]
00458fe4  fld        st(0)
00458fe6  fmulp      st(2)
00458fe8  fxch       st(1)
00458fea  fstp       dword ptr [esp + 4]
00458fee  fmul       dword ptr [edi + 0x14]
00458ff1  fstp       dword ptr [esp + 8]
00458ff5  fld        dword ptr [edi + 8]
00458ff8  fld        dword ptr [esp + 0x14]
00458ffc  fld        st(0)
00458ffe  fmulp      st(2)
00459000  fxch       st(1)
00459002  fstp       dword ptr [esp + 0x14]
00459006  fmul       dword ptr [edi + 0xc]
00459009  fstp       dword ptr [esp + 0x18]
0045900d  fld        dword ptr [edi]
0045900f  fld        dword ptr [esp + 0x1c]
00459013  fld        st(0)
00459015  fmulp      st(2)
00459017  fxch       st(1)
00459019  fstp       dword ptr [esp + 0x1c]
0045901d  fmul       dword ptr [edi + 4]
00459020  fstp       dword ptr [esp + 0x20]
00459024  fld        dword ptr [esp + 0x1c]
00459028  fadd       dword ptr [esp + 0x14]
0045902c  fstp       dword ptr [esp + 0xc]
00459030  fld        dword ptr [esp + 0x20]
00459034  fadd       dword ptr [esp + 0x18]
00459038  fstp       dword ptr [esp + 0x10]
0045903c  fld        dword ptr [esp + 0xc]
00459040  fadd       dword ptr [esp + 4]
00459044  fstp       dword ptr [esp + 0x1c]
00459048  fld        dword ptr [esp + 0x10]
0045904c  fadd       dword ptr [esp + 8]
00459050  fstp       dword ptr [esp + 0x20]
00459054  fld        dword ptr [esp + 0x1c]
00459058  fadd       dword ptr [esp + 0x24]
0045905c  fstp       dword ptr [esp + 0x14]
00459060  mov        eax, dword ptr [esp + 0x14]
00459064  fld        dword ptr [esp + 0x20]
00459068  mov        dword ptr [ebx], eax
0045906a  fadd       dword ptr [esp + 0x28]
0045906e  fstp       dword ptr [esp + 0x18]
00459072  mov        ecx, dword ptr [esp + 0x18]
00459076  mov        dword ptr [ebx + 4], ecx
00459079  mov        eax, ebx
0045907b  pop        esi
0045907c  add        esp, 0x28
0045907f  ret        

// block 00459080
00459080  mov        eax, edi
00459082  call       0x459360

// block 00459087
00459087  fstp       dword ptr [esp + 0x1c]
0045908b  fld        dword ptr [edi + 8]
0045908e  pop        esi
0045908f  fsub       dword ptr [edi]
00459091  fstp       dword ptr [esp + 0x20]
00459095  fld        dword ptr [edi + 0xc]
00459098  fsub       dword ptr [edi + 4]
0045909b  fstp       dword ptr [esp + 0x24]
0045909f  fld        dword ptr [esp + 0x20]
004590a3  fld        dword ptr [esp + 0x18]
004590a7  fld        st(0)
004590a9  fmulp      st(2)
004590ab  fxch       st(1)
004590ad  fstp       dword ptr [esp + 0x18]
004590b1  fmul       dword ptr [esp + 0x24]
004590b5  fstp       dword ptr [esp + 0x1c]
004590b9  fld        dword ptr [esp + 0x18]
004590bd  fadd       dword ptr [edi]
004590bf  fstp       dword ptr [esp + 0x20]
004590c3  mov        edx, dword ptr [esp + 0x20]
004590c7  fld        dword ptr [edi + 4]
004590ca  mov        dword ptr [ebx], edx
004590cc  fadd       dword ptr [esp + 0x1c]
004590d0  fstp       dword ptr [esp + 0x24]
004590d4  mov        eax, dword ptr [esp + 0x24]
004590d8  mov        dword ptr [ebx + 4], eax
004590db  mov        eax, ebx
004590dd  add        esp, 0x28
004590e0  ret        
