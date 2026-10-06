
// block 00460040
00460040  add        byte ptr [eax], al
00460042  add        byte ptr [ecx], al
00460044  push       esp
00460045  and        al, 0xc
00460047  test       eax, eax
00460049  je         0x460082

// block 0046004b
0046004b  add        ecx, edx
0046004d  add        esi, eax
0046004f  mov        dword ptr [esp + 0x10], ecx
00460053  cmp        ecx, dword ptr [edi + 0x124]
00460059  je         0x460062

// block 0046005b
0046005b  cmp        dword ptr [esp + 0x14], 0
00460060  je         0x460000

// block 00460062
00460062  add        dword ptr [edi + 0x124], edx
00460068  mov        eax, edi
0046006a  pop        esi
0046006b  pop        ebx
0046006c  mov        esp, ebp
0046006e  pop        ebp
0046006f  ret        

// block 00460082
00460082  pop        esi
00460083  mov        dword ptr [edi + 0x124], 0
0046008d  mov        eax, edi
0046008f  pop        ebx
00460090  mov        esp, ebp
00460092  pop        ebp
00460093  ret        
