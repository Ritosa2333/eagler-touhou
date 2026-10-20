
// block 00406e60
00406e60  cmp        dword ptr [eax + 0x3c], 0
00406e64  je         0x406eb4

// block 00406e66
00406e66  mov        ecx, dword ptr [0x4b0c94]
00406e6c  mov        edx, dword ptr [0x4b0c90]
00406e72  lea        edx, [ecx + edx*2]
00406e75  cmp        edx, 5
00406e78  ja         0x406eb4

// block 00406e7a
00406e7a  jmp        dword ptr [edx*4 + 0x406eb8]

// block 00406e81
00406e81  call       0x46abe0

// block 00406e86
00406e86  xor        eax, eax
00406e88  ret        

// block 00406e89
00406e89  call       0x408510

// block 00406e8e
00406e8e  xor        eax, eax
00406e90  ret        

// block 00406e91
00406e91  push       ebx
00406e92  mov        ebx, eax
00406e94  call       0x407580

// block 00406e99
00406e99  pop        ebx
00406e9a  xor        eax, eax
00406e9c  ret        

// block 00406e9d
00406e9d  mov        ecx, eax
00406e9f  call       0x407a30

// block 00406ea4
00406ea4  xor        eax, eax
00406ea6  ret        

// block 00406ea7
00406ea7  call       0x408c30

// block 00406eac
00406eac  xor        eax, eax
00406eae  ret        

// block 00406eaf
00406eaf  call       0x409220

// block 00406eb4
00406eb4  xor        eax, eax
00406eb6  ret        
