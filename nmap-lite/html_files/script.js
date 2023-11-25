document.addEventListener("DOMContentLoaded", function () {
    var form = document.getElementById("dataForm");
    form.addEventListener("submit", displayData);
});

function displayData(event) {
    // Prevent the default form submission behavior
    event.preventDefault();

    // Get the value from the input field
    var userInput = document.getElementById("userInput").value;

    // Create a new paragraph element
    var para = document.createElement("p");

    // Set the text of the paragraph to the user input
    para.textContent = "You entered: " + userInput;

    // Append the paragraph to the display area
    document.getElementById("displayArea").appendChild(para);
}
