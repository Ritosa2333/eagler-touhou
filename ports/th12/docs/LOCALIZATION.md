# thcrap 本地化

TH12 与 TH10、TH11 共用启动器的 thpatch 依赖解析、CRC 校验和静态语言包发布流程。
翻译来源为 [thpatch](https://srv.thpatch.net/)，提供简体中文 `lang_zh-hans` 和英文 `lang_en`。
游戏资源以日文 1.00b 为基础；安装目录中的旧版 `th12c.dat` 与 `th12c.exe` 不参与准备。

## 准备语言包

在 monorepo 根目录执行，输入为日文零售版 `th12.dat`：

```powershell
node launcher/scripts/prepare-th06-language-pack.mjs --game th12 --language lang_zh-hans --archive D:/Games/TH12/th12.dat --thdat D:/Tools/thtk/thdat.exe --thmsg D:/Tools/thtk/thmsg.exe --output artifacts/th12-language
node launcher/scripts/prepare-th06-language-pack.mjs --game th12 --language lang_en --archive D:/Games/TH12/th12.dat --thdat D:/Tools/thtk/thdat.exe --thmsg D:/Tools/thtk/thmsg.exe --output artifacts/th12-language
```

准备过程只读取日文归档，不执行原版程序。日文 DAT 身份为
`dca82df072ba51c4b74c3d41ea1cbdcb91f709ce7cfea23599ee52caa34f2d72`。
需要 Python 与 Pillow；共享 Unicode 字体由启动器安装并在游戏初始化前等待加载完成。
可通过 `--font-python` 指定 Python，通过 `--temporary-root` 指定 thtk 临时目录。

输出的 `catalog.json` 与 `language/*.zip` 可直接用于 Host 组装。
ZIP 使用 `eagler-touhou/thcrap-static-pack/1`，内含文字覆盖表和翻译 PNG。
每个文件记录大小和 SHA-256；翻译包、资源和过程记录保存在 Git 忽略目录。

## 剧情、结局与符卡

文字框编号、自动行、结束行和删除指令遵循
[thcrap MSG 格式](https://github.com/thpatch/thcrap-tsa/blob/master/thcrap_tsa/src/th06_msg.cpp)
的 MSG_TH11 / END_TH10 定义。文字补丁记录原版资源摘要、控制锚点和每条文字时间。
Runtime 从已校验的日文 MSG 重建文字，保留等待、跳过、表情、音乐和关卡控制命令；
按 thcrap 定义移除用于原版字距定位的 opcode 25。

一个文字框可以增加翻译行和注音。普通行与注音分别使用原有文本槽；
自动播放文字保留每个原版时间点，Ctrl 跳过仍由原版 MSG 虚拟机处理。
缺译文字保留原始 CP932 字节。结局使用相同的文字锚点方式，保留日文控制指令和时间。

113 个符卡名称按 ECL 中的真实编号和基础编号解析。
稀疏翻译表采用 [thcrap 符卡查找](https://github.com/thpatch/thcrap-tsa/blob/master/thcrap_tsa/src/spells.cpp)
的难度编号向基础编号回溯规则。游戏内符卡标题按加载字体的实际字形宽度右对齐，
长名称适配文字矩形；成绩页优先显示翻译名称。

## 音乐室与字体

17 首音乐的主题名和评论按音乐室顺序匹配，不直接使用零售音乐文件的数字后缀。
例如第一面使用 `th12_00`，标题使用 `th12_01`。
音乐标题、八行评论和未解锁提醒使用已安装的 Unicode 字体，
解释 thcrap 的样式与 tab 排版指令；长行按文字矩形适配，保留整句。

ANM 继续使用日文原版程序和 sprite 布局。翻译 PNG 按原版纹理名映射到 atlas；
PNG 按 thcrap 的 sprite 边界合成：全透明的替换 sprite 保留原图，已有不透明底图使用混合，其余 sprite 使用覆盖；裁剪范围外保留原像素。

## 验证

不依赖原版资源的回归检查：

```powershell
node --test launcher/tests/react-main/th12-localization.test.mjs
python launcher/tests/test-th12-language-textures.py
```

设置 EMSDK 后，使用日文提取目录和已生成的两份语言包检查实际 MSG 虚拟机：

```powershell
node ports/th12/scripts/build-message-tests.mjs
node ports/th12/scripts/check-thcrap.mjs --retail=ports/th12/local/retail --assets=ports/th12/site/assets --packs=artifacts/th12-language
```

每种语言覆盖 42 个 MSG、108 个入口、自动剧情及 Ctrl 跳过，逐条核对实际输出文字、
原版非文字指令和完成帧数；同时检查 12 个结局、113 个符卡名称和 17 首音乐评论。
结果写入 `ports/th12/artifacts/thcrap-check/verification.json`。
浏览器验收还应检查音乐室、双行剧情、注音、长符卡名称及切回日文。
