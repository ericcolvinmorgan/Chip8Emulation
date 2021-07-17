document.addEventListener("DOMContentLoaded", function () {
    let gamePaused = 0;
    let soundOn = 0;
    document.querySelector("#rom-dropdown").onchange = async (event) => {
        const gameSelection = event.target.options[event.target.selectedIndex];
        const response = await fetch(`./${gameSelection.value}`);
        if (response.ok) {
            const responseData = await response.arrayBuffer();
            const gameData = new Uint8Array(responseData);
            Module.ccall('loadROM', null, ['array'], [gameData]);
            Module.ccall('setTargetSpeed', null, ['number'], [gameSelection.dataset.rate]);
            console.log(`Loaded ${gameSelection.dataset.rate}`);
        }
        else {
            console.log(`Unable to fetch requested game: ${response.statusText}`);
        }
    }
    
    document.querySelector("#paused").onclick = async (event) => {
        gamePaused = !gamePaused;
        Module.ccall('setPaused', null, ['number'], [gamePaused]);
        if(gamePaused)
            event.currentTarget.innerText = "RESUME";
        else
            event.currentTarget.innerText = "PAUSE";
    }

    document.querySelector("#soundOn").onclick = async (event) => {
        soundOn = !soundOn;
        Module.ccall('setSoundOn', null, ['number'], [soundOn]);        
        if(soundOn)
            event.currentTarget.innerText = "SOUND: ON";
        else
            event.currentTarget.innerText = "SOUND: OFF";
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
        let cpuDetail = "";
        cpuDetail += `<div><div>INDEX REGISTER: ${this.indexRegister[0]}</div>`;
        cpuDetail += `<div>PROGRAM COUNTER: ${this.programCounter[0]}</div>`;
        cpuDetail += `<div>DELAY TIMER: ${this.delayTimer[0]}</div>`;
        cpuDetail += `<div>SOUND TIMER: ${this.soundTimer[0]}</div></div>`;
        for(i = 0; i < 16; i+=4)
        {
            cpuDetail += `<div><div>V${i}: ${this.registers[i]}</div>`;
            cpuDetail += `<div>V${i + 1}: ${this.registers[i + 1]}</div>`;
            cpuDetail += `<div>V${i + 2}: ${this.registers[i + 2]}</div>`;
            cpuDetail += `<div>V${i + 3}: ${this.registers[i + 3]}</div></div>`;
        }
        this.registersView.innerHTML = cpuDetail;
    }
}