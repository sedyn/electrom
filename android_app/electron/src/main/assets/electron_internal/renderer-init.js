(function (window) {
    class IpcRenderer {
        handler = {}

        sendSync(channel, data) {
            return window['@@android'].ipcRendererSendSync(channel, data);
        }

        on(channel, cb) {
            this.handler[channel] = cb;
            console.log('handler registered : ' + channel);
        }

        emit(channel, data) {
            const cb = this.handler[channel];
            if (cb) {
                console.log('handler triggered : ' + channel)
                cb({}, data);
            } else {
                console.log('handler not found : ' + channel)
            }
        }

        send(channel, data) {
            const trackId = window['@@android'].ipcRendererSend(channel, data);
        }
    };

    const ipcRenderer = new IpcRenderer();

    window['@@electron'] = {
        ipcRenderer
    };

    window.require = function (path) {
        if (path === 'electron') {
            return window['@@electron'];
        } else {
            throw new Error('Module not found ' + path)
        }
    };

    window.module = {};
})(window);
