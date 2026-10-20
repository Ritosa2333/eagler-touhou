
// block 004838bd
004838bd  mov        edi, edi
004838bf  push       ebp
004838c0  mov        ebp, esp
004838c2  sub        esp, 0x10
004838c5  movzx      ecx, word ptr [eax + 0x42]
004838c9  movzx      edx, word ptr [eax + 0x44]
004838cd  mov        dword ptr [ebp - 4], ecx
004838d0  mov        dword ptr [ebp - 8], edx
004838d3  test       esi, esi
004838d5  jne        0x4838dc

// block 004838d7
004838d7  or         eax, 0xffffffff
004838da  leave      
004838db  ret        

// block 004838dc
004838dc  and        dword ptr [ebp - 0xc], 0
004838e0  push       ebx
004838e1  push       edi
004838e2  mov        dword ptr [ebp - 0x10], eax
004838e5  lea        eax, [esi + 4]
004838e8  push       eax
004838e9  push       0x31
004838eb  push       ecx
004838ec  xor        ebx, ebx
004838ee  inc        ebx
004838ef  lea        eax, [ebp - 0x10]
004838f2  push       ebx
004838f3  push       eax
004838f4  call       0x47a12b

// block 004838f9
004838f9  mov        edi, eax
004838fb  lea        eax, [esi + 8]
004838fe  push       eax
004838ff  push       0x32
00483901  push       dword ptr [ebp - 4]
00483904  lea        eax, [ebp - 0x10]
00483907  push       ebx
00483908  push       eax
00483909  call       0x47a12b

// block 0048390e
0048390e  or         edi, eax
00483910  lea        eax, [esi + 0xc]
00483913  push       eax
00483914  push       0x33
00483916  push       dword ptr [ebp - 4]
00483919  lea        eax, [ebp - 0x10]
0048391c  push       ebx
0048391d  push       eax
0048391e  call       0x47a12b

// block 00483923
00483923  or         edi, eax
00483925  lea        eax, [esi + 0x10]
00483928  push       eax
00483929  push       0x34
0048392b  push       dword ptr [ebp - 4]
0048392e  lea        eax, [ebp - 0x10]
00483931  push       ebx
00483932  push       eax
00483933  call       0x47a12b

// block 00483938
00483938  add        esp, 0x50
0048393b  or         edi, eax
0048393d  lea        eax, [esi + 0x14]
00483940  push       eax
00483941  push       0x35
00483943  push       dword ptr [ebp - 4]
00483946  lea        eax, [ebp - 0x10]
00483949  push       ebx
0048394a  push       eax
0048394b  call       0x47a12b

// block 00483950
00483950  or         edi, eax
00483952  lea        eax, [esi + 0x18]
00483955  push       eax
00483956  push       0x36
00483958  push       dword ptr [ebp - 4]
0048395b  lea        eax, [ebp - 0x10]
0048395e  push       ebx
0048395f  push       eax
00483960  call       0x47a12b

// block 00483965
00483965  push       esi
00483966  push       0x37
00483968  push       dword ptr [ebp - 4]
0048396b  or         edi, eax
0048396d  lea        eax, [ebp - 0x10]
00483970  push       ebx
00483971  push       eax
00483972  call       0x47a12b

// block 00483977
00483977  or         edi, eax
00483979  lea        eax, [esi + 0x20]
0048397c  push       eax
0048397d  push       0x2a
0048397f  push       dword ptr [ebp - 4]
00483982  lea        eax, [ebp - 0x10]
00483985  push       ebx
00483986  push       eax
00483987  call       0x47a12b

// block 0048398c
0048398c  add        esp, 0x50
0048398f  or         edi, eax
00483991  lea        eax, [esi + 0x24]
00483994  push       eax
00483995  push       0x2b
00483997  push       dword ptr [ebp - 4]
0048399a  lea        eax, [ebp - 0x10]
0048399d  push       ebx
0048399e  push       eax
0048399f  call       0x47a12b

// block 004839a4
004839a4  or         edi, eax
004839a6  lea        eax, [esi + 0x28]
004839a9  push       eax
004839aa  push       0x2c
004839ac  push       dword ptr [ebp - 4]
004839af  lea        eax, [ebp - 0x10]
004839b2  push       ebx
004839b3  push       eax
004839b4  call       0x47a12b

// block 004839b9
004839b9  or         edi, eax
004839bb  lea        eax, [esi + 0x2c]
004839be  push       eax
004839bf  push       0x2d
004839c1  push       dword ptr [ebp - 4]
004839c4  lea        eax, [ebp - 0x10]
004839c7  push       ebx
004839c8  push       eax
004839c9  call       0x47a12b

// block 004839ce
004839ce  or         edi, eax
004839d0  lea        eax, [esi + 0x30]
004839d3  push       eax
004839d4  push       0x2e
004839d6  push       dword ptr [ebp - 4]
004839d9  lea        eax, [ebp - 0x10]
004839dc  push       ebx
004839dd  push       eax
004839de  call       0x47a12b

// block 004839e3
004839e3  add        esp, 0x50
004839e6  or         edi, eax
004839e8  lea        eax, [esi + 0x34]
004839eb  push       eax
004839ec  push       0x2f
004839ee  push       dword ptr [ebp - 4]
004839f1  lea        eax, [ebp - 0x10]
004839f4  push       ebx
004839f5  push       eax
004839f6  call       0x47a12b

// block 004839fb
004839fb  or         edi, eax
004839fd  lea        eax, [esi + 0x1c]
00483a00  push       eax
00483a01  push       0x30
00483a03  push       dword ptr [ebp - 4]
00483a06  lea        eax, [ebp - 0x10]
00483a09  push       ebx
00483a0a  push       eax
00483a0b  call       0x47a12b

// block 00483a10
00483a10  or         edi, eax
00483a12  lea        eax, [esi + 0x38]
00483a15  push       eax
00483a16  push       0x44
00483a18  push       dword ptr [ebp - 4]
00483a1b  lea        eax, [ebp - 0x10]
00483a1e  push       ebx
00483a1f  push       eax
00483a20  call       0x47a12b

// block 00483a25
00483a25  or         edi, eax
00483a27  lea        eax, [esi + 0x3c]
00483a2a  push       eax
00483a2b  push       0x45
00483a2d  push       dword ptr [ebp - 4]
00483a30  lea        eax, [ebp - 0x10]
00483a33  push       ebx
00483a34  push       eax
00483a35  call       0x47a12b

// block 00483a3a
00483a3a  add        esp, 0x50
00483a3d  or         edi, eax
00483a3f  lea        eax, [esi + 0x40]
00483a42  push       eax
00483a43  push       0x46
00483a45  push       dword ptr [ebp - 4]
00483a48  lea        eax, [ebp - 0x10]
00483a4b  push       ebx
00483a4c  push       eax
00483a4d  call       0x47a12b

// block 00483a52
00483a52  or         edi, eax
00483a54  lea        eax, [esi + 0x44]
00483a57  push       eax
00483a58  push       0x47
00483a5a  push       dword ptr [ebp - 4]
00483a5d  lea        eax, [ebp - 0x10]
00483a60  push       ebx
00483a61  push       eax
00483a62  call       0x47a12b

// block 00483a67
00483a67  or         edi, eax
00483a69  lea        eax, [esi + 0x48]
00483a6c  push       eax
00483a6d  push       0x48
00483a6f  push       dword ptr [ebp - 4]
00483a72  lea        eax, [ebp - 0x10]
00483a75  push       ebx
00483a76  push       eax
00483a77  call       0x47a12b

// block 00483a7c
00483a7c  or         edi, eax
00483a7e  lea        eax, [esi + 0x4c]
00483a81  push       eax
00483a82  push       0x49
00483a84  push       dword ptr [ebp - 4]
00483a87  lea        eax, [ebp - 0x10]
00483a8a  push       ebx
00483a8b  push       eax
00483a8c  call       0x47a12b

// block 00483a91
00483a91  add        esp, 0x50
00483a94  or         edi, eax
00483a96  lea        eax, [esi + 0x50]
00483a99  push       eax
00483a9a  push       0x4a
00483a9c  push       dword ptr [ebp - 4]
00483a9f  lea        eax, [ebp - 0x10]
00483aa2  push       ebx
00483aa3  push       eax
00483aa4  call       0x47a12b

// block 00483aa9
00483aa9  or         edi, eax
00483aab  lea        eax, [esi + 0x54]
00483aae  push       eax
00483aaf  push       0x4b
00483ab1  push       dword ptr [ebp - 4]
00483ab4  lea        eax, [ebp - 0x10]
00483ab7  push       ebx
00483ab8  push       eax
00483ab9  call       0x47a12b

// block 00483abe
00483abe  or         edi, eax
00483ac0  lea        eax, [esi + 0x58]
00483ac3  push       eax
00483ac4  push       0x4c
00483ac6  push       dword ptr [ebp - 4]
00483ac9  lea        eax, [ebp - 0x10]
00483acc  push       ebx
00483acd  push       eax
00483ace  call       0x47a12b

// block 00483ad3
00483ad3  or         edi, eax
00483ad5  lea        eax, [esi + 0x5c]
00483ad8  push       eax
00483ad9  push       0x4d
00483adb  push       dword ptr [ebp - 4]
00483ade  lea        eax, [ebp - 0x10]
00483ae1  push       ebx
00483ae2  push       eax
00483ae3  call       0x47a12b

// block 00483ae8
00483ae8  add        esp, 0x50
00483aeb  or         edi, eax
00483aed  lea        eax, [esi + 0x60]
00483af0  push       eax
00483af1  push       0x4e
00483af3  push       dword ptr [ebp - 4]
00483af6  lea        eax, [ebp - 0x10]
00483af9  push       ebx
00483afa  push       eax
00483afb  call       0x47a12b

// block 00483b00
00483b00  or         edi, eax
00483b02  lea        eax, [esi + 0x64]
00483b05  push       eax
00483b06  push       0x4f
00483b08  push       dword ptr [ebp - 4]
00483b0b  lea        eax, [ebp - 0x10]
00483b0e  push       ebx
00483b0f  push       eax
00483b10  call       0x47a12b

// block 00483b15
00483b15  or         edi, eax
00483b17  lea        eax, [esi + 0x68]
00483b1a  push       eax
00483b1b  push       0x38
00483b1d  push       dword ptr [ebp - 4]
00483b20  lea        eax, [ebp - 0x10]
00483b23  push       ebx
00483b24  push       eax
00483b25  call       0x47a12b

// block 00483b2a
00483b2a  or         edi, eax
00483b2c  lea        eax, [esi + 0x6c]
00483b2f  push       eax
00483b30  push       0x39
00483b32  push       dword ptr [ebp - 4]
00483b35  lea        eax, [ebp - 0x10]
00483b38  push       ebx
00483b39  push       eax
00483b3a  call       0x47a12b

// block 00483b3f
00483b3f  add        esp, 0x50
00483b42  or         edi, eax
00483b44  lea        eax, [esi + 0x70]
00483b47  push       eax
00483b48  push       0x3a
00483b4a  push       dword ptr [ebp - 4]
00483b4d  lea        eax, [ebp - 0x10]
00483b50  push       ebx
00483b51  push       eax
00483b52  call       0x47a12b

// block 00483b57
00483b57  or         edi, eax
00483b59  lea        eax, [esi + 0x74]
00483b5c  push       eax
00483b5d  push       0x3b
00483b5f  push       dword ptr [ebp - 4]
00483b62  lea        eax, [ebp - 0x10]
00483b65  push       ebx
00483b66  push       eax
00483b67  call       0x47a12b

// block 00483b6c
00483b6c  or         edi, eax
00483b6e  lea        eax, [esi + 0x78]
00483b71  push       eax
00483b72  push       0x3c
00483b74  push       dword ptr [ebp - 4]
00483b77  lea        eax, [ebp - 0x10]
00483b7a  push       ebx
00483b7b  push       eax
00483b7c  call       0x47a12b

// block 00483b81
00483b81  or         edi, eax
00483b83  lea        eax, [esi + 0x7c]
00483b86  push       eax
00483b87  push       0x3d
00483b89  push       dword ptr [ebp - 4]
00483b8c  lea        eax, [ebp - 0x10]
00483b8f  push       ebx
00483b90  push       eax
00483b91  call       0x47a12b

// block 00483b96
00483b96  add        esp, 0x50
00483b99  or         edi, eax
00483b9b  lea        eax, [esi + 0x80]
00483ba1  push       eax
00483ba2  push       0x3e
00483ba4  push       dword ptr [ebp - 4]
00483ba7  lea        eax, [ebp - 0x10]
00483baa  push       ebx
00483bab  push       eax
00483bac  call       0x47a12b

// block 00483bb1
00483bb1  or         edi, eax
00483bb3  lea        eax, [esi + 0x84]
00483bb9  push       eax
00483bba  push       0x3f
00483bbc  push       dword ptr [ebp - 4]
00483bbf  lea        eax, [ebp - 0x10]
00483bc2  push       ebx
00483bc3  push       eax
00483bc4  call       0x47a12b

// block 00483bc9
00483bc9  or         edi, eax
00483bcb  lea        eax, [esi + 0x88]
00483bd1  push       eax
00483bd2  push       0x40
00483bd4  push       dword ptr [ebp - 4]
00483bd7  push       ebx
00483bd8  lea        eax, [ebp - 0x10]
00483bdb  push       eax
00483bdc  call       0x47a12b

// block 00483be1
00483be1  or         edi, eax
00483be3  lea        eax, [esi + 0x8c]
00483be9  push       eax
00483bea  push       0x41
00483bec  push       dword ptr [ebp - 4]
00483bef  lea        eax, [ebp - 0x10]
00483bf2  push       ebx
00483bf3  push       eax
00483bf4  call       0x47a12b

// block 00483bf9
00483bf9  add        esp, 0x50
00483bfc  or         edi, eax
00483bfe  lea        eax, [esi + 0x90]
00483c04  push       eax
00483c05  push       0x42
00483c07  push       dword ptr [ebp - 4]
00483c0a  lea        eax, [ebp - 0x10]
00483c0d  push       ebx
00483c0e  push       eax
00483c0f  call       0x47a12b

// block 00483c14
00483c14  or         edi, eax
00483c16  lea        eax, [esi + 0x94]
00483c1c  push       eax
00483c1d  push       0x43
00483c1f  push       dword ptr [ebp - 4]
00483c22  lea        eax, [ebp - 0x10]
00483c25  push       ebx
00483c26  push       eax
00483c27  call       0x47a12b

// block 00483c2c
00483c2c  or         edi, eax
00483c2e  lea        eax, [esi + 0x98]
00483c34  push       eax
00483c35  push       0x28
00483c37  push       dword ptr [ebp - 4]
00483c3a  lea        eax, [ebp - 0x10]
00483c3d  push       ebx
00483c3e  push       eax
00483c3f  call       0x47a12b

// block 00483c44
00483c44  or         edi, eax
00483c46  lea        eax, [esi + 0x9c]
00483c4c  push       eax
00483c4d  push       0x29
00483c4f  push       dword ptr [ebp - 4]
00483c52  lea        eax, [ebp - 0x10]
00483c55  push       ebx
00483c56  push       eax
00483c57  call       0x47a12b

// block 00483c5c
00483c5c  add        esp, 0x50
00483c5f  or         edi, eax
00483c61  lea        eax, [esi + 0xa0]
00483c67  push       eax
00483c68  push       0x1f
00483c6a  push       dword ptr [ebp - 8]
00483c6d  lea        eax, [ebp - 0x10]
00483c70  push       ebx
00483c71  push       eax
00483c72  call       0x47a12b

// block 00483c77
00483c77  or         edi, eax
00483c79  lea        eax, [esi + 0xa4]
00483c7f  push       eax
00483c80  push       0x20
00483c82  push       dword ptr [ebp - 8]
00483c85  lea        eax, [ebp - 0x10]
00483c88  push       ebx
00483c89  push       eax
00483c8a  call       0x47a12b

// block 00483c8f
00483c8f  or         edi, eax
00483c91  lea        eax, [esi + 0xa8]
00483c97  push       eax
00483c98  push       0x1003
00483c9d  push       dword ptr [ebp - 8]
00483ca0  lea        eax, [ebp - 0x10]
00483ca3  push       ebx
00483ca4  push       eax
00483ca5  call       0x47a12b

// block 00483caa
00483caa  or         edi, eax
00483cac  lea        eax, [esi + 0xb0]
00483cb2  push       eax
00483cb3  push       0x1009
00483cb8  mov        ebx, dword ptr [ebp - 8]
00483cbb  push       ebx
00483cbc  lea        eax, [ebp - 0x10]
00483cbf  push       0
00483cc1  push       eax
00483cc2  call       0x47a12b

// block 00483cc7
00483cc7  add        esp, 0x50
00483cca  or         edi, eax
00483ccc  mov        eax, edi
00483cce  pop        edi
00483ccf  mov        dword ptr [esi + 0xac], ebx
00483cd5  pop        ebx
00483cd6  leave      
00483cd7  ret        
