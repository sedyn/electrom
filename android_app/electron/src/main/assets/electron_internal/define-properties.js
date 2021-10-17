// lib/common/define-properties.ts
const handleESModule = (loader) => () => {
    const value = loader();
    if (value.__esModule && value.default) return value.default;
    return value;
}

function defineProperties(targetExports, moduleList) {
    const descriptors = {};
    for (const module of moduleList) {
        descriptors[module.name] = {
            enumerable: !module.private,
            get: handleESModule(module.loader)
        };
    }
    return Object.defineProperties(targetExports, descriptors);
}

module.exports = {
    defineProperties
};
