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