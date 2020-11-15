isShowProcessDialog = false;

class ProcessBaseInfo{
    constructor() {
        this.baseAddress = 0;
        this.entryPointAddress = 0;
        this.entryPointIndex = 0;
        this.instructionCount = 0;
    }
}

function isInt(value) {
    return !isNaN(value) &&
        parseInt(Number(value)) == value &&
        !isNaN(parseInt(value, 10));
}

function initHomePage() {
    var selectProcessButton = document.querySelector('#select-process-button');
    if (selectProcessButton) {
        selectProcessButton.addEventListener('click', showSelectProcessDialog);
    }
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider) {
        instSlider.addEventListener('change', instSliderChanged);
    }
    
}

function showSelectProcessDialog() {
    loadHTML('overlay-content', 'select-process-dialog.html', afterSelectProcessDialogLoad);
    showOverlay();
}

function afterSelectProcessDialogLoad() {
    initSelectProcessDialog();
}

function instSliderChanged(evt) {
    console.log(evt.target.value);
}

function processBaseInfoValidate(processBaseInfoJSON) {
    if (!processBaseInfoJSON.hasOwnProperty("processBaseInfo")) {
        return false;
    }
    processBaseInfoJSON = processBaseInfoJSON["processBaseInfo"];
    if (!processBaseInfoJSON.hasOwnProperty("baseAddress")) {
        return false;
    }
    if (!processBaseInfoJSON.hasOwnProperty("entryPointAddress")) {
        return false;
    }
    if (!processBaseInfoJSON.hasOwnProperty("entryPointIndex")) {
        return false;
    }
    if (!processBaseInfoJSON.hasOwnProperty("instructionCount")) {
        return false;
    }
    return true;
}

function loadBaseInformation() {
    var processBaseInfo = GetProcessInstructionsCount();

    if (!isJSON(processBaseInfo)) {
        return false;
    }

    var processBaseInfoJSON = JSON.parse(processBaseInfo);
    if (!processBaseInfoJSON.hasOwnProperty("processBaseInfo")) {
        return false;
    }
    processBaseInfoJSON = processBaseInfoJSON["processBaseInfo"];
    var instanceProcessBaseInfo = new ProcessBaseInfo();
    if (!Object.assign(instanceProcessBaseInfo, processBaseInfoJSON)) {
        return false;
    }
    console.log(instanceProcessBaseInfo)
    return instanceProcessBaseInfo;
}

function setSliderValues(processBaseInfoJSON) {
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider === null) {
        return false;
    }
    instSlider.min = 0;
    instSlider.max = processBaseInfoJSON["instructionCount"];
    instSlider.value = processBaseInfoJSON["entryPointIndex"];
    return true;
}


function getInstructionToLoadCount() {
    var rowHeight = getThRowHeight();
    if (rowHeight == 0) {
        return false;
    }

    var tBodyHeight = getTBodyHeight();
    if (tBodyHeight == 0) {
        return false;
    }

    var summaryPadding = 2;
    var instCount = tBodyHeight / (rowHeight + summaryPadding);
    return parseInt(instCount);
}

function loadInstructions(startIndex, count) {
    if(!Number.isInteger(count)){
        return false;
    }
    if (count <= 0) {
        return false;
    }
    console.log(1);
    var instructions = GetProcessInstructionByIndex(startIndex, count);

}

function loadDisAsmTable(status) {
    if (!status) {
        return;
    }

    var processBaseInfoJSON = loadBaseInformation();
    if (processBaseInfoJSON == false) {
        return;
    }

    if (setSliderValues(processBaseInfoJSON) == false) {
        return;
    }
    loadInstructions(processBaseInfoJSON["entryPointIndex"], getInstructionToLoadCount());
}

function getThRowHeight() {
    var thRow = document.querySelector("#th-row");
    if (thRow === null) {
        return 0;
    }
    var height = thRow.offsetHeight;
    console.log(height);
    return height;
}

function getTBodyHeight() {
    var customTBody = document.querySelector("#custom-tbody");
    if (customTBody === null) {
        return 0;
    }
    var height = customTBody.offsetHeight;
    return height;
}