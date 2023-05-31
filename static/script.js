fetch("treemap.svg")
  .then(response => {
    response.text().then(text => {
      document.getElementById("node").innerHTML = text
    })
  })
