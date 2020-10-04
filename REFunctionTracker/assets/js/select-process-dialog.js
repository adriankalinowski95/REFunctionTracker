var gProcessList = null;
var gProcIndex = -1;

function isJSON(str) {
    try {
        return (JSON.parse(str) && !!str);
    } catch (e) {
        return false;
    }
}


function initSelectProcessDialog()
{
    document.querySelector('#select-process-dialog-button').addEventListener('click', setProcessRequest);
    getProcessListRequest();
}

function getProcessListRequest() {
    var processListData = GetProcessList();
    if (isJSON(processListData)) {
        var jsonProcessList = JSON.parse(processListData);
        if (!jsonProcessList.hasOwnProperty("processList")) {
            return;
        }

        jsonProcessList = jsonProcessList["processList"];
        cleanTable();
        addProcessRows(jsonProcessList);
        addSelectRowClickEvent();
        gProcessList = jsonProcessList; 
        gProcIndex = -1;
    }
}
/* Dodac set process i dialog potwierdzenia */
function setProcessRequest() {
    if (gProcIndex != -1) {
        console.log("process " + gProcIndex.toString());
        var procTestObj = { processData: gProcessList[gProcIndex] };
        console.log(procTestObj);
        var setProcessInfo = SetProcess(JSON.stringify(procTestObj));
        console.log(setProcessInfo);
    }
}

function cleanTable() {
    var tbody = document.querySelector('#select-process-dialog-table .custom-tbody');
    if (tbody) {
        tbody.innerHTML = "";
    }
}

function addSelectRowClickEvent() {
    var rows = document.querySelectorAll("#select-process-dialog-table .custom-table .custom-tbody .custom-row");
    rows.forEach(function (row) {
        row.addEventListener('click', selectRow);
    });
}
function addProcessRows(jsonProcessList) {
    var table = document.querySelector('#select-process-dialog-table .custom-tbody');
    if (!table) {
        return;
    }
    for (i in jsonProcessList) {
        addProcessRow(table, jsonProcessList[i],i);
    }
}


function addProcessRow(table, process,index)
{
    if(!process.hasOwnProperty("processName")){
        return;
    }
    if(!process.hasOwnProperty("processPID")){
        return;
    }
    var row = document.createElement("div");
    row.classList.add("custom-row");
    row.setAttribute("data-index", index.toString());

    var cel1 = document.createElement("div");
    var cel2 = document.createElement("div");


    cel1.classList.add("custom-td");
    cel2.classList.add("custom-td");

    cel1.classList.add("custom-table-el-50");
    cel2.classList.add("custom-table-el-50");

    cel1.innerHTML = process.processPID.toString();
    cel2.innerHTML = process.processName;

    row.appendChild(cel1);
    row.appendChild(cel2);

    table.appendChild(row);
}

function selectRow() {
    var rows = document.querySelectorAll("#select-process-dialog-table .custom-table .custom-tbody .custom-row");
    rows.forEach(function (row) {
        row.classList.remove("custom-row-active");
    })
    this.classList.add("custom-row-active");
    if (this.hasAttribute("data-index")) {
        gProcIndex = parseInt(this.getAttribute("data-index"));
    } else {
        gProcIndex = -1;
    }
}


function selectProcessTest(id) {
    console.log(id);
}