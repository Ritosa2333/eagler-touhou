
// block 00425750
00425750  mov        ecx, dword ptr [eax + 0x30]
00425753  fldz       
00425755  xor        edx, edx
00425757  test       cl, 1
0042575a  jne        0x425776

// block 0042575c
0042575c  or         ecx, 1
0042575f  fst        dword ptr [eax + 0x28]
00425762  mov        dword ptr [eax + 0x24], edx
00425765  mov        dword ptr [eax + 0x20], 0xfff0bdc1
0042576c  mov        dword ptr [eax + 0x2c], 0x4b2ed0
00425773  mov        dword ptr [eax + 0x30], ecx

// block 00425776
00425776  fstp       dword ptr [eax + 0x28]
00425779  mov        dword ptr [eax + 0x24], edx
0042577c  mov        dword ptr [eax + 0x20], 0xffffffff
00425783  ret        
