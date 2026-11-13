# TH14 界面 Oracle

使用日文 1.00b 的原版 `th14.exe`、启用 SDL3 的 Emscripten SDK、Node.js 和安装了 Unicorn 的 Python。先用当前源码构建 GLES Runtime，再用同一构建的游戏对象运行对照。

```sh
python -m pip install unicorn
python tools/th14-build-gles.py --emsdk /path/to/emsdk --out artifacts/th14-ui
python tools/th14-ui-oracle.py --exe /path/to/th14.exe --emsdk /path/to/emsdk --build artifacts/th14-ui --report artifacts/th14-ui/oracle.json
```

Oracle 执行原版暂停截图矩形、曲线激光顶点、位图四边形和续关次数初始化，并读取原版菜单音效调用与 WAV 映射。位图对照为两边提供相同的各分辨率素材尺寸，验证位置缩放不会再次放大位图。实际字库的选择由 [ANM VM Oracle](TH14_ANM_VM_ORACLE.md) 验证。曲线激光在两边使用同一中间画面坐标原点。候选侧还验证旧配置中的各个显示模式在网页里都使用 1280×960。

对照使用受控的菜单状态、几何和截图接口。浏览器像素、纹理采样和实际 PCM 需要在游戏内验证。出现差异时命令返回非零退出码，JSON 同时保留原版预期和候选输出。
