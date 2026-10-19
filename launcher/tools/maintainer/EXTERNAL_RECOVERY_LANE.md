# 恢复 External 站点

既有 Hosted 站点仍提供正确的资源时，可以基于它的 Package generation 重新生成 External 前端站。资源内容与描述符沿用原 generation，Relay 和来源迁移配置使用本次明确给出的值。

## 1. 准备

准备完整的 Hosted Release、与其 DATA 布局匹配的 Runtime Release，以及新的输出目录。命令从 `launcher/` 运行：

```sh
node tools/maintainer/recover-external-site.mjs   --source=/releases/hosted-site   --runtime-release=/releases/runtime-release   --output=/validation/external-site   --profile=web-validation-external-recovery   --netplay-relay=wss://play.example.com/eagler-netplay/   --origin-migration=none
```

## 2. 检查结果

工具校验输入 manifest、校验和及 Runtime 身份。输出的 `release-manifest.json` 记录原 Hosted release ID、Runtime manifest 哈希、所选游戏和本次覆盖配置。

检查生成站点的游戏、语言、音乐与资源指针，再上传到已确认的测试或恢复地址。运行 `npm run verify:deployed -- <地址>`，并在浏览器中安装和启动游戏。保留上一个可用部署版本以便切回。

本流程生成 `web-validation-*` 产物。准备新正式版本时使用[Release 流程](../../docs/RELEASE.md)。
