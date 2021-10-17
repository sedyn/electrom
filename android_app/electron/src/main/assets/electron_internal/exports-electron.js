// lib/browser/api/exports/electron.ts
const browserModules = require('./module-lists.js');
const { defineProperties } = require('./define-properties.js');

const electronExports = {};
const electronBrowserModules = defineProperties(electronExports, browserModules);

module.exports = electronBrowserModules;
