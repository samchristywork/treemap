function apply_hover() {
  const hoverElements = document.querySelectorAll('.hover-element');

  const tooltip = document.getElementById('tooltip');

  hoverElements.forEach((element) => {
    element.addEventListener('mouseover', (event) => {
      element.classList.add('hover-highlight');
      tooltip.innerHTML = element.getAttribute('data-tooltip');
      tooltip.style.display = 'block';
    });

    element.addEventListener('mouseout', () => {
      element.classList.remove('hover-highlight');
      tooltip.style.display = 'none';
    });

    element.addEventListener('mousemove', (event) => {
      tooltip.style.top = `${event.pageY + 10}px`;
      tooltip.style.left = `${event.pageX + 10}px`;
    });
  });
}

fetch("treemap.svg")
  .then(response => {
    response.text().then(text => {
      document.getElementById("node").innerHTML = text
      apply_hover();
    })
  })
