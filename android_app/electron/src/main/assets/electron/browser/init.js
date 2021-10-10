const events = require('events');
const Module = require('module');

process.argv.splice(1, 1);

// We modified the original process.argv to let node.js load the init.js,
// we need to restore it here.
process._linkedBinding('electron_browser_event_emitter').setEventEmitterPrototype(events.EventEmitter.prototype);

console.log('BROWSER INIT');
