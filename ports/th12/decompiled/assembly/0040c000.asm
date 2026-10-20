
// block 0040c000
0040c000  mov        edx, dword ptr [edi + 0x3fc]
0040c006  fld        dword ptr [edx + 0x34]
0040c009  sub        esp, 8
0040c00c  fstp       dword ptr [esp + 4]
0040c010  lea        ecx, [edi + 0x4bc]
0040c016  fld        dword ptr [edx + 0x38]
0040c019  fstp       dword ptr [esp]
0040c01c  call       0x409970

// block 0040c021
0040c021  test       eax, eax
0040c023  je         0x40c09e

// block 0040c025
0040c025  fld        dword ptr [0x4a3d98]
0040c02b  push       esi
0040c02c  fcomp      dword ptr [ecx]
0040c02e  fnstsw     ax
0040c030  test       ah, 0x41
0040c033  jne        0x40c042

// block 0040c035
0040c035  fld        dword ptr [edx + 0x38]
0040c038  fadd       qword ptr [0x4a3d80]
0040c03e  fadd       dword ptr [ecx]
0040c040  jmp        0x40c05e

// block 0040c042
0040c042  fld        dword ptr [0x4a3d88]
0040c048  fcomp      dword ptr [ecx]
0040c04a  fnstsw     ax
0040c04c  test       ah, 5
0040c04f  jp         0x40c084

// block 0040c051
0040c051  fld        dword ptr [ecx]
0040c053  fld        dword ptr [edx + 0x38]
0040c056  fadd       qword ptr [0x4a3d80]
0040c05c  fsubp      st(1)

// block 0040c05e
0040c05e  fstp       dword ptr [ecx]
0040c060  push       ecx
0040c061  fld        dword ptr [0x4a3da8]
0040c067  lea        esi, [edi + 0x838]
0040c06d  fstp       dword ptr [esp]
0040c070  call       0x464a20

// block 0040c075
0040c075  mov        edx, dword ptr [edi + 0x544]
0040c07b  test       edx, edx
0040c07d  jl         0x40c084

// block 0040c07f
0040c07f  call       0x453d90

// block 0040c084
0040c084  cmp        dword ptr [edi + 0x83c], 0
0040c08b  pop        esi
0040c08c  jg         0x40c09e

// block 0040c08e
0040c08e  xor        dword ptr [edi + 0x528], 0x20000
0040c098  mov        eax, 1
0040c09d  ret        

// block 0040c09e
0040c09e  xor        eax, eax
0040c0a0  ret        
