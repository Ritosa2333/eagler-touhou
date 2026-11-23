# th16 天空璋移植开发环境
# 多阶段与 ports/th13/container/Dockerfile 同构：base → node / jdk / ghidra / emsdk / wasi / pydeps → dev。
# 工具链版本与 docs/DEVELOPMENT.md「工具链」一致；相同阶段的指令文本保持不变，层缓存可在两作镜像间复用。
# 构建与运行统一走同目录 podman.sh（podman run --rm，本机不安装依赖）。
ARG DEBIAN_TAG=bookworm-slim

# ---------- base：系统依赖 ----------
FROM debian:${DEBIAN_TAG} AS base
    ENV DEBIAN_FRONTEND=noninteractive LANG=C.UTF-8
    RUN apt-get update && apt-get install -y --no-install-recommends \
            ca-certificates curl git ripgrep p7zip-full ffmpeg jq \
            cmake ninja-build build-essential pkg-config unzip xz-utils file \
            bison flex \
            python3 python3-venv python3-pip python3-dev \
        && rm -rf /var/lib/apt/lists/*

# ---------- node（工作区工具要求 Node.js ≥ 22.18） ----------
FROM base AS node
    ARG NODE_VERSION=v24.22.3
    RUN curl -fsSL "https://nodejs.org/dist/${NODE_VERSION}/node-${NODE_VERSION}-linux-x64.tar.xz" \
        | tar -xJ -C /opt/ \
        && mv /opt/node-${NODE_VERSION}-linux-x64 /opt/node

# ---------- jdk ----------
FROM base AS jdk
    ARG JDK_URL=https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.12.1%2B1/OpenJDK21U-jdk_x64_linux_hotspot_21.0.12.1_1.tar.gz
    RUN curl -fsSL "$JDK_URL" | tar -xz -C /opt/ \
        && mv /opt/jdk-21.0.12.1+1 /opt/jdk

# ---------- ghidra（web/scripts/cpp/analyze.mjs 的 headless 导出器，固定 12.1.3） ----------
FROM base AS ghidra
    COPY --from=jdk /opt/jdk /opt/jdk
    ARG GHIDRA_URL=https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_12.1.3_build/ghidra_12.1.3_PUBLIC_20260817.zip
    RUN curl -fsSL "$GHIDRA_URL" -o /tmp/ghidra.zip \
        && unzip -q /tmp/ghidra.zip -d /opt/ \
        && mv /opt/ghidra_12.1.3_PUBLIC /opt/ghidra \
        && rm /tmp/ghidra.zip
    ENV JAVA_HOME=/opt/jdk

# ---------- emsdk（portable/build.mjs，固定 6.0.9） ----------
FROM base AS emsdk
    ARG EMSCRIPTEN_VERSION=6.0.9
    RUN git clone --depth 1 --branch ${EMSCRIPTEN_VERSION} \
            https://github.com/emscripten-core/emsdk /opt/emsdk \
        && cd /opt/emsdk \
        && ./emsdk install ${EMSCRIPTEN_VERSION} \
        && ./emsdk activate ${EMSCRIPTEN_VERSION} \
        && rm -rf .git downloads/zips
    ENV EMSDK=/opt/emsdk

# ---------- wasi-sdk（portable/build-harness.mjs 的无头 harness，固定 34） ----------
FROM base AS wasi
    ARG WASI_URL=https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-34/wasi-sdk-34.0-x86_64-linux.tar.gz
    RUN curl -fsSL "$WASI_URL" | tar -xz -C /opt/ \
        && mv /opt/wasi-sdk-34.0*x86_64* /opt/wasi-sdk
    ENV WASI_SDK_PATH=/opt/wasi-sdk

# ---------- python 依赖（tools/oracle/*.py：Unicorn 直跑原版） ----------
FROM base AS pydeps
    RUN python3 -m venv /opt/venv \
        && /opt/venv/bin/pip install --no-cache-dir \
            unicorn==2.1.4 capstone pefile pillow numpy

# ---------- thtk（对照基准：thdat -x16 解包、thanm -l16 列表；extlib 子模块自带 libpng / zlib-ng） ----------
FROM base AS thtk
    ARG THTK_COMMIT=917a3d3858850f01276b7fb5b9327e2c24d27d5b
    RUN git init -q /opt/thtk-src \
        && git -C /opt/thtk-src fetch -q --depth 1 https://github.com/thpatch/thtk ${THTK_COMMIT} \
        && git -C /opt/thtk-src checkout -q --detach FETCH_HEAD \
        && git -C /opt/thtk-src submodule update -q --init --recursive --depth 1 \
        && cmake -S /opt/thtk-src -B /opt/thtk-src/build -DCMAKE_BUILD_TYPE=Release \
        && cmake --build /opt/thtk-src/build --parallel 2 \
        && cmake --install /opt/thtk-src/build --prefix /opt/thtk \
        && rm -rf /opt/thtk-src

# ---------- dev：开发镜像 ----------
FROM base AS dev
    COPY --from=node /opt/node /opt/node
    COPY --from=jdk /opt/jdk /opt/jdk
    COPY --from=ghidra /opt/ghidra /opt/ghidra
    COPY --from=emsdk /opt/emsdk /opt/emsdk
    COPY --from=wasi /opt/wasi-sdk /opt/wasi-sdk
    COPY --from=pydeps /opt/venv /opt/venv
    COPY --from=thtk /opt/thtk /opt/thtk
    ENV PATH=/opt/node/bin:/opt/venv/bin:/opt/thtk/bin:/opt/ghidra/support:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH \
        JAVA_HOME=/opt/jdk \
        EMSDK=/opt/emsdk \
        WASI_SDK_PATH=/opt/wasi-sdk \
        TH16_DECOMP_TOOLS=/opt \
        TH16_PYTHON=/opt/venv/bin/python \
        LD_LIBRARY_PATH=/opt/thtk/lib \
        PYTHONDONTWRITEBYTECODE=1
    # clang++：tests/run.sh 的本机 harness 编译器，与 emcc / wasi-sdk 同为 clang 系。
    # PYTHONDONTWRITEBYTECODE：挂载卷里不落 __pycache__。
    ARG DEV_UID=1000
    ARG DEV_GID=1000
    RUN apt-get update && apt-get install -y --no-install-recommends clang \
        && rm -rf /var/lib/apt/lists \
        && echo 'export PATH=/opt/node/bin:/opt/venv/bin:/opt/thtk/bin:/opt/ghidra/support:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH' > /etc/profile.d/th16-path.sh \
        && (groupadd -g $DEV_GID dev 2>/dev/null || true) \
        && useradd -m -u $DEV_UID -g $DEV_GID -s /bin/bash dev \
        && mkdir -p /work && chown -R dev:dev /work
    USER dev
    WORKDIR /work
    COPY --chmod=0755 entrypoint.sh /usr/local/bin/entrypoint.sh
    ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
    CMD ["bash"]
