
// block 00465f50
00465f50  push       esi
00465f51  push       edi
00465f52  mov        esi, ecx
00465f54  xor        edi, edi
00465f56  mov        dword ptr [esi], 0x4a3b1c
00465f5c  cmp        dword ptr [esi + 0x10], edi
00465f5f  jbe        0x465f87

// block 00465f61
00465f61  mov        eax, dword ptr [esi + 4]
00465f64  cmp        dword ptr [eax + edi*4], 0
00465f68  lea        eax, [eax + edi*4]
00465f6b  je         0x465f81

// block 00465f6d
00465f6d  mov        eax, dword ptr [eax]
00465f6f  mov        ecx, dword ptr [eax]
00465f71  mov        edx, dword ptr [ecx + 8]
00465f74  push       eax
00465f75  call       edx

// block 00465f77
00465f77  mov        eax, dword ptr [esi + 4]
00465f7a  mov        dword ptr [eax + edi*4], 0

// block 00465f81
00465f81  inc        edi
00465f82  cmp        edi, dword ptr [esi + 0x10]
00465f85  jb         0x465f61

// block 00465f87
00465f87  mov        eax, dword ptr [esi + 4]
00465f8a  test       eax, eax
00465f8c  je         0x465f9e

// block 00465f8e
00465f8e  push       eax
00465f8f  call       0x46ca4f

// block 00465f94
00465f94  add        esp, 4
00465f97  mov        dword ptr [esi + 4], 0

// block 00465f9e
00465f9e  mov        edi, dword ptr [esi + 0xc]
00465fa1  test       edi, edi
00465fa3  je         0x465fd2

// block 00465fa5
00465fa5  cmp        dword ptr [edi + 0x78], 1
00465fa9  jne        0x465fc2

// block 00465fab
00465fab  mov        ecx, dword ptr [edi + 0x8c]
00465fb1  push       ecx
00465fb2  call       dword ptr [0x4980a4]

// block 00465fb8
00465fb8  mov        dword ptr [edi + 0x8c], 0xffffffff

// block 00465fc2
00465fc2  push       edi
00465fc3  call       0x46ca4f

// block 00465fc8
00465fc8  add        esp, 4
00465fcb  mov        dword ptr [esi + 0xc], 0

// block 00465fd2
00465fd2  pop        edi
00465fd3  pop        esi
00465fd4  ret        
