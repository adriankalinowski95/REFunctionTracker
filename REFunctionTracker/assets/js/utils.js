class ASMInst {
    constructor() {
        this.mnemonic = "";
        this.operands = "";
        this.instructionHex = "";
        this.offset = "";
        this.instructionIndex = "";
        this.isBreakPoint = false;
    }
}

function isFunction(func) {
    return typeof func !== 'undefined' && $.isFunction(func);
}

function isJSON(str) {
    try {
        return (JSON.parse(str) && !!str);
    } catch (e) {
        return false;
    }
}

function isInt(value) {
    return !isNaN(value) &&
        parseInt(Number(value)) == value &&
        !isNaN(parseInt(value, 10));
}

function isHex(h) {
    try {
        var a = parseInt(h, 16);
        if (Number.isInteger(a)) {
            return true;
        }
        return false;
    } catch (e) {
        return false;
    }
}

