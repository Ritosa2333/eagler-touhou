# 第三方构建依赖

本目录通过子模块引入 SDL、SDL_image 和 SDL_ttf，供 SDL3 / Emscripten 构建使用。版本由仓库 gitlink 固定；SDL 使用官方 3.4.12 提交 `f87239e71e42da91ca317a12eefb82cfbf3393eb`。

从本作目录初始化依赖：

```sh
git submodule update --init --recursive
```

Web 音频缓冲设置在 `src/SoundPlayer.cpp` 中通过 `SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES` 提供。构建步骤见[开发指南](../docs/DEVELOPMENT.md)。
