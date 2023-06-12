require(['gitbook'], function (gitbook) {
  gitbook.events.bind('page.change', function () {
    // move the edit link to the header, after font-setting icon
    var links = document.getElementsByClassName('links-link')
    for (var i=0; i<links.length; i++) {
      $(links.item(i)).insertAfter('div.font-settings')
    }
  })
})
