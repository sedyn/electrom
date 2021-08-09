const {app, BrowserWindow} = require('electron')

let window = null

app.once('ready', () => {
    window = new BrowserWindow({
        width: 800,
        height: 600,
        backgroundColor: '#D6D8DC',
        show: false,
    })

    window.loadURL(`file://${__dirname}/index.html`)
    window.once('ready-to-show', () => {
        window.show()
    })
})
