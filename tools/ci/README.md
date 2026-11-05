# CI 来源资产清单

发布审计读取 [`launcher/config/reviewed-source-assets.json`](../../launcher/config/reviewed-source-assets.json)。清单登记有效资产，并逐项核对资产与来源证据的 SHA-256。资产和证据齐全、哈希一致时通过检查。

[`docs/migration/reviewed-source-assets.json`](../../docs/migration/reviewed-source-assets.json) 和同目录其他 JSON 保存迁移历史。有效清单从中收录文件存在且哈希匹配的资产；历史文件保存原始字节。

[`fixtures/th08-packaging-source.txt`](fixtures/th08-packaging-source.txt) 是旧 `ports/th08-mp/portable/package-architecture.mjs` 的文本快照，来源提交为 `991e7cdf9060252e022eba25431254c6a0ce73e9`，原脚本 blob SHA-256 为 `219578162ce4f2622358f71d39895ef9ec80f5ac9f09c3f40121a0ca2d2e270f`。该 `.txt` 文件供来源核对。脚本的 TH08 分支列出 `spessasynth.mjs`、`stb-vorbis.mjs`、两份许可证，并将这些文件复制到发布包；当前清单另行固定相应文件和 `stb-vorbis` 来源记录的哈希。
