# 构建与测试

在完整解压后的根目录，用 cmd 执行 call 开发环境.cmd，再运行：

    node th15_web/scripts/build-sdl-application.mjs --release
    node th15_web/scripts/package-release.mjs
    node th15_web/scripts/verify-delivery.mjs

更新 th15_web/artifacts/sdl-release 并校验源码、共享架构、生产导出、所有资源、HTTP 与部署入口。

原版对照核心和冻结模块回归：

    node th15_web/scripts/cpp/build.mjs
    node th15_web/scripts/cpp/test-suite.mjs delivery-local

集成开发夹具与真实生产启动器测试：

    node th15_web/scripts/build-sdl-application.mjs
    node th15_web/scripts/build-sdl-graphics.mjs
    set TH15_BROWSER_SITE=artifacts/sdl-release/site
    node --test th15_web/tests/sdl/production-launcher-browser.test.mjs

开发夹具含测试导出，不能正式部署。上述 TH15_BROWSER_SITE 指向当前发布目录，检查桌面和模拟手机的真实 RAF、游戏、暂停、返回标题与浏览器本地存档。完整模块／长录像对照耗时较长，范围以各脚本输出为准。

历史报告保留原版本哈希，当前发布身份以 reference/web-release.json、reference/release-acceptance.json 和 sdl-release/site/manifest.json 为准。源码绝对路径可能写入 Wasm，异地重建二进制不强求字节相同，需检查源码一致性与重建运行结果。
