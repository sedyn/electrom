// lib/browser/init.ts
// set EventEmitter first
const events = require('events');
const Module = require('module');
process._linkedBinding('electron_browser_event_emitter').setEventEmitterPrototype(events.EventEmitter.prototype);

// We modified the original process.argv to let node.js load the init.js,
// we need to restore it here.
process.argv.splice(1, 1);

require('./reset-search-paths.js');

Module._load(process.argv[1], Module, true)
