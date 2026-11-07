# TH15 移动端 GPU 诊断

开发夹具加载原版三面或六面，使用双倍分辨率、持续射击和测试用无敌。四种配置分别选择原有光栅绘制、等尺寸 GPU 拷贝、拷贝后提交命令，以及每批重新应用绘制状态。选择配置后开局，进入 Chapter Finish 或出现残影时添加标记，下载含构建身份、画布属性和 GPU 计数的 JSON。

在 monorepo 根目录构建开发 Wasm，再运行本机诊断服务。

```powershell
node ports/th15/web/scripts/build-sdl-application.mjs --thprac
$env:TH15_CAPTURE_INPUT = '<导入目录，含 games/th15/th15.dat、shared/msgothic.ttc 和 shared/unifont.otf>'
$env:TH15_CAPTURE_FONTS = '<包含 cp932.bin 的目录>'
node ports/th15/portable/mobile-gpu/serve.mjs
```

默认访问 `http://127.0.0.1:8165/`。手机实测时可在可信局域网中显式设置 `TH15_GPU_HOST=0.0.0.0`，结束后关闭服务。数据和字体由操作者在本机提供。

`node --test ports/th15/web/tests/sdl/gpu-diagnostics.test.mjs` 校验配置和记录边界。设置同样的资源环境变量后，`application-mobile-capture-browser.test.mjs` 在 Chromium 中检查三面、六面的当前帧标记、弹幕负载与 WebGL 状态。移动设备上的画面由操作者现场观察。
