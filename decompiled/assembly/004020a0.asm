
// block 004020a0
004020a0  push       ebp
004020a1  mov        ebp, esp
004020a3  and        esp, 0xfffffff8
004020a6  sub        esp, 0x240
004020ac  mov        eax, dword ptr [0x4ad138]
004020b1  xor        eax, esp
004020b3  mov        dword ptr [esp + 0x23c], eax
004020ba  mov        ecx, dword ptr [ebp + 8]
004020bd  lea        eax, [ebp + 0xc]
004020c0  push       eax
004020c1  push       ecx
004020c2  lea        edx, [esp + 0x140]
004020c9  push       edx
004020ca  call       0x46cad8

// block 004020cf
004020cf  add        esp, 0xc
004020d2  xor        eax, eax

// block 004020d4
004020d4  mov        cl, byte ptr [esp + eax + 0x138]
004020db  mov        byte ptr [esp + eax], cl
004020de  inc        eax
004020df  test       cl, cl
004020e1  jne        0x4020d4

// block 004020e3
004020e3  mov        edx, dword ptr [edi + 8]
004020e6  fld        dword ptr [esi + 0x18f84]
004020ec  mov        eax, dword ptr [edi]
004020ee  fstp       dword ptr [esp + 0x110]
004020f5  mov        ecx, dword ptr [edi + 4]
004020f8  fld        dword ptr [esi + 0x18f88]
004020fe  mov        dword ptr [esp + 0x108], edx
00402105  fstp       dword ptr [esp + 0x114]
0040210c  mov        edx, dword ptr [esi + 0x18f98]
00402112  mov        dword ptr [esp + 0x100], eax
00402119  mov        eax, dword ptr [esi + 0x18f80]
0040211f  mov        dword ptr [esp + 0x104], ecx
00402126  mov        ecx, dword ptr [esi + 0x18f8c]
0040212c  mov        dword ptr [esp + 0x120], edx
00402133  mov        edx, dword ptr [esi + 0x18fa0]
00402139  mov        dword ptr [esp + 0x10c], eax
00402140  mov        eax, dword ptr [esi + 0x18f94]
00402146  mov        dword ptr [esp + 0x11c], ecx
0040214d  mov        ecx, dword ptr [esi + 0x18f9c]
00402153  mov        dword ptr [esp + 0x12c], edx
0040215a  lea        edx, [esp]
0040215d  mov        dword ptr [esp + 0x124], eax
00402164  mov        eax, dword ptr [esi + 0x18fa4]
0040216a  mov        dword ptr [esp + 0x128], ecx
00402171  mov        ecx, dword ptr [esi + 0x18fa8]
00402177  push       edx
00402178  push       esi
00402179  mov        dword ptr [esp + 0x138], eax
00402180  mov        dword ptr [esp + 0x13c], ecx
00402187  call       0x4018f0

// block 0040218c
0040218c  mov        ecx, dword ptr [esp + 0x23c]
00402193  xor        ecx, esp
00402195  call       0x46c932

// block 0040219a
0040219a  mov        esp, ebp
0040219c  pop        ebp
0040219d  ret        
