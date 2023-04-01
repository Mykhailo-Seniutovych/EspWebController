init();

async function init() {
    const greenLed = document.getElementById("greenLed");
    const redLed = document.getElementById("redLed");

    const res = await fetch("/led");
    const ledStatus = await res.json();

    setTimeout(() => {
        if (ledStatus.greenLed) {
            greenLed.checked = true;
        }
        if (ledStatus.redLed) {
            redLed.checked = true;
        }

        greenLed.addEventListener("input", onGreenLed);
        redLed.addEventListener("input", onRedLed);
    }, 100);
}

async function onGreenLed(event) {
    const isChecked = event.target.checked;

    if (isChecked) {
        await fetch("/led/greenOn", { method: "POST" });
    } else {
        await fetch("/led/greenOff", { method: "POST" });
    }
}

async function onRedLed(event) {
    const isChecked = event.target.checked;

    if (isChecked) {
        await fetch("/led/redOn", { method: "POST" });
    } else {
        await fetch("/led/redOff", { method: "POST" });
    }
}