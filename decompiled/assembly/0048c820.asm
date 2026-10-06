
// block 0048c820
0048c820  mov        edi, edi
0048c822  push       ebp
0048c823  mov        ebp, esp
0048c825  sub        esp, 0x10
0048c828  push       dword ptr [ebp + 0xc]
0048c82b  lea        ecx, [ebp - 0x10]
0048c82e  call       0x46d3b4

// block 0048c833
0048c833  mov        eax, dword ptr [ebp - 0xc]
0048c836  test       eax, eax
0048c838  je         0x48c864

// block 0048c83a
0048c83a  cmp        dword ptr [eax + 4], 0x3a4
0048c841  jne        0x48c864

// block 0048c843
0048c843  push       3
0048c845  push       0
0048c847  push       dword ptr [ebp + 8]
0048c84a  push       dword ptr [ebp + 0xc]
0048c84d  call       0x48c5cb

// block 0048c852
0048c852  add        esp, 0x10
0048c855  cmp        byte ptr [ebp - 4], 0
0048c859  je         0x48c873

// block 0048c85b
0048c85b  mov        ecx, dword ptr [ebp - 8]
0048c85e  and        dword ptr [ecx + 0x70], 0xfffffffd
0048c862  leave      
0048c863  ret        

// block 0048c864
0048c864  cmp        byte ptr [ebp - 4], 0
0048c868  je         0x48c871

// block 0048c86a
0048c86a  mov        eax, dword ptr [ebp - 8]
0048c86d  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048c871
0048c871  xor        eax, eax

// block 0048c873
0048c873  leave      
0048c874  ret        
