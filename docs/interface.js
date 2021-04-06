document.addEventListener("DOMContentLoaded", function () {
    document.querySelector("#rom-dropdown").onchange = async (event) => {
        const gameSelection = event.target.options[event.target.selectedIndex];
        const response = await fetch(`./${gameSelection.value}`);
        if(response.ok)
        {
            const responseData = await response.arrayBuffer();
            const gameData = new Uint8Array(responseData);
            Module.ccall('loadROM', null, ['array'], [gameData]);
            console.log(`Loaded ${gameSelection.dataset.rate}`);
        }
        else
        {
            console.log(`Unable to fetch requested game: ${response.statusText}`);
        }
    }
});

const Interface = {
    registersView: document.getElementById('registers'),
    indexRegister: null,
    programCounter: null,
    registers: null,
    delayTimer: null,
    soundTimer: null,
    updateScreen(value) {
        this.registersView.innerHTML = "";
        this.registersView.innerHTML += `<div>Index Register: ${this.indexRegister[0]}</div>`;
        this.registersView.innerHTML += `<div>Program Counter: ${this.programCounter[0]}</div>`;
        this.registersView.innerHTML += `<div>Delay Timer: ${this.delayTimer[0]}</div>`;
        this.registersView.innerHTML += `<div>Sound Timer: ${this.soundTimer[0]}</div>`;
        for(i = 0; i < 16; i++)
        {
            this.registersView.innerHTML += `<div>V${i}: ${this.registers[i]}</div>`;
        }
    }
}