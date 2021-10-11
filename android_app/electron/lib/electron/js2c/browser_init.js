const events = require('events');
const Module = require('module');

// We modified the original process.argv to let node.js load the init.js,
// we need to restore it here.
process.argv.splice(1, 1);

process._linkedBinding('electron_browser_event_emitter').setEventEmitterPrototype(events.EventEmitter.prototype);

console.log('BROWSER INIT');

Module._load(process.argv[1], Module, true)
