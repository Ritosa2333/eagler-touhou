
// block 00435750
00435750  mov        eax, dword ptr [esp + 4]
00435754  push       ebx
00435755  push       ebp
00435756  mov        ebp, dword ptr [0x4ce8cc]
0043575c  push       esi
0043575d  push       edi
0043575e  push       eax
0043575f  mov        edx, ebp
00435761  call       0x461920

// block 00435766
00435766  mov        edi, eax
00435768  test       edi, edi
0043576a  jne        0x435770

// block 0043576c
0043576c  mov        dword ptr [esp + 0x14], eax

// block 00435770
00435770  mov        esi, dword ptr [edi + 0x3f4]
00435776  fld        dword ptr [esi + 0x34]
00435779  mov        ebx, dword ptr [esi + 4]
0043577c  call       0x4931e0

// block 00435781
00435781  fld        dword ptr [esi + 0x38]
00435784  push       eax
00435785  call       0x4931e0

// block 0043578a
0043578a  fld        dword ptr [esi + 0x10]
0043578d  push       eax
0043578e  call       0x4931e0

// block 00435793
00435793  fld        dword ptr [esi + 0xc]
00435796  push       eax
00435797  call       0x4931e0

// block 0043579c
0043579c  mov        ecx, dword ptr [edi + 0x3f8]
004357a2  mov        edx, dword ptr [ecx]
004357a4  push       eax
004357a5  push       ebx
004357a6  push       edx
004357a7  mov        edx, ebp
004357a9  call       0x4356d0

// block 004357ae
004357ae  pop        edi
004357af  pop        esi
004357b0  pop        ebp
004357b1  pop        ebx
004357b2  ret        4
