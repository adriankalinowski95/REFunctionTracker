isShowProcessDialog = false;
var lastInstructions = []
var lastBaseInformation = null;


function initSearchFunctionsPage() {
    
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider) {
        instSlider.addEventListener('input', instSliderChanged2);
        instSlider.addEventListener('change', instSliderChanged2);
        instSlider.addEventListener('wheel', mouseWheelSlider2);
    }
    var customTBody = document.querySelector(".custom-tbody");
    if (customTBody) {
        customTBody.addEventListener('wheel', mouseWheelSlider2);
    }
    setSliderValues2();
    loadInstructionsByIndexWithCount2(0);
    console.log("init serach page");
}


function loadInstructionsByIndexWithCount2(startIndex) {
    loadInstructionsByIndex2(startIndex, getInstructionToLoadCount());
}


function mouseWheelSlider2(event) {
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
        loadInstructionsByIndexWithCount2(instSlider.value);
    } else {
        if (parseInt(instSlider.value) >= 0) {
            instSlider.value = parseInt(instSlider.value) - 1;
            loadInstructionsByIndexWithCount2(instSlider.value);
        }
    }
}


function loadInstructionsByIndex2(startIndex, count) {
    if (!Number.isInteger(count)) {
        return false;
    }
    if (count <= 0) {
        return false;
    }

    var instructions = LoadCallFunctions(startIndex, count);
    var instructionsJSON = JSON.parse(instructions);
    if (!instructionsJSON.hasOwnProperty("instructions")) {
        return false;
    }
    instructionsJSON = instructionsJSON["instructions"];
    loadInstToTable(instructionsJSON);
}


function instSliderChanged2(evt) {
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
    LoadCallFunctions(evt.target.value, getInstructionToLoadCount());
}

function setSliderValues2() {
    var instSlider = document.querySelector("#inst-slider");
    if (instSlider === null) {
        return false;
    }
    instSlider.min = 0;
    instSlider.max = GetSearchFunctionMax();
    instSlider.value = 0;
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
    var instCount = (diasmTableHeight - tHeadHeight) / (rowHeight + summaryPadding);
    return parseInt(instCount);
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

        var customTBody = document.querySelector(".custom-tbody");
        if (customTBody === null) {
            return false;
        }
        while (customTBody.firstChild) {
            customTBody.removeChild(customTBody.firstChild);
        }

        for (var i = 0; i < list.length; i++) {
            addASMToArray(customTBody, list[i]);
        }
        if (list.length > 0) {
            lastInstructions = list;
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
    cel1.setAttribute("data-offset", asmInst.offset);
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


    if (asmInst.isBreakPoint) {
        var breakPoint = document.createElement("a");
        breakPoint.classList.add("break-point");
        cel1.appendChild(breakPoint);
        row.classList.add("custom-table-debugg-row");
    }

}
function getThRowHeight() {
    var thRow = document.querySelector(".th-row");
    if (thRow === null) {
        return 0;
    }
    var height = thRow.offsetHeight;
    console.log(height);
    return height;
}

function getDisasmTableHeight() {
    var customDisasmTable = document.querySelector(".disasm-custom-table");
    if (customDisasmTable === null) {
        return 0;
    }
    var height = customDisasmTable.offsetHeight;
    return height;
}
function getTBodyHeight() {
    var customTBody = document.querySelector(".custom-tbody");
    if (customTBody === null) {
        return 0;
    }
    var height = customTBody.offsetHeight;
    return height;
}

function getTHeadHeight() {
    var customTHead = document.querySelector(".custom-thead");
    if (customTHead === null) {
        return 0;
    }
    var height = customTHead.offsetHeight;
    return height;
}
