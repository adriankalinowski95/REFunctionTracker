isShowProcessDialog = false;
var lastInstructions = []
var lastBaseInformation = null;


function initSearchFunctionsPage() {
    loadInstructionsByIndexWithCount2(0);
    console.log("init serach page");
}


function loadInstructionsByIndexWithCount2(startIndex) {
    loadInstructionsByIndex2(startIndex, getInstructionToLoadCount2());
}

function loadInstructionsByIndex2(startIndex, count) {
    if (!Number.isInteger(count)) {
        return false;
    }
    if (count <= 0) {
        return false;
    }

    var instructions = LoadCallFunctions(startIndex, count);
    console.log(instructions);
    var instructionsJSON = JSON.parse(instructions);
    if (!instructionsJSON.hasOwnProperty("instructions")) {
        return false;
    }
    instructionsJSON = instructionsJSON["instructions"];
    loadInstToTable2(instructionsJSON);
}

function getInstructionToLoadCount2() {
    var rowHeight = getThRowHeight2();
    if (rowHeight == 0) {
        return false;
    }

    var tHeadHeight = getTHeadHeight();
    if (tHeadHeight == 0) {
        return false;
    }

    var diasmTableHeight = getDisasmTableHeight2();
    if (diasmTableHeight == 0) {
        return false;
    }


    var summaryPadding = 0;
    var instCount = (diasmTableHeight - tHeadHeight) / (rowHeight + summaryPadding);
    return parseInt(instCount);
}

function loadInstToTable2(instructionsJSON) {
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
            addASMToArray2(customTBody, list[i]);
        }
        if (list.length > 0) {
            lastInstructions = list;
        }
        var instSlider = document.querySelector("#inst-slider");
        if (instSlider !== null) {
            instSlider.style.height = getTBodyHeight2() + getTHeadHeight2();
            console.log("last item of list! :" + list[0].instructionIndex);
            instSlider.value = list[0].instructionIndex;
        }
        return true;
    } catch (e) {
        console.log(e);
    }
}

function addASMToArray2(tBody, asmInst) {
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

    cel1.addEventListener("click", toggleBreakPoint2);


    if (asmInst.isBreakPoint) {
        var breakPoint = document.createElement("a");
        breakPoint.classList.add("break-point");
        cel1.appendChild(breakPoint);
        row.classList.add("custom-table-debugg-row");
    }

}

function getThRowHeight2() {
    var thRow = document.querySelector(".th-row");
    if (thRow === null) {
        return 0;
    }
    var height = thRow.offsetHeight;
    console.log(height);
    return height;
}

function getDisasmTableHeight2() {
    var customDisasmTable = document.querySelector(".disasm-custom-table");
    if (customDisasmTable === null) {
        return 0;
    }
    var height = customDisasmTable.offsetHeight;
    return height;
}

function getTBodyHeight2() {
    var customTBody = document.querySelector(".custom-tbody");
    if (customTBody === null) {
        return 0;
    }
    var height = customTBody.offsetHeight;
    return height;
}

function getTHeadHeight2() {
    var customTHead = document.querySelector(".custom-thead");
    if (customTHead === null) {
        return 0;
    }
    var height = customTHead.offsetHeight;
    return height;
}

function toggleBreakPoint2() {
    var index = this.getAttribute("data-index");
    var dataOffset = this.getAttribute("data-offset");
    console.log(dataOffset);
    if (!index) {
        return;
    }
    if (!dataOffset) {
        return;
    }
    if (!isInt(index)) {
        return;
    }
    var intDataOffset = parseInt(dataOffset, 16);
    var status = ToggleBreakPoint(intDataOffset);
    console.log("break point from serach function!");
    /*
    if (status && lastInstructions.length > 0) {
        loadInstructionsByAddressWithCount(parseInt(lastInstructions[0].offset, 16));
    }*/
}