
// block 00450810
00450810  sub        esp, 0x110
00450816  mov        eax, dword ptr [0x4ad138]
0045081b  xor        eax, esp
0045081d  mov        dword ptr [esp + 0x108], eax
00450824  push       esi
00450825  call       0x44f4b0

// block 0045082a
0045082a  test       dword ptr [0x4d48c4], 0x40000
00450834  je         0x450896

// block 00450836
00450836  push       0x4a2644
0045083b  call       0x46d585

// block 00450840
00450840  add        esp, 4
00450843  xor        esi, esi

// block 00450845
00450845  push       esi
00450846  lea        eax, [esp + 0xc]
0045084a  push       0x4a2650
0045084f  push       eax
00450850  call       0x46cbbb

// block 00450855
00450855  add        esp, 0xc
00450858  lea        ecx, [esp + 8]
0045085c  push       ecx
0045085d  call       0x463df0

// block 00450862
00450862  test       eax, eax
00450864  je         0x450885

// block 00450866
00450866  inc        esi
00450867  cmp        esi, 0x3e8
0045086d  jl         0x450845

// block 0045086f
0045086f  pop        esi
00450870  mov        ecx, dword ptr [esp + 0x108]
00450877  xor        ecx, esp
00450879  call       0x46c932

// block 0045087e
0045087e  add        esp, 0x110
00450884  ret        

// block 00450885
00450885  cmp        esi, 0x3e8
0045088b  jge        0x450896

// block 0045088d
0045088d  lea        eax, [esp + 8]
00450891  call       0x42fca0

// block 00450896
00450896  mov        ecx, dword ptr [esp + 0x10c]
0045089d  pop        esi
0045089e  xor        ecx, esp
004508a0  call       0x46c932

// block 004508a5
004508a5  add        esp, 0x110
004508ab  ret        
