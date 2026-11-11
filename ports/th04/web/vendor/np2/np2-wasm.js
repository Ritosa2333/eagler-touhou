;
function applyDefaultConfig(config) {
    return Object.assign({
        fontfile: 'font.bmp'
    }, config);
}
class NP2 {
    #state = 'loading';
    module;
    config;
    get state() { return this.#state; }
    constructor(config, createModule, resolveReady, rejectReady) {
        this.config = config;
        const module = this.module = {
            canvas: this.config.canvas,
            noImageDecoding: this.config.workerRuntime === true,
            noAudioDecoding: this.config.workerRuntime === true,
            nativeSoloAudio: this.config.nativeSoloAudio === true,
            lockstepEpoch: this.config.lockstepEpoch,
            preRun: [
                () => {
                    const url = new URL(config.fontfile, import.meta.url).href;
                    module.FS.createPreloadedFile('/', config.fontfile, url, true, false);
                },
            ],
            onReady: () => {
                if (!config.lockstep) {
                    module.pauseMainLoop();
                    document.addEventListener('visibilitychange', this.onVisibilityChange.bind(this));
                }
                this.#state = 'ready';
                resolveReady(this);
            },
            onExit: this.onExit.bind(this),
            getConfig: this.getConfig.bind(this),
            setConfig: this.setConfig.bind(this),
            onDiskChange: this.onDiskChange.bind(this),
        };
        createModule(module).catch(rejectReady);
    }
    run() {
        if (this.#state === 'ready' || this.#state === 'paused') {
            this.#state = 'running';
            if (!this.config.lockstep) this.module._np2_resume();
        }
    }
    pause() {
        if (this.#state === 'running') {
            this.#state = 'paused';
            if (!this.config.lockstep) this.module._np2_pause();
        }
    }
    reset() {
        if (this.#state === 'exited') {
            this.#state = 'running';
            this.module._np2_resume();
        }
        this.module._np2_reset();
    }
    step() {
        if (!this.config.lockstep || this.#state !== 'running') throw Error('模拟器未进入帧同步运行状态');
        this.module.netStep();
    }
    addDiskImage(name, bytes) {
        this.module.FS.writeFile(name, bytes);
    }
    getDiskImage(name) {
        return this.module.FS.readFile(name, { encoding: 'binary' });
    }
    setFdd(drive, name) {
        if (!name) {
            // Eject.
            this.module.ccall('diskdrv_setfddex', undefined, ['number', 'number', 'number', 'number'], [drive, 0, 0, 0]);
            return;
        }
        try {
            this.module.FS.stat(name, undefined);
        }
        catch (err) {
            throw new Error(`${name}: Invalid disk image name`);
        }
        this.module.ccall('diskdrv_setfddex', undefined, ['number', 'string', 'number', 'number'], [drive, name, 0, 0]);
    }
    setHdd(drive, name) {
        if (!name) {
            // Disconnect.
            this.module.ccall('diskdrv_setsxsi', undefined, ['number', 'number'], [drive, 0]);
        }
        else {
            try {
                this.module.FS.stat(name, undefined);
            }
            catch (err) {
                throw new Error(`${name}: Invalid disk image name`);
            }
            this.module.ccall('diskdrv_setsxsi', undefined, ['number', 'string'], [drive, name]);
        }
        if (this.#state === 'ready') {
            this.reset();
        }
        else {
            console.log('setHdd() called after boot. It will not take effect until reset.');
        }
    }
    getConfig(pName, type, pValue, size) {
        var value = this.config[this.module.UTF8ToString(pName)];
        switch (type) {
            case 0 /* IniType.STR */:
                if (typeof value === 'string')
                    this.module.stringToUTF8(value, pValue, size);
                break;
            case 1 /* IniType.BOOL */:
                if (typeof value === 'boolean')
                    this.module.HEAP8[pValue] = value ? 1 : 0;
                break;
            case 2 /* IniType.BYTEARG */:
                if (Array.isArray(value) && value.length == size) {
                    for (var i = 0; i < size; i++)
                        this.module.HEAPU8[pValue + i] = value[i];
                }
                break;
            case 3 /* IniType.SINT8 */:
                if (typeof value === 'number')
                    this.module.HEAP8[pValue] = value;
                break;
            case 6 /* IniType.UINT8 */:
            case 9 /* IniType.HEX8 */:
                if (typeof value === 'number')
                    this.module.HEAPU8[pValue] = value;
                break;
            case 4 /* IniType.SINT16 */:
                if (typeof value === 'number')
                    this.module.HEAP16[pValue >> 1] = value;
                break;
            case 7 /* IniType.UINT16 */:
            case 10 /* IniType.HEX16 */:
                if (typeof value === 'number')
                    this.module.HEAPU16[pValue >> 1] = value;
                break;
            case 5 /* IniType.SINT32 */:
                if (typeof value === 'number')
                    this.module.HEAP32[pValue >> 2] = value;
                break;
            case 8 /* IniType.UINT32 */:
            case 11 /* IniType.HEX32 */:
                if (typeof value === 'number')
                    this.module.HEAPU32[pValue >> 2] = value;
                break;
            default:
                console.warn('getConfig: unknown type ' + type);
                break;
        }
    }
    setConfig(pName, type, pValue, size) {
        const name = this.module.UTF8ToString(pName);
        switch (type) {
            case 0 /* IniType.STR */:
                this.config[name] = this.module.UTF8ToString(pValue);
                break;
            case 1 /* IniType.BOOL */:
                this.config[name] = this.module.HEAP8[pValue] ? true : false;
                break;
            case 2 /* IniType.BYTEARG */:
                var a = [];
                for (var i = 0; i < size; i++)
                    a[i] = this.module.HEAPU8[pValue + i];
                this.config[name] = a;
                break;
            case 3 /* IniType.SINT8 */:
                this.config[name] = this.module.HEAP8[pValue];
                break;
            case 6 /* IniType.UINT8 */:
            case 9 /* IniType.HEX8 */:
                this.config[name] = this.module.HEAPU8[pValue];
                break;
            case 4 /* IniType.SINT16 */:
                this.config[name] = this.module.HEAP16[pValue >> 1];
                break;
            case 7 /* IniType.UINT16 */:
            case 10 /* IniType.HEX16 */:
                this.config[name] = this.module.HEAPU16[pValue >> 1];
                break;
            case 5 /* IniType.SINT32 */:
                this.config[name] = this.module.HEAP32[pValue >> 2];
                break;
            case 8 /* IniType.UINT32 */:
            case 11 /* IniType.HEX32 */:
                this.config[name] = this.module.HEAPU32[pValue >> 2];
                break;
            default:
                console.warn('setConfig: ' + name + ' has unknown type ' + type);
                break;
        }
    }
    onExit() {
        // This is called deep inside pccore, so do the actual work asynchronously.
        setTimeout(() => {
            this.pause();
            this.#state = 'exited';
            if (this.config.onExit) {
                this.config.onExit();
            }
        }, 0);
    }
    onDiskChange(pName) {
        if (this.config.onDiskChange) {
            this.config.onDiskChange(this.module.UTF8ToString(pName));
        }
    }
    onVisibilityChange() {
        if (document.visibilityState === 'hidden') {
            if (this.#state === 'running')
                this.pause();
        }
        else {
            if (this.#state === 'paused')
                this.run();
        }
    }
}
export class NP21 extends NP2 {
    static async create(config) {
        const factory = (await import(config.lockstep ? './np21-lockstep.js' : './np21-60.js')).default;
        return new Promise((resolve, reject) => {
            new NP21(applyDefaultConfig(config), factory, resolve, reject);
        });
    }
}
