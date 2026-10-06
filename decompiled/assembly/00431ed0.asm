
// block 00431ed0
00431ed0  push       ebx
00431ed1  push       ebp
00431ed2  mov        ebp, dword ptr [0x498088]
00431ed8  push       esi
00431ed9  mov        ebx, eax
00431edb  mov        esi, dword ptr [ebx + 8]
00431ede  push       edi
00431edf  mov        edi, dword ptr [0x4ce89c]
00431ee5  test       esi, esi
00431ee7  je         0x431f28

// block 00431ee9
00431ee9  test       dword ptr [0x4cee78], 0x8000
00431ef3  je         0x431f02

// block 00431ef5
00431ef5  push       0x4cf0f8
00431efa  call       ebp

// block 00431efc
00431efc  inc        byte ptr [0x4cf218]

// block 00431f02
00431f02  mov        ecx, esi
00431f04  mov        edx, edi
00431f06  call       0x462890

// block 00431f0b
00431f0b  test       dword ptr [0x4cee78], 0x8000
00431f15  je         0x431f28

// block 00431f17
00431f17  push       0x4cf0f8
00431f1c  call       dword ptr [0x49808c]

// block 00431f22
00431f22  dec        byte ptr [0x4cf218]

// block 00431f28
00431f28  mov        esi, dword ptr [ebx + 0xc]
00431f2b  mov        edi, dword ptr [0x4ce89c]
00431f31  test       esi, esi
00431f33  je         0x431f74

// block 00431f35
00431f35  test       dword ptr [0x4cee78], 0x8000
00431f3f  je         0x431f4e

// block 00431f41
00431f41  push       0x4cf0f8
00431f46  call       ebp

// block 00431f48
00431f48  inc        byte ptr [0x4cf218]

// block 00431f4e
00431f4e  mov        ecx, esi
00431f50  mov        edx, edi
00431f52  call       0x462890

// block 00431f57
00431f57  test       dword ptr [0x4cee78], 0x8000
00431f61  je         0x431f74

// block 00431f63
00431f63  push       0x4cf0f8
00431f68  call       dword ptr [0x49808c]

// block 00431f6e
00431f6e  dec        byte ptr [0x4cf218]

// block 00431f74
00431f74  lea        edi, [ebx + 0x204]
00431f7a  mov        ebx, 0x19
00431f7f  nop        

// block 00431f80
00431f80  mov        esi, dword ptr [edi]
00431f82  test       esi, esi
00431f84  je         0x431f95

// block 00431f86
00431f86  push       esi
00431f87  call       0x43b450

// block 00431f8c
00431f8c  push       esi
00431f8d  call       0x46ca4f

// block 00431f92
00431f92  add        esp, 4

// block 00431f95
00431f95  add        edi, 4
00431f98  sub        ebx, 1
00431f9b  jne        0x431f80

// block 00431f9d
00431f9d  pop        edi
00431f9e  pop        esi
00431f9f  pop        ebp
00431fa0  mov        dword ptr [0x4b4510], ebx
00431fa6  pop        ebx
00431fa7  ret        
