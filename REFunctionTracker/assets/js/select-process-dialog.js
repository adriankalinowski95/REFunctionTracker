var gProcessList = null;
function isJSON(str) {
    try {
        return (JSON.parse(str) && !!str);
    } catch (e) {
        return false;
    }
}

function initSelectProcessDialog()
{
    document.querySelector('#select-process-dialog-button').addEventListener('click', getProcessListRequest);
    getProcessListRequest();
    var rows = document.querySelectorAll("#select-process-dialog-table table tbody tr");
    rows.forEach(function (row) {
        row.addEventListener('click', selectRow);
    });
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
        gProcessList = jsonProcessList; 
    }
}

function cleanTable() {
    var tbody = document.querySelector('#select-process-dialog-table tbody');
    if (tbody) {
        tbody.innerHTML = "";
    }
}

function addProcessRows(jsonProcessList) {
    var table = document.querySelector('#select-process-dialog-table tbody');
    if (!table) {
        return;
    }
    for (i in jsonProcessList) {
        addProcessRow(table, jsonProcessList[i]);
    }
}


function addProcessRow(table, process)
{
    if(!process.hasOwnProperty("processName")){
        return;
    }
    if(!process.hasOwnProperty("processPID")){
        return;
    }
    var row = table.insertRow(0);
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);

    cell2.innerHTML = process.processName;
    cell1.innerHTML = process.processPID.toString();
}

function selectRow() {
    var rows = document.querySelectorAll("#select-process-dialog-table table tbody tr");
    rows.forEach(function (row) {
        row.classList.remove("select-process-row-active");
    })
    this.classList.add("select-process-row-active");
}


function selectProcessTest(id) {
    console.log(id);
}