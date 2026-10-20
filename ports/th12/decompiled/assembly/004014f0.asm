
// block 004014f0
004014f0  mov        edx, dword ptr [ecx + 0x18f7c]
004014f6  cmp        edx, 0x140
004014fc  push       esi
004014fd  mov        esi, eax
004014ff  jge        0x4015bd

// block 00401505
00401505  mov        eax, edx
00401507  imul       eax, eax, 0x138
0040150d  push       edi
0040150e  lea        eax, [eax + ecx + 0x97c]
00401515  inc        edx
00401516  mov        edi, eax
00401518  mov        dword ptr [ecx + 0x18f7c], edx
0040151e  sub        edi, esi

// block 00401520
00401520  mov        dl, byte ptr [esi]
00401522  mov        byte ptr [edi + esi], dl
00401525  inc        esi
00401526  test       dl, dl
00401528  jne        0x401520

// block 0040152a
0040152a  mov        edx, dword ptr [ebx]
0040152c  mov        dword ptr [eax + 0x100], edx
00401532  mov        edx, dword ptr [ebx + 4]
00401535  mov        dword ptr [eax + 0x104], edx
0040153b  mov        edx, dword ptr [ebx + 8]
0040153e  mov        dword ptr [eax + 0x108], edx
00401544  mov        edx, dword ptr [ecx + 0x18f80]
0040154a  mov        dword ptr [eax + 0x10c], edx
00401550  fld        dword ptr [ecx + 0x18f84]
00401556  fstp       dword ptr [eax + 0x110]
0040155c  pop        edi
0040155d  fld        dword ptr [ecx + 0x18f88]
00401563  fstp       dword ptr [eax + 0x114]
00401569  mov        edx, dword ptr [ecx + 0x18f8c]
0040156f  mov        dword ptr [eax + 0x11c], edx
00401575  mov        edx, dword ptr [ecx + 0x18f98]
0040157b  mov        dword ptr [eax + 0x120], edx
00401581  mov        edx, dword ptr [ecx + 0x18f94]
00401587  mov        dword ptr [eax + 0x124], edx
0040158d  mov        edx, dword ptr [ecx + 0x18f9c]
00401593  mov        dword ptr [eax + 0x128], edx
00401599  mov        edx, dword ptr [ecx + 0x18fa0]
0040159f  mov        dword ptr [eax + 0x12c], edx
004015a5  mov        edx, dword ptr [ecx + 0x18fa4]
004015ab  mov        dword ptr [eax + 0x130], edx
004015b1  mov        ecx, dword ptr [ecx + 0x18fa8]
004015b7  mov        dword ptr [eax + 0x134], ecx

// block 004015bd
004015bd  pop        esi
004015be  ret        
