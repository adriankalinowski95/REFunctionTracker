function initSelectProcessDialog() {
    document.querySelector('#select-process-dialog-button').addEventListener('click', selectProcess);
}

function selectProcess() {
    console.log(GetMessageFromSelectProcess());
}