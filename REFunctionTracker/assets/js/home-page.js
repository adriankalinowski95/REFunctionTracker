isShowProcessDialog = false;

class ProcessBaseInfo{
    constructor() {
        this.baseAddress = 0;
        this.entryPointAddress = 0;
        this.entryPointIndex = 0;
        this.instructionCount = 0;
        this.processLength = 0;
    }
}

class ASMInst{
    constructor(){
        this.mnemonic = "";
        this.operands = "";
        this.instructionHex = "";
        this.offset = "";
        this.instructionIndex = "";
    }
}


function initHomePage() {
    var selectProcessButton = document.querySelector('#select-process-button');
    if (selectProcessButton) {
        selectProcessButton.addEventListener('click', showSelectProcessDialog);
    }
    var searchButton = document.querySelector('#search-button');
    if (searchButton) {
        searchButton.addEventListener('click', showSearchDialog);
    }
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider) {
        instSlider.addEventListener('input', instSliderChanged);
        instSlider.addEventListener('change', instSliderChanged);
        instSlider.addEventListener('wheel', mouseWheelSlider);
    }
    var customTBody = document.querySelector("#custom-tbody");
    if (customTBody) {
        customTBody.addEventListener('wheel', mouseWheelSlider);
    }
}

function mouseWheelSlider(event) {
    console.log("into");
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider == null) {
        return;
    }
    if (instSlider.max == 0) {
        return;
    }
    
    event.preventDefault();
    delta = event.deltaY;
    console.log(delta);
    if (delta > 0) {
        instSlider.value = parseInt(instSlider.value) + 1;
        loadInstructionsByIndex(instSlider.value, getInstructionToLoadCount());
    } else {
        if (parseInt(instSlider.value) >= 0) {
            instSlider.value = parseInt(instSlider.value) - 1;
            loadInstructionsByIndex(instSlider.value, getInstructionToLoadCount());
        }
    }
}

function showSelectProcessDialog() {
    loadHTML('overlay-content', 'select-process-dialog.html', afterSelectProcessDialogLoad);
    showOverlay();
}

function showSearchDialog() {
    loadHTML('overlay-content', 'search-dialog.html', afterSearchDialogLoad);
    showOverlay();
}

function afterSelectProcessDialogLoad() {
    initSelectProcessDialog();
}
function afterSearchDialogLoad() {
    initSearchDialog();
}

function instSliderChanged(evt) {
    console.log(evt.target.value);
    var intNumber = evt.target.value;
    try {
        intNumber = parseInt(evt.target.value);
    } catch (e) {
        console.log(e);
        return;
    }
    if (!Number.isInteger(intNumber)) {
        return;
    }
    loadInstructionsByIndex(evt.target.value, getInstructionToLoadCount());
}

function loadBaseInformation() {
    var processBaseInfo = GetProcessBaseInfo();

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

    var tHeadHeight = getTHeadHeight();
    if (tHeadHeight == 0) {
        return false;
    }

    var diasmTableHeight = getDisasmTableHeight();
    if (diasmTableHeight == 0) {
        return false;
    }


    var summaryPadding = 0;
    var instCount = (diasmTableHeight - tHeadHeight)  / (rowHeight + summaryPadding);
    return parseInt(instCount);
}

function loadInstructionsByAddress(startAddress, count) {
    if(!Number.isInteger(count)){
        return false;
    }
    if (count <= 0) {
        return false;
    }

    var instructions = GetProcessInstructionByAddress(startAddress, count);
    var instructionsJSON = JSON.parse(instructions);
    if (!instructionsJSON.hasOwnProperty("instructions")) {
        return false;
    };
    instructionsJSON = instructionsJSON["instructions"];
    loadInstToTable(instructionsJSON);
}

function loadInstructionsByIndex(startIndex, count) {
    if (!Number.isInteger(count)) {
        return false;
    }
    if (count <= 0) {
        return false;
    }

    var instructions = GetProcessInstructionByIndex(startIndex, count);
    var instructionsJSON = JSON.parse(instructions);
    if (!instructionsJSON.hasOwnProperty("instructions")) {
        return false;
    }
    instructionsJSON = instructionsJSON["instructions"];
    loadInstToTable(instructionsJSON);
}

function loadInstToTable(instructionsJSON) {
    try {
        var list = [];
        for (var i = 0; i < instructionsJSON.length; i++) {
            var asmInst = new ASMInst();
            if (!Object.assign(asmInst, instructionsJSON[i])) {
                return false;
            }
            list.push(asmInst);
        }
        if (list.length <= 0) {
            return false;
        }

        var customTBody = document.querySelector("#custom-tbody");
        if (customTBody === null) {
            return false;
        }
        while (customTBody.firstChild) {
            customTBody.removeChild(customTBody.firstChild);
        }
        console.log(list);
        for (var i = 0; i < list.length; i++) {
            addASMToArray(customTBody, list[i]);
        }
        var instSlider = document.querySelector("#inst-slider");
        if (instSlider !== null) {
            instSlider.style.height = getTBodyHeight() + getTHeadHeight();
            console.log("last item of list! :" + list[0].instructionIndex);
            instSlider.value = list[0].instructionIndex;
        }
        return true;
    } catch (e) {
        console.log(e);
    }
}

function addASMToArray(tBody, asmInst) {
    var row = document.createElement("div");
    row.classList.add("custom-row");
    row.setAttribute("data-index", asmInst.instructionIndex);

    var cel1 = document.createElement("div");
    var cel2 = document.createElement("div");
    var cel3 = document.createElement("div");
    var cel4 = document.createElement("div");
    var cel5 = document.createElement("div");

    cel1.classList.add("custom-td");
    cel1.setAttribute("data-index", asmInst.instructionIndex);
    cel2.classList.add("custom-td");
    cel3.classList.add("custom-td");
    cel4.classList.add("custom-td");
    cel5.classList.add("custom-td");

    cel1.classList.add("custom-table-el-10");
    cel2.classList.add("custom-table-el-20");
    cel3.classList.add("custom-table-el-20");
    cel4.classList.add("custom-table-el-20");
    cel5.classList.add("custom-table-el-30");

    //cel1.innerHTML = process.processPID.toString();
    cel2.innerHTML = asmInst.offset;
    cel3.innerHTML = asmInst.mnemonic;
    cel4.innerHTML = asmInst.operands;
    cel5.innerHTML = asmInst.instructionHex;

    row.appendChild(cel1);
    row.appendChild(cel2);
    row.appendChild(cel3);
    row.appendChild(cel4);
    row.appendChild(cel5);

    tBody.appendChild(row);

    cel1.addEventListener("click", toggleBreakPoint);
}

function toggleBreakPoint() {
    var index = this.getAttribute("data-index");
    if (!index) {
        return;
    }
    if (!isInt(index)) {
        return;
    }
    var intIndex = parseInt(index, 10);
    var status = ToggleBreakPoint(intIndex);
    console.log(status);
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
    loadInstructionsByIndex(processBaseInfoJSON["entryPointIndex"], getInstructionToLoadCount());
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

function getDisasmTableHeight() {
    var customDisasmTable = document.querySelector("#disasm-custom-table");
    if (customDisasmTable === null) {
        return 0;
    }
    var height = customDisasmTable.offsetHeight;
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

function getTHeadHeight() {
    var customTHead = document.querySelector("#custom-thead");
    if (customTHead === null) {
        return 0;
    }
    var height = customTHead.offsetHeight;
    return height;
}