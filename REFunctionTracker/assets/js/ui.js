function $(id) {
    return document.getElementById(id);
}

function HandleClick() {
    document.body.classList.add('transition');
    $('msg').innerHTML = GetMessage();
}

function loadHTML(div, fileName) {
    var url = fileName;
    var jsonFile = new XMLHttpRequest();
    jsonFile.open("GET", url, true);
    jsonFile.send();
    jsonFile.onreadystatechange = function () {
        if (jsonFile.readyState == 4 && jsonFile.status == 200) {
            $(div).innerHTML = jsonFile.responseText;
            leftNavTransition();
        }
    }
}
function afterLoad() {
    // loadHTML('top', 'top.html');
    loadHTML('content', 'content.html');
}

function menuButtonClick()
{
    var el = document.getElementById("menu-button");
    var content_center = document.getElementById("contet-center");
    var left_nav = document.getElementById("left-nav");
    if (content_center.classList.contains("contet-center-full-width"))
    {
        content_center.classList.remove("contet-center-full-width");
        left_nav.classList.remove("nav-small-height");
    } else
    {
        content_center.classList.add("contet-center-full-width");
        left_nav.classList.add("nav-small-height");
        var customTextList = document.querySelectorAll('.custom-text');
        customTextList.forEach(function (customText) {
            customText.classList.add('opacity-0');
            customText.classList.remove('opacity-1');
        });
        
    }
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
    var overlay = document.getElementById("overlay");
    if (overlay.classList.contains("overlay-visible")) {
        overlay.classList.remove("overlay-visible");
    } else {
        overlay.classList.add("overlay-visible");
    }
}
function closeOverlay() {
    var overlay = document.getElementById("overlay");
    if (overlay.classList.contains("overlay-visible")) {
        overlay.classList.remove("overlay-visible");
        var overlay_content = document.getElementById("overlay-content");
        overlay_content.innerHTML = "";
    } else {
        overlay.classList.add("overlay-visible");
    }
}

isShowProcessDialog = false;

function showSelectProcessDialog() {
    loadHTML('overlay-content', 'select-process-dialog.html');
    showOverlay();
}

function keyboardClick() {
    document.addEventListener('keydown', function (event) {
        if (event.keyCode === 112) {
            OnToggleTools();
        }
    });
}

function init() {
    afterLoad();
    keyboardClick();
    //document.querySelector('#home-button').addEventListener('click', event => OnToggleTools());
}


init();