
// block 00462420
00462420  mov        eax, dword ptr [esi + 0xc]
00462423  push       ebp
00462424  xor        ebp, ebp
00462426  push       edi
00462427  mov        edi, dword ptr [0x4ce89c]
0046242d  test       eax, eax
0046242f  je         0x46243f

// block 00462431
00462431  mov        ecx, dword ptr [esi + 0x20]
00462434  call       eax

// block 00462436
00462436  mov        ebp, eax
00462438  mov        dword ptr [esi + 0xc], 0

// block 0046243f
0046243f  test       dword ptr [0x4cee78], 0x8000
00462449  je         0x46245c

// block 0046244b
0046244b  push       0x4cf0f8
00462450  call       dword ptr [0x498088]

// block 00462456
00462456  inc        byte ptr [0x4cf218]

// block 0046245c
0046245c  lea        eax, [edi + 0x38]
0046245f  mov        dword ptr [esi], ebx
00462461  cmp        dword ptr [eax + 4], 0
00462465  je         0x462478

// block 00462467
00462467  mov        ecx, dword ptr [eax + 4]
0046246a  mov        edx, dword ptr [ecx]
0046246c  cmp        dword ptr [edx], ebx
0046246e  jge        0x462478

// block 00462470
00462470  mov        eax, ecx
00462472  cmp        dword ptr [eax + 4], 0
00462476  jne        0x462467

// block 00462478
00462478  mov        edx, dword ptr [eax + 4]
0046247b  lea        ecx, [esi + 0x14]
0046247e  test       edx, edx
00462480  je         0x46248b

// block 00462482
00462482  mov        dword ptr [ecx + 4], edx
00462485  mov        edx, dword ptr [eax + 4]
00462488  mov        dword ptr [edx + 8], ecx

// block 0046248b
0046248b  mov        dword ptr [eax + 4], ecx
0046248e  mov        dword ptr [ecx + 8], eax
00462491  test       dword ptr [0x4cee78], 0x8000
0046249b  je         0x4624ae

// block 0046249d
0046249d  push       0x4cf0f8
004624a2  call       dword ptr [0x49808c]

// block 004624a8
004624a8  dec        byte ptr [0x4cf218]

// block 004624ae
004624ae  pop        edi
004624af  mov        eax, ebp
004624b1  pop        ebp
004624b2  ret        
