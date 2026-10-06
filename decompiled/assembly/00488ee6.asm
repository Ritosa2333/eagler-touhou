
// block 00488ee6
00488ee6  mov        edi, edi
00488ee8  push       ebp
00488ee9  mov        ebp, esp
00488eeb  push       ecx
00488eec  push       ecx
00488eed  and        dword ptr [ebp - 4], 0
00488ef1  push       ebx
00488ef2  push       esi
00488ef3  push       edi
00488ef4  mov        edi, dword ptr [ebp + 0xc]
00488ef7  dec        edi
00488ef8  lea        ecx, [edi + 1]
00488efb  mov        eax, ecx
00488efd  cdq        
00488efe  and        edx, 0x1f
00488f01  add        eax, edx
00488f03  mov        ebx, eax
00488f05  mov        eax, ecx
00488f07  sar        ebx, 5
00488f0a  and        eax, 0x8000001f
00488f0f  jns        0x488f16

// block 00488f11
00488f11  dec        eax
00488f12  or         eax, 0xffffffe0
00488f15  inc        eax

// block 00488f16
00488f16  mov        esi, dword ptr [ebp + 8]
00488f19  push       0x1f
00488f1b  pop        ecx
00488f1c  sub        ecx, eax
00488f1e  xor        eax, eax
00488f20  inc        eax
00488f21  shl        eax, cl
00488f23  mov        dword ptr [ebp - 8], ecx
00488f26  test       dword ptr [esi + ebx*4], eax
00488f29  je         0x488fb0

// block 00488f2f
00488f2f  or         edx, 0xffffffff
00488f32  shl        edx, cl
00488f34  mov        eax, ebx
00488f36  not        edx
00488f38  test       dword ptr [esi + eax*4], edx
00488f3b  jmp        0x488f41

// block 00488f3d
00488f3d  cmp        dword ptr [esi + eax*4], 0

// block 00488f41
00488f41  jne        0x488f4b

// block 00488f43
00488f43  inc        eax
00488f44  cmp        eax, 3
00488f47  jl         0x488f3d

// block 00488f49
00488f49  jmp        0x488fb0

// block 00488f4b
00488f4b  mov        eax, edi
00488f4d  cdq        
00488f4e  push       0x1f
00488f50  pop        ecx
00488f51  and        edx, ecx
00488f53  add        eax, edx
00488f55  sar        eax, 5
00488f58  and        edi, 0x8000001f
00488f5e  jns        0x488f65

// block 00488f60
00488f60  dec        edi
00488f61  or         edi, 0xffffffe0
00488f64  inc        edi

// block 00488f65
00488f65  and        dword ptr [ebp + 0xc], 0
00488f69  xor        edx, edx
00488f6b  sub        ecx, edi
00488f6d  inc        edx
00488f6e  shl        edx, cl
00488f70  mov        ecx, dword ptr [esi + eax*4]
00488f73  lea        edi, [ecx + edx]
00488f76  cmp        edi, ecx
00488f78  jb         0x488f7e

// block 00488f7a
00488f7a  cmp        edi, edx
00488f7c  jae        0x488f85

// block 00488f7e
00488f7e  mov        dword ptr [ebp + 0xc], 1

// block 00488f85
00488f85  mov        ecx, dword ptr [ebp + 0xc]
00488f88  mov        dword ptr [esi + eax*4], edi
00488f8b  jmp        0x488faa

// block 00488f8d
00488f8d  test       ecx, ecx
00488f8f  je         0x488fad

// block 00488f91
00488f91  mov        ecx, dword ptr [esi + eax*4]
00488f94  lea        edx, [ecx + 1]
00488f97  xor        edi, edi
00488f99  cmp        edx, ecx
00488f9b  jb         0x488fa2

// block 00488f9d
00488f9d  cmp        edx, 1
00488fa0  jae        0x488fa5

// block 00488fa2
00488fa2  xor        edi, edi
00488fa4  inc        edi

// block 00488fa5
00488fa5  mov        dword ptr [esi + eax*4], edx
00488fa8  mov        ecx, edi

// block 00488faa
00488faa  dec        eax
00488fab  jns        0x488f8d

// block 00488fad
00488fad  mov        dword ptr [ebp - 4], ecx

// block 00488fb0
00488fb0  mov        ecx, dword ptr [ebp - 8]
00488fb3  or         eax, 0xffffffff
00488fb6  shl        eax, cl
00488fb8  push       3
00488fba  pop        ecx
00488fbb  and        dword ptr [esi + ebx*4], eax
00488fbe  inc        ebx
00488fbf  cmp        ebx, ecx
00488fc1  jge        0x488fce

// block 00488fc3
00488fc3  lea        esi, [esi + ebx*4]
00488fc6  sub        ecx, ebx
00488fc8  xor        eax, eax
00488fca  mov        edi, esi

// block 00488fcc
00488fcc  rep stosd  dword ptr es:[edi], eax

// block 00488fce
00488fce  mov        eax, dword ptr [ebp - 4]
00488fd1  pop        edi
00488fd2  pop        esi
00488fd3  pop        ebx
00488fd4  leave      
00488fd5  ret        
