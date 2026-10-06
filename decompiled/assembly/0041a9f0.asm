
// block 0041a9f0
0041a9f0  push       ecx
0041a9f1  fld        dword ptr [esp + 8]
0041a9f5  mov        eax, dword ptr [0x4b43dc]
0041a9fa  mov        eax, dword ptr [eax + 0x68]
0041a9fd  fmul       st(0), st(0)
0041a9ff  push       esi
0041aa00  xor        esi, esi
0041aa02  fstp       dword ptr [esp + 0xc]
0041aa06  test       eax, eax
0041aa08  je         0x41aa64

// block 0041aa0a
0041aa0a  lea        ebx, [ebx]

// block 0041aa10
0041aa10  mov        ecx, dword ptr [eax]
0041aa12  mov        edx, dword ptr [eax + 4]
0041aa15  mov        eax, dword ptr [ecx + 0x26f8]
0041aa1b  test       al, 0x21
0041aa1d  jne        0x41aa5e

// block 0041aa1f
0041aa1f  test       eax, 0x6000000
0041aa24  jne        0x41aa5e

// block 0041aa26
0041aa26  fld        dword ptr [edi + 4]
0041aa29  fsub       dword ptr [ecx + 0x1078]
0041aa2f  fld        dword ptr [edi]
0041aa31  fsub       dword ptr [ecx + 0x1074]
0041aa37  fmul       st(0), st(0)
0041aa39  fld        st(1)
0041aa3b  fmulp      st(2)
0041aa3d  faddp      st(1)
0041aa3f  fstp       dword ptr [esp + 4]
0041aa43  fld        dword ptr [esp + 4]
0041aa47  fld        dword ptr [esp + 0xc]
0041aa4b  fcomp      st(1)
0041aa4d  fnstsw     ax
0041aa4f  test       ah, 0x41
0041aa52  jne        0x41aa5c

// block 0041aa54
0041aa54  fstp       dword ptr [esp + 0xc]
0041aa58  mov        esi, ecx
0041aa5a  jmp        0x41aa5e

// block 0041aa5c
0041aa5c  fstp       st(0)

// block 0041aa5e
0041aa5e  mov        eax, edx
0041aa60  test       edx, edx
0041aa62  jne        0x41aa10

// block 0041aa64
0041aa64  mov        eax, esi
0041aa66  pop        esi
0041aa67  pop        ecx
0041aa68  ret        4
