# **这是被vibe coding魔改的uth05win**

**This project is vibe coded**

您需要自行提供有效的游戏文件。

本仓库不分发任何游戏的素材及运行库。

如您遇到本Fork的问题，请勿向Pegasy留言或者进入原水木社区留言。

You should provide your own legal copy of the game.


The following text was taken from the original author Pegasy

### 介绍 Introduction

============

这是“东方怪绮谈：Mystic Square”的非官方Windows移植版本。（Touhou 05,下称th05）
此移植版几乎已经完全移植了原版的功能和特性，但是仍然有许多区别。
请看“对比”以了解详细信息。
This is an unofficial port of the game Touhou Kaikidan: Mystic Square (the fifth game of the touhou series, referred to as th05 below) to windows. It faithfully reproduces almost all features of the original PC98 version. Check "Comparison" section for the details

### 使用方法 Usage

=====

1.克隆/下载源代码/thprac Branch的源代码。
2.使用msvc等编译器编译二进制文件。
3.去SourceForge下载[glew](https://glew.sourceforge.net/)注意版本必须早于2014年。
4.将glew32.dll放入存有二进制文件的文件夹中。
5.去下载[PMDWin](http://c60.fmp.jp/)，注意版本必须早于2014年。并将PMDWin.dll放入同一个文件夹中。
6.将您拥有的合法的游戏资产文件拷贝到该文件夹中，并将“怪綺談1.dat”重命名为"kaiki1.dat"，“怪綺談2.dat”重命名为"kaiki2.dat"
7.运行编译后的二进制文件。
1. Extract all files into a folder.
2. Download glew from http://glew.sourceforge.net/ (download the binaries) and extract glew32.dll into the same folder.
3. Download PMDWin from http://c60.fmp.jp/ and extract PMDWin.dll into the same folder.
4. (Optional) Put anex86.bmp into the same foldeer. (google it to find out how to get this file)
5. Put 怪綺談1.dat, 怪綺談2.dat and ZUN.com from the orignal PC98 version into the same folder (these files may have different names depending on you system's code page, for example, 夦鉟択1.dat in chinese code page)
  5.1 If you only have an .hdi file that contains the original game, you can:
    5.1.1 Download anex86 (google it to find the download link)
    5.1.2 Use anxdiet that is included in anex86 to open the .hdi file
    5.1.3 Drag the three need files to the destination folder
6. Rename 怪綺談1.dat to kaiki1.dat, 怪綺談2.dat to kaiki2.dat.
7. Run uth05win.exe.

To start the game without translation, open config.ini and comment the line with "MOD_FILE = chs.dat"

### 对比 Comparison

==========

在该移植版本，添加的功能：
 * Replay 支持
 * 中文翻译（可关闭）
 * 修复PC-98版的各类bug
 * 将使用`int`的部分代码转换为`float`增强精度，同时避免游戏崩溃

在该Fork中，新添加的功能：
 * Practice 单关练习支持
 * Music Room
 * 原版通关 Staff Roll、Extra All Cast 和角色实力评价
 * thprac 支持（位于`thprac`分支内）
 * 部分视觉优化和QoS

New features in this ported version:
 * replay support
 * translated into chinese (can be turned off)
 * fixed a number of bugs in the PC98 version
 * uses float instead of int where necessary, increasing accuracy

目前仍需验证／实现的功能：
 * 原始 Demo 已接入输入和专用原版运算路径，仍需 PC-98 原版逐帧轨迹对照；原版允许掉残与放 Bomb，不能以无伤或播放到末尾作为还原判据。
 * 部分选项

The current fork restores Staff Roll, All Cast and player performance evaluation using the original game data.
See [PRACTICE.md](PRACTICE.md) for controls, verification and Demo compatibility details.

Features still requiring verification or implementation:
 * original-frame validation of Demo battle simulation (original inputs and source-based compatibility are implemented; native misses and bombs are allowed)
 * some options

### 操作 Controls

========

和大部分东方游戏类似：
  Z      射击
  X      Bomb
  Shift  低速模式
  Ctrl   跳过对话
  ESC    暂停菜单

thprac的特殊键：
  Tab    显示Tracker
  F1-F7  作弊功能
  Bksp   作弊菜单
  F12    高级作弊选项

Similar to other games of touhou series:
  Z      Shoot
  X      Bomb
  Shift  Slow move
  Ctrl   skip dialog
  ESC    Menu

要进入全屏，请按Alt+Enter。

To toggle fullscreen mode, press alt + enter.

目前不支持控制器。

Game pad is not supported.

### 已知问题 Known bugs

==========

When you switch to/from full screen mode, the font texture may become corrupted.

### 版权 Copyright

=========

1999 上海爱丽丝幻乐团/ZunTeam
2011 Pegasy
1999 Team Shanghai Alice
2011 Pegasy

所有所使用的资产和库版权均属对应作者所有。

The assets and libraries it uses belongs to their authors. Check their webpage for copyright information

此项目使用的资产：
* “东方怪绮谈”游戏内容（未随仓库分发）：(http://www.kt.rim.or.jp/~aotaka/am/index.htm)
* OpenGL 扩展渲染器库（glew）：(https://glew.sourceforge.net/)
* PMDWin (http://c60.fmp.jp/)
* Rec98 (rec98.nmlgc.net)

This program uses:
* assets from th05 (http://www.kt.rim.or.jp/~aotaka/am/index.htm)
* now need to prepare your own assets
* The OpenGL Extension Wrangler Library (http://glew.sourceforge.net/)
* PMDWin (http://c60.fmp.jp/)
* Rec98 (rec98.nmlgc.net)
