
// block 00475d72
00475d72  mov        edi, edi
00475d74  push       ebp
00475d75  mov        ebp, esp
00475d77  sub        esp, 0x44
00475d7a  mov        eax, dword ptr [0x4ad138]
00475d7f  xor        eax, ebp
00475d81  mov        dword ptr [ebp - 4], eax
00475d84  mov        eax, dword ptr [ebp + 8]
00475d87  mov        ecx, 0x4adfe0
00475d8c  sub        ecx, 0x60
00475d8f  xor        edx, edx
00475d91  mov        dword ptr [ebp - 0x24], ecx
00475d94  cmp        dword ptr [ebp + 0xc], edx
00475d97  je         0x47606b

// block 00475d9d
00475d9d  jge        0x475dad

// block 00475d9f
00475d9f  neg        dword ptr [ebp + 0xc]
00475da2  mov        ecx, 0x4ae140
00475da7  sub        ecx, 0x60
00475daa  mov        dword ptr [ebp - 0x24], ecx

// block 00475dad
00475dad  cmp        dword ptr [ebp + 0x10], edx
00475db0  jne        0x475db7

// block 00475db2
00475db2  xor        ecx, ecx
00475db4  mov        word ptr [eax], cx

// block 00475db7
00475db7  cmp        dword ptr [ebp + 0xc], edx
00475dba  je         0x47606b

// block 00475dc0
00475dc0  push       ebx
00475dc1  push       esi
00475dc2  push       edi

// block 00475dc3
00475dc3  mov        ecx, dword ptr [ebp + 0xc]
00475dc6  add        dword ptr [ebp - 0x24], 0x54
00475dca  sar        dword ptr [ebp + 0xc], 3
00475dce  and        ecx, 7
00475dd1  cmp        ecx, edx
00475dd3  je         0x47605d

// block 00475dd9
00475dd9  imul       ecx, ecx, 0xc
00475ddc  add        ecx, dword ptr [ebp - 0x24]
00475ddf  mov        ebx, ecx
00475de1  mov        ecx, 0x8000
00475de6  cmp        word ptr [ebx], cx
00475de9  jb         0x475df9

// block 00475deb
00475deb  mov        esi, ebx
00475ded  lea        edi, [ebp - 0x1c]
00475df0  movsd      dword ptr es:[edi], dword ptr [esi]
00475df1  movsd      dword ptr es:[edi], dword ptr [esi]
00475df2  movsd      dword ptr es:[edi], dword ptr [esi]
00475df3  dec        dword ptr [ebp - 0x1a]
00475df6  lea        ebx, [ebp - 0x1c]

// block 00475df9
00475df9  movzx      ecx, word ptr [eax + 0xa]
00475dfd  mov        dword ptr [ebp - 0x38], edx
00475e00  mov        dword ptr [ebp - 0x10], edx
00475e03  mov        dword ptr [ebp - 0xc], edx
00475e06  mov        dword ptr [ebp - 8], edx
00475e09  movzx      edx, word ptr [ebx + 0xa]
00475e0d  mov        esi, edx
00475e0f  xor        esi, ecx
00475e11  and        esi, 0x8000
00475e17  mov        dword ptr [ebp - 0x30], esi
00475e1a  mov        esi, 0x7fff
00475e1f  and        ecx, esi
00475e21  and        edx, esi
00475e23  lea        edi, [edx + ecx]
00475e26  movzx      edi, di
00475e29  mov        dword ptr [ebp - 0x28], edi
00475e2c  mov        edi, esi
00475e2e  cmp        cx, di
00475e31  jae        0x47603d

// block 00475e37
00475e37  cmp        dx, si
00475e3a  jae        0x47603d

// block 00475e40
00475e40  mov        edi, dword ptr [ebp - 0x28]
00475e43  mov        esi, 0xbffd
00475e48  cmp        di, si
00475e4b  ja         0x47603d

// block 00475e51
00475e51  mov        esi, 0x3fbf
00475e56  cmp        di, si
00475e59  ja         0x475e65

// block 00475e5b
00475e5b  xor        ecx, ecx
00475e5d  mov        dword ptr [eax + 8], ecx
00475e60  jmp        0x476058

// block 00475e65
00475e65  xor        esi, esi
00475e67  cmp        cx, si
00475e6a  jne        0x475e8d

// block 00475e6c
00475e6c  inc        edi
00475e6d  test       dword ptr [eax + 8], 0x7fffffff
00475e74  mov        dword ptr [ebp - 0x28], edi
00475e77  jne        0x475e8d

// block 00475e79
00475e79  cmp        dword ptr [eax + 4], esi
00475e7c  jne        0x475e8d

// block 00475e7e
00475e7e  cmp        dword ptr [eax], esi
00475e80  jne        0x475e8d

// block 00475e82
00475e82  xor        ecx, ecx
00475e84  mov        word ptr [eax + 0xa], cx
00475e88  jmp        0x47605d

// block 00475e8d
00475e8d  cmp        dx, si
00475e90  jne        0x475eb5

// block 00475e92
00475e92  inc        edi
00475e93  test       dword ptr [ebx + 8], 0x7fffffff
00475e9a  mov        dword ptr [ebp - 0x28], edi
00475e9d  jne        0x475eb5

// block 00475e9f
00475e9f  cmp        dword ptr [ebx + 4], esi
00475ea2  jne        0x475eb5

// block 00475ea4
00475ea4  cmp        dword ptr [ebx], esi
00475ea6  jne        0x475eb5

// block 00475ea8
00475ea8  mov        dword ptr [eax + 8], esi
00475eab  mov        dword ptr [eax + 4], esi
00475eae  mov        dword ptr [eax], esi
00475eb0  jmp        0x47605d

// block 00475eb5
00475eb5  mov        dword ptr [ebp - 0x34], esi
00475eb8  lea        esi, [ebp - 0xc]
00475ebb  mov        dword ptr [ebp - 0x20], 5

// block 00475ec2
00475ec2  mov        ecx, dword ptr [ebp - 0x34]
00475ec5  mov        edx, dword ptr [ebp - 0x20]
00475ec8  add        ecx, ecx
00475eca  mov        dword ptr [ebp - 0x2c], edx
00475ecd  test       edx, edx
00475ecf  jle        0x475f24

// block 00475ed1
00475ed1  lea        edx, [ebx + 8]
00475ed4  add        ecx, eax
00475ed6  mov        dword ptr [ebp - 0x40], edx
00475ed9  mov        dword ptr [ebp - 0x3c], ecx

// block 00475edc
00475edc  mov        ecx, dword ptr [ebp - 0x3c]
00475edf  mov        edx, dword ptr [ebp - 0x40]
00475ee2  movzx      edx, word ptr [edx]
00475ee5  movzx      ecx, word ptr [ecx]
00475ee8  and        dword ptr [ebp - 0x44], 0
00475eec  imul       ecx, edx
00475eef  mov        edx, dword ptr [esi - 4]
00475ef2  lea        edi, [edx + ecx]
00475ef5  cmp        edi, edx
00475ef7  jb         0x475efd

// block 00475ef9
00475ef9  cmp        edi, ecx
00475efb  jae        0x475f04

// block 00475efd
00475efd  mov        dword ptr [ebp - 0x44], 1

// block 00475f04
00475f04  cmp        dword ptr [ebp - 0x44], 0
00475f08  mov        dword ptr [esi - 4], edi
00475f0b  je         0x475f10

// block 00475f0d
00475f0d  inc        word ptr [esi]

// block 00475f10
00475f10  add        dword ptr [ebp - 0x3c], 2
00475f14  sub        dword ptr [ebp - 0x40], 2
00475f18  dec        dword ptr [ebp - 0x2c]
00475f1b  cmp        dword ptr [ebp - 0x2c], 0
00475f1f  jg         0x475edc

// block 00475f21
00475f21  mov        edi, dword ptr [ebp - 0x28]

// block 00475f24
00475f24  inc        esi
00475f25  inc        esi
00475f26  inc        dword ptr [ebp - 0x34]
00475f29  dec        dword ptr [ebp - 0x20]
00475f2c  cmp        dword ptr [ebp - 0x20], 0
00475f30  jg         0x475ec2

// block 00475f32
00475f32  add        edi, 0xc002
00475f38  test       di, di
00475f3b  jle        0x475f78

// block 00475f3d
00475f3d  test       dword ptr [ebp - 8], 0x80000000
00475f44  jne        0x475f73

// block 00475f46
00475f46  mov        esi, dword ptr [ebp - 0xc]
00475f49  mov        ecx, dword ptr [ebp - 0x10]
00475f4c  shl        dword ptr [ebp - 0x10], 1
00475f4f  shr        ecx, 0x1f
00475f52  mov        edx, esi
00475f54  add        esi, esi
00475f56  or         esi, ecx
00475f58  mov        ecx, dword ptr [ebp - 8]
00475f5b  shr        edx, 0x1f
00475f5e  add        ecx, ecx
00475f60  or         ecx, edx
00475f62  add        edi, 0xffff
00475f68  mov        dword ptr [ebp - 0xc], esi
00475f6b  mov        dword ptr [ebp - 8], ecx
00475f6e  test       di, di
00475f71  jg         0x475f3d

// block 00475f73
00475f73  test       di, di
00475f76  jg         0x475fc5

// block 00475f78
00475f78  add        edi, 0xffff
00475f7e  test       di, di
00475f81  jge        0x475fc5

// block 00475f83
00475f83  mov        ecx, edi
00475f85  neg        ecx
00475f87  movzx      esi, cx
00475f8a  add        edi, esi

// block 00475f8c
00475f8c  test       byte ptr [ebp - 0x10], 1
00475f90  je         0x475f95

// block 00475f92
00475f92  inc        dword ptr [ebp - 0x38]

// block 00475f95
00475f95  mov        ecx, dword ptr [ebp - 8]
00475f98  mov        ebx, dword ptr [ebp - 0xc]
00475f9b  mov        edx, dword ptr [ebp - 0xc]
00475f9e  shr        dword ptr [ebp - 8], 1
00475fa1  shl        ecx, 0x1f
00475fa4  shr        ebx, 1
00475fa6  or         ebx, ecx
00475fa8  mov        ecx, dword ptr [ebp - 0x10]
00475fab  shl        edx, 0x1f
00475fae  shr        ecx, 1
00475fb0  or         ecx, edx
00475fb2  dec        esi
00475fb3  mov        dword ptr [ebp - 0xc], ebx
00475fb6  mov        dword ptr [ebp - 0x10], ecx
00475fb9  jne        0x475f8c

// block 00475fbb
00475fbb  cmp        dword ptr [ebp - 0x38], esi
00475fbe  je         0x475fc5

// block 00475fc0
00475fc0  or         word ptr [ebp - 0x10], 1

// block 00475fc5
00475fc5  mov        ecx, 0x8000
00475fca  mov        edx, ecx
00475fcc  cmp        word ptr [ebp - 0x10], dx
00475fd0  ja         0x475fe3

// block 00475fd2
00475fd2  mov        edx, dword ptr [ebp - 0x10]
00475fd5  and        edx, 0x1ffff
00475fdb  cmp        edx, 0x18000
00475fe1  jne        0x476017

// block 00475fe3
00475fe3  cmp        dword ptr [ebp - 0xe], -1
00475fe7  jne        0x476014

// block 00475fe9
00475fe9  and        dword ptr [ebp - 0xe], 0
00475fed  cmp        dword ptr [ebp - 0xa], -1
00475ff1  jne        0x47600f

// block 00475ff3
00475ff3  and        dword ptr [ebp - 0xa], 0
00475ff7  mov        edx, 0xffff
00475ffc  cmp        word ptr [ebp - 6], dx
00476000  jne        0x476009

// block 00476002
00476002  mov        word ptr [ebp - 6], cx
00476006  inc        edi
00476007  jmp        0x476017

// block 00476009
00476009  inc        word ptr [ebp - 6]
0047600d  jmp        0x476017

// block 0047600f
0047600f  inc        dword ptr [ebp - 0xa]
00476012  jmp        0x476017

// block 00476014
00476014  inc        dword ptr [ebp - 0xe]

// block 00476017
00476017  mov        ecx, 0x7fff
0047601c  cmp        di, cx
0047601f  jae        0x47603d

// block 00476021
00476021  mov        cx, word ptr [ebp - 0xe]
00476025  or         edi, dword ptr [ebp - 0x30]
00476028  mov        word ptr [eax], cx
0047602b  mov        ecx, dword ptr [ebp - 0xc]
0047602e  mov        dword ptr [eax + 2], ecx
00476031  mov        ecx, dword ptr [ebp - 8]
00476034  mov        dword ptr [eax + 6], ecx
00476037  mov        word ptr [eax + 0xa], di
0047603b  jmp        0x47605d

// block 0047603d
0047603d  xor        edx, edx
0047603f  xor        ecx, ecx
00476041  cmp        word ptr [ebp - 0x30], cx
00476045  sete       dl
00476048  dec        edx
00476049  and        edx, 0x80000000
0047604f  add        edx, 0x7fff8000
00476055  mov        dword ptr [eax + 8], edx

// block 00476058
00476058  mov        dword ptr [eax + 4], ecx
0047605b  mov        dword ptr [eax], ecx

// block 0047605d
0047605d  xor        edx, edx
0047605f  cmp        dword ptr [ebp + 0xc], edx
00476062  jne        0x475dc3

// block 00476068
00476068  pop        edi
00476069  pop        esi
0047606a  pop        ebx

// block 0047606b
0047606b  mov        ecx, dword ptr [ebp - 4]
0047606e  xor        ecx, ebp
00476070  call       0x46c932

// block 00476075
00476075  leave      
00476076  ret        
