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
        }
    }
}
function afterLoad() {
    // loadHTML('top', 'top.html');
    loadHTML('content', 'content.html');
}

afterLoad();