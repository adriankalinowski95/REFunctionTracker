const INFO_DIALOG_ERROR = 1;
const INFO_DIALOG_SUCCESS = 2;
const INFO_DIALOG_UNKOWN = 4;

function isJSON(str) {
    try {
        return (JSON.parse(str) && !!str);
    } catch (e) {
        return false;
    }
}

function showInfoDialog(message, status) {
    var infoDialog = document.createElement('div');

    var strongMessage = "";
    infoDialog.classList.add("alert-dialog");
    if (status) {
        infoDialog.classList.add("success");
        strongMessage = "Success!";
    } else {
        infoDialog.classList.add("fail");
        strongMessage = "Fail!";
    }

    var exitSpan = document.createElement("SPAN");
    exitSpan.innerHTML += "&times;";
    exitSpan.classList.add("closebtn");

    var strong = document.createElement("STRONG");
    strong.appendChild(document.createTextNode(strongMessage));

    infoDialog.appendChild(exitSpan);
    infoDialog.appendChild(strong);
    infoDialog.innerHTML += " " + message;

    document.querySelector("#alerts").appendChild(infoDialog);
    //document.body.appendChild(infoDialog);

    infoDialog.classList.add("faded-out");
    requestAnimationFrame(() => {
        infoDialog.classList.remove("faded-out");
    });

    var close = document.getElementsByClassName("closebtn");
    for (var i = 0; i < close.length; i++) {
        close[i].onclick = function () {
            var div = this.parentElement;
            div.style.opacity = "0";
            setTimeout(function () {
                div.style.display = "none";
                div.remove();
            }, 600);
        }
    }
}

function isInfoDialogValidate(json) {
    var tempJson = json;

    if (!tempJson.hasOwnProperty("info-message")) {
        return false;
    }
    tempJson = tempJson["info-message"];
    if (!tempJson.hasOwnProperty("message")) {
        return false;
    }
    if (!tempJson.hasOwnProperty("status")) {
        return false;
    }

    return true;
}


function HandleClick() {
    document.body.classList.add('transition');
    $('#msg').innerHTML = GetMessage();
}


function loadHTML(div, fileName,callback) {
    var url = fileName;
    var jsonFile = new XMLHttpRequest();
    jsonFile.open("GET", url, true);
    jsonFile.onreadystatechange = function () {
        if (jsonFile.readyState == 4 && jsonFile.status == 200) {
            document.querySelector("#" + div).innerHTML = jsonFile.responseText;
            if (callback != undefined) {
                callback();
            }
        }
    }
    jsonFile.send();
}

function leftNavTransition() {
    var nav = document.querySelector('#left-nav');
    nav.addEventListener('transitionend', function(e) {
        var nav = document.querySelector('#left-nav');
        if (!nav.classList.contains("nav-small-height")) {
            var customTextList = document.querySelectorAll('.custom-text');
            customTextList.forEach(function (customText) {
                customText.classList.add('opacity-1');
                document.querySelector('.custom-text').classList.remove('opacity-0');
            });
        }
    });
}

function showOverlay() {
    var overlay = document.querySelector("#overlay");
    if (overlay.classList.contains("overlay-visible")) {
        overlay.classList.remove("overlay-visible");
    } else {
        overlay.classList.add("overlay-visible");
    }
}
function closeOverlay() {
    var overlay = document.querySelector("#overlay");
    if (overlay.classList.contains("overlay-visible")) {
        overlay.classList.remove("overlay-visible");
        var overlay_content = document.querySelector("#overlay-content");
        overlay_content.innerHTML = "";
    } else {
        overlay.classList.add("overlay-visible");
    }
}

function keyboardClick() {
    document.addEventListener('keydown', function (event) {
        if (event.keyCode === 112) {
            OnToggleTools();
        }
    });
}


function menuButtonClick() {
    var el = document.querySelector("#menu-button");
    var content_center = document.querySelector("#contet-center");
    var left_nav = document.querySelector("#left-nav");
    if (content_center.classList.contains("contet-center-full-width")) {
        content_center.classList.remove("contet-center-full-width");
        left_nav.classList.remove("nav-small-height");
    } else {
        content_center.classList.add("contet-center-full-width");
        left_nav.classList.add("nav-small-height");
        var customTextList = document.querySelectorAll('.custom-text');
        customTextList.forEach(function (customText) {
            customText.classList.add('opacity-0');
            customText.classList.remove('opacity-1');
        });

    }
}

function homePageButtonClicked() {
    loadHTML('page', 'home-page.html', afterHomePageLoad);
}

function testButtonClicked() {
    GetMessage();
}

function connectNavButtons() {
    document.querySelector('#home-button').addEventListener('click', event => homePageButtonClicked());
    document.querySelector('#menu-button').addEventListener('click', event => menuButtonClick());
    document.querySelector('#test-button').addEventListener('click', event => testButtonClicked());
}

function afterLoad() {
    loadHTML('page', 'home-page.html', afterHomePageLoad);
}

function init() {
    leftNavTransition();
    keyboardClick();
    afterLoad();
    connectNavButtons();
}

init();


function afterHomePageLoad() {
    initHomePage();
}

function testFunction(id) {
    console.log(id);
}