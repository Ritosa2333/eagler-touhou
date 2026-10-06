
// block 00410ea0
00410ea0  push       ebp
00410ea1  mov        ebp, dword ptr [0x498088]
00410ea7  push       esi
00410ea8  mov        esi, dword ptr [ebx + 8]
00410eab  push       edi
00410eac  mov        edi, dword ptr [0x4ce89c]
00410eb2  test       esi, esi
00410eb4  je         0x410ef5

// block 00410eb6
00410eb6  test       dword ptr [0x4cee78], 0x8000
00410ec0  je         0x410ecf

// block 00410ec2
00410ec2  push       0x4cf0f8
00410ec7  call       ebp

// block 00410ec9
00410ec9  inc        byte ptr [0x4cf218]

// block 00410ecf
00410ecf  mov        ecx, esi
00410ed1  mov        edx, edi
00410ed3  call       0x462890

// block 00410ed8
00410ed8  test       dword ptr [0x4cee78], 0x8000
00410ee2  je         0x410ef5

// block 00410ee4
00410ee4  push       0x4cf0f8
00410ee9  call       dword ptr [0x49808c]

// block 00410eef
00410eef  dec        byte ptr [0x4cf218]

// block 00410ef5
00410ef5  mov        esi, dword ptr [ebx + 0xc]
00410ef8  mov        edi, dword ptr [0x4ce89c]
00410efe  test       esi, esi
00410f00  je         0x410f41

// block 00410f02
00410f02  test       dword ptr [0x4cee78], 0x8000
00410f0c  je         0x410f1b

// block 00410f0e
00410f0e  push       0x4cf0f8
00410f13  call       ebp

// block 00410f15
00410f15  inc        byte ptr [0x4cf218]

// block 00410f1b
00410f1b  mov        ecx, esi
00410f1d  mov        edx, edi
00410f1f  call       0x462890

// block 00410f24
00410f24  test       dword ptr [0x4cee78], 0x8000
00410f2e  je         0x410f41

// block 00410f30
00410f30  push       0x4cf0f8
00410f35  call       dword ptr [0x49808c]

// block 00410f3b
00410f3b  dec        byte ptr [0x4cf218]

// block 00410f41
00410f41  mov        esi, dword ptr [ebx + 0x18]
00410f44  test       esi, esi
00410f46  je         0x410f58

// block 00410f48
00410f48  mov        eax, esi
00410f4a  call       0x410a50

// block 00410f4f
00410f4f  push       esi
00410f50  call       0x46ca4f

// block 00410f55
00410f55  add        esp, 4

// block 00410f58
00410f58  mov        esi, dword ptr [0x4ce8cc]
00410f5e  add        esi, 0x4b5130
00410f64  mov        dword ptr [ebx + 0x18], 0
00410f6b  mov        edi, dword ptr [esi]
00410f6d  test       edi, edi
00410f6f  je         0x410f87

// block 00410f71
00410f71  call       0x4604e0

// block 00410f76
00410f76  mov        eax, dword ptr [esi]
00410f78  push       eax
00410f79  call       0x46ca4f

// block 00410f7e
00410f7e  add        esp, 4
00410f81  mov        dword ptr [esi], 0

// block 00410f87
00410f87  mov        esi, dword ptr [0x4ce8cc]
00410f8d  mov        edi, dword ptr [esi + 0x4b5134]
00410f93  add        esi, 0x4b5134
00410f99  test       edi, edi
00410f9b  je         0x410fb3

// block 00410f9d
00410f9d  call       0x4604e0

// block 00410fa2
00410fa2  mov        ecx, dword ptr [esi]
00410fa4  push       ecx
00410fa5  call       0x46ca4f

// block 00410faa
00410faa  add        esp, 4
00410fad  mov        dword ptr [esi], 0

// block 00410fb3
00410fb3  mov        esi, dword ptr [0x4ce8cc]
00410fb9  mov        edi, dword ptr [esi + 0x4b5138]
00410fbf  add        esi, 0x4b5138
00410fc5  test       edi, edi
00410fc7  je         0x410fdf

// block 00410fc9
00410fc9  call       0x4604e0

// block 00410fce
00410fce  mov        edx, dword ptr [esi]
00410fd0  push       edx
00410fd1  call       0x46ca4f

// block 00410fd6
00410fd6  add        esp, 4
00410fd9  mov        dword ptr [esi], 0

// block 00410fdf
00410fdf  mov        esi, dword ptr [0x4ce8cc]
00410fe5  mov        edi, dword ptr [esi + 0x4b513c]
00410feb  add        esi, 0x4b513c
00410ff1  test       edi, edi
00410ff3  je         0x41100b

// block 00410ff5
00410ff5  call       0x4604e0

// block 00410ffa
00410ffa  mov        eax, dword ptr [esi]
00410ffc  push       eax
00410ffd  call       0x46ca4f

// block 00411002
00411002  add        esp, 4
00411005  mov        dword ptr [esi], 0

// block 0041100b
0041100b  mov        eax, dword ptr [ebx + 0x14]
0041100e  pop        edi
0041100f  pop        esi
00411010  pop        ebp
00411011  test       eax, eax
00411013  je         0x411025

// block 00411015
00411015  push       eax
00411016  call       0x46c941

// block 0041101b
0041101b  add        esp, 4
0041101e  mov        dword ptr [ebx + 0x14], 0

// block 00411025
00411025  xor        eax, eax
00411027  mov        dword ptr [ebx + 0x14], eax
0041102a  mov        dword ptr [0x4b43d8], eax
0041102f  mov        dword ptr [0x4ce55c], eax
00411034  ret        
