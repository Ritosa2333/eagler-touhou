
// block 0047ee08
0047ee08  mov        edi, edi
0047ee0a  push       ebp
0047ee0b  mov        ebp, esp
0047ee0d  sub        esp, 0x10
0047ee10  mov        eax, dword ptr [0x4b4318]
0047ee15  mov        al, byte ptr [eax]
0047ee17  and        dword ptr [ebp - 8], 0
0047ee1b  and        dword ptr [ebp - 4], 0xffff0000
0047ee22  test       al, al
0047ee24  je         0x47eead

// block 0047ee2a
0047ee2a  movsx      eax, al
0047ee2d  add        eax, -0x30
0047ee30  cmp        eax, 7
0047ee33  ja         0x47eea9

// block 0047ee35
0047ee35  jmp        dword ptr [eax*4 + 0x47eebc]

// block 0047ee3c
0047ee3c  push       0x49de54
0047ee41  jmp        0x47ee56

// block 0047ee43
0047ee43  push       0x49de4c
0047ee48  jmp        0x47ee56

// block 0047ee4a
0047ee4a  push       0x49de44
0047ee4f  jmp        0x47ee56

// block 0047ee51
0047ee51  push       0x49de3c

// block 0047ee56
0047ee56  lea        ecx, [ebp - 8]
0047ee59  call       0x47e896

// block 0047ee5e
0047ee5e  mov        eax, dword ptr [0x4b4318]
0047ee63  movsx      eax, byte ptr [eax]
0047ee66  inc        dword ptr [0x4b4318]
0047ee6c  sub        eax, 0x31
0047ee6f  je         0x47ee7d

// block 0047ee71
0047ee71  dec        eax
0047ee72  dec        eax
0047ee73  je         0x47ee7d

// block 0047ee75
0047ee75  dec        eax
0047ee76  dec        eax
0047ee77  je         0x47ee7d

// block 0047ee79
0047ee79  dec        eax
0047ee7a  dec        eax
0047ee7b  jne        0x47ee99

// block 0047ee7d
0047ee7d  lea        eax, [ebp - 8]
0047ee80  push       eax
0047ee81  lea        eax, [ebp - 0x10]
0047ee84  push       0x49de30
0047ee89  push       eax
0047ee8a  call       0x47ec4a

// block 0047ee8f
0047ee8f  mov        ecx, dword ptr [eax]
0047ee91  mov        edx, dword ptr [eax + 4]
0047ee94  add        esp, 0xc
0047ee97  jmp        0x47ee9f

// block 0047ee99
0047ee99  mov        edx, dword ptr [ebp - 4]
0047ee9c  mov        ecx, dword ptr [ebp - 8]

// block 0047ee9f
0047ee9f  mov        eax, dword ptr [ebp + 8]
0047eea2  mov        dword ptr [eax], ecx
0047eea4  mov        dword ptr [eax + 4], edx
0047eea7  leave      
0047eea8  ret        

// block 0047eea9
0047eea9  push       2
0047eeab  jmp        0x47eeaf

// block 0047eead
0047eead  push       1

// block 0047eeaf
0047eeaf  mov        ecx, dword ptr [ebp + 8]
0047eeb2  call       0x47e1ce

// block 0047eeb7
0047eeb7  mov        eax, dword ptr [ebp + 8]
0047eeba  leave      
0047eebb  ret        
