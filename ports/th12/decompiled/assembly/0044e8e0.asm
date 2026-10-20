
// block 0044e8e0
0044e8e0  sub        esp, 0x1c
0044e8e3  push       ebx
0044e8e4  push       ebp
0044e8e5  push       esi
0044e8e6  push       edi
0044e8e7  test       eax, eax
0044e8e9  jne        0x44e8f3

// block 0044e8eb
0044e8eb  mov        edi, dword ptr [0x4ce554]
0044e8f1  jmp        0x44e911

// block 0044e8f3
0044e8f3  cmp        eax, 1
0044e8f6  jne        0x44e900

// block 0044e8f8
0044e8f8  mov        edi, dword ptr [0x4ce550]
0044e8fe  jmp        0x44e911

// block 0044e900
0044e900  mov        edi, dword ptr [0x4cc54c]
0044e906  cmp        eax, 2
0044e909  je         0x44e911

// block 0044e90b
0044e90b  mov        edi, dword ptr [0x4b453c]

// block 0044e911
0044e911  mov        eax, 0x11
0044e916  cmp        dword ptr [esp + 0x38], eax
0044e91a  jge        0x44e920

// block 0044e91c
0044e91c  mov        dword ptr [esp + 0x38], eax

// block 0044e920
0044e920  mov        ecx, dword ptr [esp + 0x3c]
0044e924  mov        eax, ecx
0044e926  shr        eax, 0x14
0044e929  mov        edx, ecx
0044e92b  and        eax, 0xf
0044e92e  shr        edx, 0xc
0044e931  shl        eax, 4
0044e934  and        edx, 0xf
0044e937  or         eax, edx
0044e939  mov        edx, dword ptr [0x4b0e68]
0044e93f  shr        ecx, 4
0044e942  and        ecx, 0xf
0044e945  shl        eax, 4
0044e948  or         eax, ecx
0044e94a  xor        ecx, ecx
0044e94c  cmp        dword ptr [0x4b0e54], ecx
0044e952  jle        0x44e965

// block 0044e954
0044e954  mov        word ptr [edx], ax
0044e957  add        ecx, 2
0044e95a  add        edx, 2
0044e95d  cmp        ecx, dword ptr [0x4b0e54]
0044e963  jl         0x44e954

// block 0044e965
0044e965  mov        esi, dword ptr [0x4b0e5c]
0044e96b  mov        ebp, dword ptr [0x49802c]
0044e971  push       edi
0044e972  push       esi
0044e973  call       ebp

// block 0044e975
0044e975  mov        edi, dword ptr [esp + 0x38]
0044e979  lea        edi, [edi + edi + 6]
0044e97d  push       edi
0044e97e  mov        ebx, eax
0044e980  call       0x44dc20

// block 0044e985
0044e985  push       1
0044e987  push       esi
0044e988  call       dword ptr [0x49801c]

// block 0044e98e
0044e98e  mov        eax, dword ptr [esp + 0x40]
0044e992  lea        ecx, [eax + 1]

// block 0044e995
0044e995  mov        dl, byte ptr [eax]
0044e997  inc        eax
0044e998  test       dl, dl
0044e99a  jne        0x44e995

// block 0044e99c
0044e99c  sub        eax, ecx
0044e99e  mov        dword ptr [esp + 0x14], eax
0044e9a2  mov        eax, dword ptr [esp + 0x3c]
0044e9a6  push       eax
0044e9a7  push       esi
0044e9a8  call       dword ptr [0x498018]

// block 0044e9ae
0044e9ae  mov        ecx, dword ptr [esp + 0x14]
0044e9b2  mov        edx, dword ptr [esp + 0x40]
0044e9b6  mov        eax, dword ptr [esp + 0x34]
0044e9ba  push       ecx
0044e9bb  push       edx
0044e9bc  push       0
0044e9be  lea        ecx, [eax + eax]
0044e9c1  push       ecx
0044e9c2  push       esi
0044e9c3  call       dword ptr [0x498014]

// block 0044e9c9
0044e9c9  push       ebx
0044e9ca  push       esi
0044e9cb  call       ebp

// block 0044e9cd
0044e9cd  push       edi
0044e9ce  call       0x44dc20

// block 0044e9d3
0044e9d3  push       edi
0044e9d4  call       0x44d8b0

// block 0044e9d9
0044e9d9  push       ebx
0044e9da  push       esi
0044e9db  call       ebp

// block 0044e9dd
0044e9dd  mov        esi, dword ptr [esp + 0x30]
0044e9e1  mov        eax, dword ptr [esi + 8]
0044e9e4  sub        eax, dword ptr [esi]
0044e9e6  mov        edx, dword ptr [esp + 0x38]
0044e9ea  xor        edi, edi
0044e9ec  lea        eax, [eax + eax + 0x16]
0044e9f0  cmp        eax, 0x400
0044e9f5  lea        ecx, [edx + edx + 2]
0044e9f9  mov        dword ptr [esp + 0x18], edi
0044e9fd  mov        dword ptr [esp + 0x1c], edi
0044ea01  mov        dword ptr [esp + 0x20], eax
0044ea05  mov        dword ptr [esp + 0x24], ecx
0044ea09  jle        0x44ea13

// block 0044ea0b
0044ea0b  mov        dword ptr [esp + 0x20], 0x400

// block 0044ea13
0044ea13  mov        eax, dword ptr [esp + 0x44]
0044ea17  mov        edx, dword ptr [eax]
0044ea19  mov        edx, dword ptr [edx + 0x48]
0044ea1c  lea        ecx, [esp + 0x38]
0044ea20  push       ecx
0044ea21  push       edi
0044ea22  push       eax
0044ea23  call       edx

// block 0044ea25
0044ea25  mov        ecx, dword ptr [0x4b0e58]
0044ea2b  mov        edx, dword ptr [0x4b0e48]
0044ea31  push       edi
0044ea32  push       4
0044ea34  lea        eax, [esp + 0x20]
0044ea38  push       eax
0044ea39  mov        eax, dword ptr [0x4b0e68]
0044ea3e  push       edi
0044ea3f  push       ecx
0044ea40  mov        ecx, dword ptr [esp + 0x4c]
0044ea44  push       edx
0044ea45  push       eax
0044ea46  push       esi
0044ea47  push       edi
0044ea48  push       ecx
0044ea49  call       0x46c6ec

// block 0044ea4e
0044ea4e  mov        eax, dword ptr [esp + 0x38]
0044ea52  cmp        eax, edi
0044ea54  je         0x44ea5e

// block 0044ea56
0044ea56  mov        edx, dword ptr [eax]
0044ea58  push       eax
0044ea59  mov        eax, dword ptr [edx + 8]
0044ea5c  call       eax

// block 0044ea5e
0044ea5e  pop        edi
0044ea5f  pop        esi
0044ea60  pop        ebp
0044ea61  pop        ebx
0044ea62  add        esp, 0x1c
0044ea65  ret        0x18
