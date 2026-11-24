
ARG DEBIAN_TAG=bookworm-slim

FROM debian:${DEBIAN_TAG} AS base
    ENV DEBIAN_FRONTEND=noninteractive LANG=C.UTF-8
    RUN apt-get update && apt-get install -y --no-install-recommends \
            ca-certificates curl git ripgrep p7zip-full ffmpeg jq \
            cmake ninja-build build-essential pkg-config unzip xz-utils file \
            bison flex \
            python3 python3-venv python3-pip python3-dev \
        && rm -rf /var/lib/apt/lists/*

FROM base AS node
    ARG NODE_VERSION=v22.23.3
    RUN curl -fsSL "https://nodejs.org/dist/${NODE_VERSION}/node-${NODE_VERSION}-linux-x64.tar.xz" \
        | tar -xJ -C /opt/ \
        && mv /opt/node-${NODE_VERSION}-linux-x64 /opt/node

FROM base AS jdk
    ARG JDK_URL=https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.12.1%2B1/OpenJDK21U-jdk_x64_linux_hotspot_21.0.12.1_1.tar.gz
    RUN curl -fsSL "$JDK_URL" | tar -xz -C /opt/ \
        && mv /opt/jdk-21.0.12.1+1 /opt/jdk

FROM base AS ghidra
    COPY --from=jdk /opt/jdk /opt/jdk
    ARG GHIDRA_URL=https://github.com/NationalSecurityAgency/ghidra/releases/download/Ghidra_12.1.3_build/ghidra_12.1.3_PUBLIC_20260817.zip
    RUN curl -fsSL "$GHIDRA_URL" -o /tmp/ghidra.zip \
        && unzip -q /tmp/ghidra.zip -d /opt/ \
        && mv /opt/ghidra_12.1.3_PUBLIC /opt/ghidra \
        && rm /tmp/ghidra.zip
    ENV JAVA_HOME=/opt/jdk

FROM base AS emsdk
    ARG EMSCRIPTEN_VERSION=6.0.9
    RUN git clone --depth 1 --branch ${EMSCRIPTEN_VERSION} \
            https://github.com/emscripten-core/emsdk /opt/emsdk \
        && cd /opt/emsdk \
        && ./emsdk install ${EMSCRIPTEN_VERSION} \
        && ./emsdk activate ${EMSCRIPTEN_VERSION} \
        && rm -rf .git downloads/zips
    ENV EMSDK=/opt/emsdk

FROM base AS wasi
    ARG WASI_URL=https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-34/wasi-sdk-34.0-x86_64-linux.tar.gz
    RUN curl -fsSL "$WASI_URL" | tar -xz -C /opt/ \
        && mv /opt/wasi-sdk-34.0*x86_64* /opt/wasi-sdk
    ENV WASI_SDK_PATH=/opt/wasi-sdk

FROM base AS pydeps
    RUN python3 -m venv /opt/venv \
        && /opt/venv/bin/pip install --no-cache-dir \
            unicorn capstone pillow numpy

FROM base AS dev
    COPY --from=node /opt/node /opt/node
    COPY --from=jdk /opt/jdk /opt/jdk
    COPY --from=ghidra /opt/ghidra /opt/ghidra
    COPY --from=emsdk /opt/emsdk /opt/emsdk
    COPY --from=wasi /opt/wasi-sdk /opt/wasi-sdk
    COPY --from=pydeps /opt/venv /opt/venv
    ENV PATH=/opt/node/bin:/opt/venv/bin:/opt/ghidra/support:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH \
        JAVA_HOME=/opt/jdk \
        EMSDK=/opt/emsdk \
        WASI_SDK_PATH=/opt/wasi-sdk
    ARG DEV_UID=1000
    ARG DEV_GID=1000
    RUN echo 'export PATH=/opt/node/bin:/opt/venv/bin:/opt/ghidra/support:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH' > /etc/profile.d/th13-path.sh \
        && groupadd -g $DEV_GID dev 2>/dev/null || true; \
        useradd -m -u $DEV_UID -g $DEV_GID -s /bin/bash dev \
        && mkdir -p /work && chown -R dev:dev /work
    USER dev
    WORKDIR /work
    COPY --chmod=0755 entrypoint.sh /usr/local/bin/entrypoint.sh
    ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
    CMD ["bash"]

FROM base AS ci
    COPY --from=node /opt/node /opt/node
    COPY --from=emsdk /opt/emsdk /opt/emsdk
    COPY --from=wasi /opt/wasi-sdk /opt/wasi-sdk
    COPY --from=pydeps /opt/venv /opt/venv
    ENV PATH=/opt/node/bin:/opt/venv/bin:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH \
        EMSDK=/opt/emsdk \
        WASI_SDK_PATH=/opt/wasi-sdk
    RUN echo 'export PATH=/opt/node/bin:/opt/venv/bin:/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH' > /etc/profile.d/th13-path.sh
    WORKDIR /work
    CMD ["bash"]

FROM base AS shot
    COPY --from=node /opt/node /opt/node
    ENV PATH=/opt/node/bin:$PATH
    RUN echo 'export PATH=/opt/node/bin:$PATH' > /etc/profile.d/th13-shot-path.sh \
        && apt-get update && apt-get install -y --no-install-recommends \
            chromium fonts-noto-cjk \
        && rm -rf /var/lib/apt/lists/* \
        && mkdir -p /opt/shot && cd /opt/shot \
        && npm install --omit=dev playwright-core@1.64.1 \
        && rm -rf /root/.npm
    ENV NODE_PATH=/opt/shot/node_modules
    ENV CHROMIUM_PATH=/usr/bin/chromium \
        PLAYWRIGHT_BROWSERS_PATH=0
    WORKDIR /work
    CMD ["bash"]
