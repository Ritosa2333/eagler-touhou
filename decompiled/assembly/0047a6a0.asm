
// block 0047a6a0
0047a6a0  push       ebp
0047a6a1  mov        ebp, esp
0047a6a3  push       edi
0047a6a4  push       esi
0047a6a5  mov        esi, dword ptr [ebp + 0xc]
0047a6a8  mov        ecx, dword ptr [ebp + 0x10]
0047a6ab  mov        edi, dword ptr [ebp + 8]
0047a6ae  mov        eax, ecx
0047a6b0  mov        edx, ecx
0047a6b2  add        eax, esi
0047a6b4  cmp        edi, esi
0047a6b6  jbe        0x47a6c0

// block 0047a6b8
0047a6b8  cmp        edi, eax
0047a6ba  jb         0x47a864

// block 0047a6c0
0047a6c0  cmp        ecx, 0x100
0047a6c6  jb         0x47a6e7

// block 0047a6c8
0047a6c8  cmp        dword ptr [0x4d52dc], 0
0047a6cf  je         0x47a6e7

// block 0047a6d1
0047a6d1  push       edi
0047a6d2  push       esi
0047a6d3  and        edi, 0xf
0047a6d6  and        esi, 0xf
0047a6d9  cmp        edi, esi
0047a6db  pop        esi
0047a6dc  pop        edi
0047a6dd  jne        0x47a6e7

// block 0047a6df
0047a6df  pop        esi
0047a6e0  pop        edi
0047a6e1  pop        ebp
0047a6e2  jmp        0x48c4e8

// block 0047a6e7
0047a6e7  test       edi, 3
0047a6ed  jne        0x47a704

// block 0047a6ef
0047a6ef  shr        ecx, 2
0047a6f2  and        edx, 3
0047a6f5  cmp        ecx, 8
0047a6f8  jb         0x47a724

// block 0047a6fa
0047a6fa  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a6fc
0047a6fc  jmp        dword ptr [edx*4 + 0x47a814]

// block 0047a704
0047a704  mov        eax, edi
0047a706  mov        edx, 3
0047a70b  sub        ecx, 4
0047a70e  jb         0x47a71c

// block 0047a710
0047a710  and        eax, 3
0047a713  add        ecx, eax
0047a715  jmp        dword ptr [eax*4 + 0x47a728]

// block 0047a71c
0047a71c  jmp        dword ptr [ecx*4 + 0x47a824]

// block 0047a724
0047a724  jmp        dword ptr [ecx*4 + 0x47a7a8]

// block 0047a738
0047a738  and        edx, ecx
0047a73a  mov        al, byte ptr [esi]
0047a73c  mov        byte ptr [edi], al
0047a73e  mov        al, byte ptr [esi + 1]
0047a741  mov        byte ptr [edi + 1], al
0047a744  mov        al, byte ptr [esi + 2]
0047a747  shr        ecx, 2
0047a74a  mov        byte ptr [edi + 2], al
0047a74d  add        esi, 3
0047a750  add        edi, 3
0047a753  cmp        ecx, 8
0047a756  jb         0x47a724

// block 0047a758
0047a758  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a75a
0047a75a  jmp        dword ptr [edx*4 + 0x47a814]

// block 0047a764
0047a764  and        edx, ecx
0047a766  mov        al, byte ptr [esi]
0047a768  mov        byte ptr [edi], al
0047a76a  mov        al, byte ptr [esi + 1]
0047a76d  shr        ecx, 2
0047a770  mov        byte ptr [edi + 1], al
0047a773  add        esi, 2
0047a776  add        edi, 2
0047a779  cmp        ecx, 8
0047a77c  jb         0x47a724

// block 0047a77e
0047a77e  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a780
0047a780  jmp        dword ptr [edx*4 + 0x47a814]

// block 0047a788
0047a788  and        edx, ecx
0047a78a  mov        al, byte ptr [esi]
0047a78c  mov        byte ptr [edi], al
0047a78e  add        esi, 1
0047a791  shr        ecx, 2
0047a794  add        edi, 1
0047a797  cmp        ecx, 8
0047a79a  jb         0x47a724

// block 0047a79c
0047a79c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a79e
0047a79e  jmp        dword ptr [edx*4 + 0x47a814]

// block 0047a7c8
0047a7c8  mov        eax, dword ptr [esi + ecx*4 - 0x1c]
0047a7cc  mov        dword ptr [edi + ecx*4 - 0x1c], eax

// block 0047a7d0
0047a7d0  mov        eax, dword ptr [esi + ecx*4 - 0x18]
0047a7d4  mov        dword ptr [edi + ecx*4 - 0x18], eax

// block 0047a7d8
0047a7d8  mov        eax, dword ptr [esi + ecx*4 - 0x14]
0047a7dc  mov        dword ptr [edi + ecx*4 - 0x14], eax

// block 0047a7e0
0047a7e0  mov        eax, dword ptr [esi + ecx*4 - 0x10]
0047a7e4  mov        dword ptr [edi + ecx*4 - 0x10], eax

// block 0047a7e8
0047a7e8  mov        eax, dword ptr [esi + ecx*4 - 0xc]
0047a7ec  mov        dword ptr [edi + ecx*4 - 0xc], eax

// block 0047a7f0
0047a7f0  mov        eax, dword ptr [esi + ecx*4 - 8]
0047a7f4  mov        dword ptr [edi + ecx*4 - 8], eax

// block 0047a7f8
0047a7f8  mov        eax, dword ptr [esi + ecx*4 - 4]
0047a7fc  mov        dword ptr [edi + ecx*4 - 4], eax
0047a800  lea        eax, [ecx*4]
0047a807  add        esi, eax
0047a809  add        edi, eax

// block 0047a80b
0047a80b  jmp        dword ptr [edx*4 + 0x47a814]

// block 0047a824
0047a824  mov        eax, dword ptr [ebp + 8]
0047a827  pop        esi
0047a828  pop        edi
0047a829  leave      
0047a82a  ret        

// block 0047a82c
0047a82c  mov        al, byte ptr [esi]
0047a82e  mov        byte ptr [edi], al
0047a830  mov        eax, dword ptr [ebp + 8]
0047a833  pop        esi
0047a834  pop        edi
0047a835  leave      
0047a836  ret        

// block 0047a838
0047a838  mov        al, byte ptr [esi]
0047a83a  mov        byte ptr [edi], al
0047a83c  mov        al, byte ptr [esi + 1]
0047a83f  mov        byte ptr [edi + 1], al
0047a842  mov        eax, dword ptr [ebp + 8]
0047a845  pop        esi
0047a846  pop        edi
0047a847  leave      
0047a848  ret        

// block 0047a84c
0047a84c  mov        al, byte ptr [esi]
0047a84e  mov        byte ptr [edi], al
0047a850  mov        al, byte ptr [esi + 1]
0047a853  mov        byte ptr [edi + 1], al
0047a856  mov        al, byte ptr [esi + 2]
0047a859  mov        byte ptr [edi + 2], al
0047a85c  mov        eax, dword ptr [ebp + 8]
0047a85f  pop        esi
0047a860  pop        edi
0047a861  leave      
0047a862  ret        

// block 0047a864
0047a864  lea        esi, [ecx + esi - 4]
0047a868  lea        edi, [ecx + edi - 4]
0047a86c  test       edi, 3
0047a872  jne        0x47a898

// block 0047a874
0047a874  shr        ecx, 2
0047a877  and        edx, 3
0047a87a  cmp        ecx, 8
0047a87d  jb         0x47a88c

// block 0047a87f
0047a87f  std        

// block 0047a880
0047a880  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a882
0047a882  cld        
0047a883  jmp        dword ptr [edx*4 + 0x47a9b0]

// block 0047a88c
0047a88c  neg        ecx
0047a88e  jmp        dword ptr [ecx*4 + 0x47a960]

// block 0047a898
0047a898  mov        eax, edi
0047a89a  mov        edx, 3
0047a89f  cmp        ecx, 4
0047a8a2  jb         0x47a8b0

// block 0047a8a4
0047a8a4  and        eax, 3
0047a8a7  sub        ecx, eax
0047a8a9  jmp        dword ptr [eax*4 + 0x47a8b4]

// block 0047a8b0
0047a8b0  jmp        dword ptr [ecx*4 + 0x47a9b0]

// block 0047a8c4
0047a8c4  mov        al, byte ptr [esi + 3]
0047a8c7  and        edx, ecx
0047a8c9  mov        byte ptr [edi + 3], al
0047a8cc  sub        esi, 1
0047a8cf  shr        ecx, 2
0047a8d2  sub        edi, 1
0047a8d5  cmp        ecx, 8
0047a8d8  jb         0x47a88c

// block 0047a8da
0047a8da  std        

// block 0047a8db
0047a8db  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a8dd
0047a8dd  cld        
0047a8de  jmp        dword ptr [edx*4 + 0x47a9b0]

// block 0047a8e8
0047a8e8  mov        al, byte ptr [esi + 3]
0047a8eb  and        edx, ecx
0047a8ed  mov        byte ptr [edi + 3], al
0047a8f0  mov        al, byte ptr [esi + 2]
0047a8f3  shr        ecx, 2
0047a8f6  mov        byte ptr [edi + 2], al
0047a8f9  sub        esi, 2
0047a8fc  sub        edi, 2
0047a8ff  cmp        ecx, 8
0047a902  jb         0x47a88c

// block 0047a904
0047a904  std        

// block 0047a905
0047a905  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a907
0047a907  cld        
0047a908  jmp        dword ptr [edx*4 + 0x47a9b0]

// block 0047a910
0047a910  mov        al, byte ptr [esi + 3]
0047a913  and        edx, ecx
0047a915  mov        byte ptr [edi + 3], al
0047a918  mov        al, byte ptr [esi + 2]
0047a91b  mov        byte ptr [edi + 2], al
0047a91e  mov        al, byte ptr [esi + 1]
0047a921  shr        ecx, 2
0047a924  mov        byte ptr [edi + 1], al
0047a927  sub        esi, 3
0047a92a  sub        edi, 3
0047a92d  cmp        ecx, 8
0047a930  jb         0x47a88c

// block 0047a936
0047a936  std        

// block 0047a937
0047a937  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0047a939
0047a939  cld        
0047a93a  jmp        dword ptr [edx*4 + 0x47a9b0]

// block 0047a964
0047a964  mov        eax, dword ptr [esi + ecx*4 + 0x1c]
0047a968  mov        dword ptr [edi + ecx*4 + 0x1c], eax

// block 0047a96c
0047a96c  mov        eax, dword ptr [esi + ecx*4 + 0x18]
0047a970  mov        dword ptr [edi + ecx*4 + 0x18], eax

// block 0047a974
0047a974  mov        eax, dword ptr [esi + ecx*4 + 0x14]
0047a978  mov        dword ptr [edi + ecx*4 + 0x14], eax

// block 0047a97c
0047a97c  mov        eax, dword ptr [esi + ecx*4 + 0x10]
0047a980  mov        dword ptr [edi + ecx*4 + 0x10], eax

// block 0047a984
0047a984  mov        eax, dword ptr [esi + ecx*4 + 0xc]
0047a988  mov        dword ptr [edi + ecx*4 + 0xc], eax

// block 0047a98c
0047a98c  mov        eax, dword ptr [esi + ecx*4 + 8]
0047a990  mov        dword ptr [edi + ecx*4 + 8], eax

// block 0047a994
0047a994  mov        eax, dword ptr [esi + ecx*4 + 4]
0047a998  mov        dword ptr [edi + ecx*4 + 4], eax
0047a99c  lea        eax, [ecx*4]
0047a9a3  add        esi, eax
0047a9a5  add        edi, eax

// block 0047a9a7
0047a9a7  jmp        dword ptr [edx*4 + 0x47a9b0]

// block 0047a9c0
0047a9c0  mov        eax, dword ptr [ebp + 8]
0047a9c3  pop        esi
0047a9c4  pop        edi
0047a9c5  leave      
0047a9c6  ret        

// block 0047a9c8
0047a9c8  mov        al, byte ptr [esi + 3]
0047a9cb  mov        byte ptr [edi + 3], al
0047a9ce  mov        eax, dword ptr [ebp + 8]
0047a9d1  pop        esi
0047a9d2  pop        edi
0047a9d3  leave      
0047a9d4  ret        

// block 0047a9d8
0047a9d8  mov        al, byte ptr [esi + 3]
0047a9db  mov        byte ptr [edi + 3], al
0047a9de  mov        al, byte ptr [esi + 2]
0047a9e1  mov        byte ptr [edi + 2], al
0047a9e4  mov        eax, dword ptr [ebp + 8]
0047a9e7  pop        esi
0047a9e8  pop        edi
0047a9e9  leave      
0047a9ea  ret        

// block 0047a9ec
0047a9ec  mov        al, byte ptr [esi + 3]
0047a9ef  mov        byte ptr [edi + 3], al
0047a9f2  mov        al, byte ptr [esi + 2]
0047a9f5  mov        byte ptr [edi + 2], al
0047a9f8  mov        al, byte ptr [esi + 1]
0047a9fb  mov        byte ptr [edi + 1], al
0047a9fe  mov        eax, dword ptr [ebp + 8]
0047aa01  pop        esi
0047aa02  pop        edi
0047aa03  leave      
0047aa04  ret        
