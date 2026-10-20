
// block 00452f00
00452f00  push       ebx
00452f01  push       ebp
00452f02  mov        ebp, dword ptr [0x498088]
00452f08  push       esi
00452f09  mov        ebx, eax
00452f0b  mov        esi, dword ptr [ebx + 8]
00452f0e  push       edi
00452f0f  mov        edi, dword ptr [0x4ce89c]
00452f15  test       esi, esi
00452f17  je         0x452f58

// block 00452f19
00452f19  test       dword ptr [0x4cee78], 0x8000
00452f23  je         0x452f32

// block 00452f25
00452f25  push       0x4cf0f8
00452f2a  call       ebp

// block 00452f2c
00452f2c  inc        byte ptr [0x4cf218]

// block 00452f32
00452f32  mov        ecx, esi
00452f34  mov        edx, edi
00452f36  call       0x462890

// block 00452f3b
00452f3b  test       dword ptr [0x4cee78], 0x8000
00452f45  je         0x452f58

// block 00452f47
00452f47  push       0x4cf0f8
00452f4c  call       dword ptr [0x49808c]

// block 00452f52
00452f52  dec        byte ptr [0x4cf218]

// block 00452f58
00452f58  mov        esi, dword ptr [ebx + 0xc]
00452f5b  mov        edi, dword ptr [0x4ce89c]
00452f61  test       esi, esi
00452f63  je         0x452fa1

// block 00452f65
00452f65  mov        ebx, 0x8000
00452f6a  test       dword ptr [0x4cee78], ebx
00452f70  je         0x452f7f

// block 00452f72
00452f72  push       0x4cf0f8
00452f77  call       ebp

// block 00452f79
00452f79  inc        byte ptr [0x4cf218]

// block 00452f7f
00452f7f  mov        ecx, esi
00452f81  mov        edx, edi
00452f83  call       0x462890

// block 00452f88
00452f88  test       dword ptr [0x4cee78], ebx
00452f8e  je         0x452fa1

// block 00452f90
00452f90  push       0x4cf0f8
00452f95  call       dword ptr [0x49808c]

// block 00452f9b
00452f9b  dec        byte ptr [0x4cf218]

// block 00452fa1
00452fa1  pop        edi
00452fa2  pop        esi
00452fa3  pop        ebp
00452fa4  pop        ebx
00452fa5  ret        
