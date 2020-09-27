isShowProcessDialog = false;

function initHomePage() {
    document.querySelector('#select-process-button').addEventListener('click', showSelectProcessDialog);
}

function showSelectProcessDialog() {
    loadHTML('overlay-content', 'select-process-dialog.html', afterSelectProcessDialogLoad);
    showOverlay();
}
function afterSelectProcessDialogLoad() {
    initSelectProcessDialog();
}