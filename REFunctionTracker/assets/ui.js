    function $(id) {
      return document.getElementById(id);
    }

    function HandleClick() {
      document.body.classList.add('transition');
      $('msg').innerHTML = GetMessage();
    }