isSearchDialogOn = false;


function initSearchDialog() {
    var goAddressButton = document.querySelector('#go-address-button');
    if (goAddressButton) {
        goAddressButton.addEventListener('click', goAddressButtonClicked);
    }

    var goIndexButton = document.querySelector('#go-index-button');
    if (goAddressButton) {
        goIndexButton.addEventListener('click', goIndexButtonClicked);
    }
}

function setInputState(state, nameID) {
    var inputTextField = document.querySelector("#" + nameID);
    if (!inputTextField) {
        return;
    }
    if (state) {
        if (inputTextField.classList.contains('search-dialog-edit-text-err')){
            inputTextField.classList.remove('search-dialog-edit-text-err');
        }
        return;
    }
    if (!state) {
        if (!inputTextField.classList.contains('search-dialog-edit-text-err')){
            inputTextField.classList.add('search-dialog-edit-text-err');
        }
        return;
    }
}


function goAddressButtonClicked() {
    if (!isFunction(loadBaseInformation)) {
        return;
    }
    var processBaseInfo = loadBaseInformation();
    if (processBaseInfo == null) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }

    var editText = document.querySelector('#search-dialog-address-edit-text');
    if (!editText) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }

    var hexText = editText.value;
    if (!hexText) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }
    if (!isHex(hexText)) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }

    var address = parseInt(hexText, 16);
    if (address < processBaseInfo.baseAddress ||
        address > (processBaseInfo.baseAddress + processBaseInfo.processLength)) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }
    setInputState(true, "search-dialog-address-edit-text");
    loadInstructionsByAddress(address, getInstructionToLoadCount());
    
}

function goIndexButtonClicked() {
    if (!isFunction(loadBaseInformation)) {
        return;
    }
    var editText = document.querySelector('#search-dialog-index-edit-text');
    if (!editText) {
        return;
    }

    var processBaseInfo = loadBaseInformation();
    if (processBaseInfo == null) {
        setInputState(false, "search-dialog-index-edit-text");
        return;
    }

    var hexText = editText.value;
    if (!hexText) {
        setInputState(false, "search-dialog-index-edit-text");
        return;
    }
    if (!isInt(hexText)) {
        setInputState(false, "search-dialog-index-edit-text");
        return;
    }

    var index = parseInt(hexText, 10);
    if (index < 0 ||
        index > (processBaseInfo.instructionCount)) {
        setInputState(false, "search-dialog-address-edit-text");
        return;
    }
    setInputState(true, "search-dialog-address-edit-text");
    loadInstructionsByIndex(index, getInstructionToLoadCount());

}