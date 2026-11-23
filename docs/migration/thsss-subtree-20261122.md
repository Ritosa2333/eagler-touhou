# thsss 独立源码与子树维护

## 来源

- 独立源码：[sblzdddd/THSSS-WASM](https://github.com/sblzdddd/THSSS-WASM)，分支 `main`，提交 `c08d073a6c0e789773879da8f1272c7afd96fe3a`。
- monorepo 目录：`ports/thsss`，普通单人模式。本次不建立 MP 目录。
- [机器可读导入登记](thsss-eagler-20261122.json)记录来源提交、连接提交及导出基点。
- 上游说明保存在 [README-UPSTREAM.md](../../ports/thsss/README-UPSTREAM.md)。游戏资源留在外部 `Assets/`，由 `EAGLER_THSSS_CONTENT_DIR` 或 `ports/thsss/content/` 提供。
- 作品保持 `testOnly`。适配进度见 [适配表](../../ports/thsss/ADAPTATION.md)。

标准 `git subtree add --prefix=ports/thsss` 的连接提交 `630b0316e13e604fc15f52c712038766c0d78dcb` 以 THSSS-WASM `c08d073a6c0e789773879da8f1272c7afd96fe3a` 为第二父提交。连接时两侧 `ports/thsss` 树均为上游树。

## 构建

```sh
node ports/thsss/scripts/build-eagler.mjs
```

该入口在装有 .NET 10 SDK 与 `wasm-tools` 的环境执行 `dotnet publish` 与 `npm run stage:eagler`，产物为 `ports/thsss/build-eagler/`。`--print-inputs` 列出本目录构建输入，供源码身份检查使用。

```sh
EAGLER_DEVELOPMENT_GAMES=thsss EAGLER_THSSS_CONTENT_DIR=/path/to/assets npm start
```

## 合并及往返

该作品 PR 使用合并提交，保留独立来源父链。

```sh
git subtree split --prefix=ports/thsss -b export/thsss
git merge-base --is-ancestor c08d073a6c0e789773879da8f1272c7afd96fe3a export/thsss
git rev-parse HEAD:ports/thsss export/thsss^{tree}
python tools/ports/check-history.py --subtree ports/thsss
```

`ports/thsss` 有新提交后，用同一条命令把目录历史快进到 THSSS-WASM 的 `main`：

```sh
python tools/ports/sync-thsss-upstream.py
```

该命令导出子树、核对 `c08d073a6c0e789773879da8f1272c7afd96fe3a` 仍是祖先、目录树与导出树一致，然后非强制推送。当前导出提交是 `380cdc69ae894e7fc38701426b162d607dbf097d`，比连接基点多 16 个提交。monorepo 的 `tools/ports/`、`launcher/tests/` 和 `docs/adaptation/status.json` 留在 eagler-touhou。更新连接提交时再改机器登记里的 `latestJoin` 与 `currentSplit`。
