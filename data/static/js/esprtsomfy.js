const baseUrl = "/";

const endpointRemotesFetch = baseUrl + "api/v1/remotes/fetch";
const endpointRemoteAction = baseUrl + "api/v1/remotes/action";
const endpointRemoteCreate = baseUrl + "api/v1/remotes/create";
const endpointRemoteUpdate = baseUrl + "api/v1/remotes/update";
const endpointRemoteDelete = baseUrl + "api/v1/remotes/delete";
const endpointNetworksFetch = baseUrl + "api/v1/wifi/networks";
const endpointNetworkUpdate = baseUrl + "api/v1/wifi/config";
const endpointCoreRestart = baseUrl + "api/v1/core/restart";

// common utils
function getRequest(endpoint, onSuccess, onFailed) {
    fetch(endpoint).then(function (response) {
        return response.json();
    }).then(function (data) {
        if (onSuccess) {
            onSuccess(data);
        }
    }).catch(function (err) {
        console.log(err);
        if (onFailed) {
            onFailed();
        }
    });
}

function postRequest(endpoint, payload, onSuccess, onFailed) {
    fetch(endpoint, {
        method: "POST", body: JSON.stringify(payload), headers: {
            'Content-Type': 'application/json'
        },
    }).then(function (response) {
        return response.json();
    }).then(function (data) {
        console.log(data);
        if (onSuccess) {
            onSuccess(data);
        }
    }).catch(function (err) {
        console.log(err);
        if (onFailed) {
            onFailed();
        }
    });
}

function findFirstParentWithClass(targetElement, className) {
    let currentElement = targetElement;
    while (currentElement) {
        if (currentElement.classList.contains(className)) {
            return currentElement;
        }
        currentElement = currentElement.parentNode;
    }
    return null;
}

// index.html

function loadRemotes() {
    getRequest(endpointRemotesFetch, onRemotesFetched);
}

function onRemotesFetched(data) {
    remoteListEl = document.getElementById("blind-list");
    for (const element of data) {
        remoteListEl.innerHTML += `
        <div class="table-row remote-element" data-remote-id="${element.id}" data-remote-rolling-code="${element.rolling_code}" data-remote-name="${element.name}">
            <div class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">

                <p class="text-base font-medium text-gray-600" contenteditable="true" onkeydown="onRemoteNameChanged(event)">${element.name}</p>
                <button @click="showModalSettings = true; showModalBackdrop = true; selectedRemoteName = '${element.name}', selectedRemoteId = ${element.id}"><i class="fas fa-cogs"></i></button>

            </div>
            <div class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">
                <div class="grid grid-cols-1 sm:grid-cols-3 gap-2">
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="up"><i class="fas fa-chevron-up"></i></button>
                    </div>
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="stop"><i class="fas fa-stop"></i></button>
                    </div>
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="down"><i class="fas fa-chevron-down"></i></button>
                    </div>
                </div>
            </div>
        </div>`;
    }
    initRemoteActionButtons();
}

function initRemoteActionButtons() {
    document.querySelectorAll('.remote-action').forEach(button => {
        button.addEventListener('click', function (event) {

            const containerEl = findFirstParentWithClass(event.target, "remote-element");
            const buttonEl = findFirstParentWithClass(event.target, "remote-action");

            let remoteId = parseInt(containerEl.dataset.remoteId);
            let action = buttonEl.dataset.action;

            let payload = { remote_id: remoteId, action: action };
            console.log(payload)
            postRequest(endpointRemoteAction, payload); // TODO on fail

            containerEl.dataset.remoteRollingCode = parseInt(containerEl.dataset.remoteRollingCode) + 1;
        });
    });
}

function initRemoteSettingsButton() {
    const button = document.getElementById("remote-setting-action-btn");
    button.addEventListener('click', function (event) {
        onSettingButtonActionClicked(event);
    });
}

function initNewRemoteButton(){
    const button = document.getElementById("new-remote-btn");
    button.addEventListener('click', function (event) {
        onNewRemoteFormSubmited();
    });
}

function onRemoteNameChanged(event) {
    if (event.key === 'Enter') {

        event.target.contentEditable = 'false';

        const containerEl = findFirstParentWithClass(event.target, "remote-element");

        let remoteId = parseInt(containerEl.dataset.remoteId);
        let cleanedName = event.target.textContent.replace(/^\s+|\s+$|\s+(?!\S)/g, '');
        let newName = cleanedName.substring(0, 15); // TODO hardcoded

        event.target.innerHTML = newName;
        event.target.contentEditable = 'true';

        let payload = { remote_id: remoteId, data: { name: newName } };

        postRequest(endpointRemoteUpdate, payload); // TODO on fail

        containerEl.dataset.remoteName = newName;
    }
}

function onSettingButtonActionClicked(event) {

    let remoteId = event.target.dataset.remoteId;
    let action = event.target.dataset.remoteAction;
    const remoteElements = document.querySelectorAll('.remote-element[data-remote-id="' + remoteId + '"]');

    if (action === "delete") {
        let payload = { remote_id: remoteId };
        postRequest(endpointRemoteDelete, payload); // TODO on fail

        remoteElements.forEach(element => {
            element.parentNode.removeChild(element);
        });

    }
    else {
        let payload = { remote_id: remoteId, action: action };
        postRequest(endpointRemoteAction, payload); // TODO on fail

        if (action === "reset"){
            remoteElements.forEach(element => {
                element.dataset.remoteRollingCode = 0;
            });
        }
    }
}

function onNewRemoteFormSubmited() {
    const input = document.getElementById("remote-name");
    let remoteName = input.value;

    let payload = {name: remoteName};

    postRequest(endpointRemoteCreate, payload, (data) =>{
        remoteListEl = document.getElementById("blind-list");
        remoteListEl.innerHTML += `
        <div class="table-row remote-element" data-remote-id="${data.id}" data-remote-rolling-code="${data.rolling_code}" data-remote-name="${data.name}">
            <div class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">

                <p class="text-base font-medium text-gray-600" contenteditable="true" onkeydown="onRemoteNameChanged(event)">${data.name}</p>
                <button @click="showModalSettings = true; showModalBackdrop = true; selectedRemoteName = '${data.name}', selectedRemoteId = ${data.id}"><i class="fas fa-cogs"></i></button>

            </div>
            <div class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">
                <div class="grid grid-cols-1 sm:grid-cols-3 gap-2">
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="up"><i class="fas fa-chevron-up"></i></button>
                    </div>
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="stop"><i class="fas fa-stop"></i></button>
                    </div>
                    <div>
                        <button
                        class="text-sm leading-none text-gray-600 py-3 px-5 bg-gray-100 rounded hover:bg-gray-200 focus:outline-none remote-action" data-action="down"><i class="fas fa-chevron-down"></i></button>
                    </div>
                </div>
            </div>
        </div>`;
        initRemoteActionButtons();
    }); // TODO on fail
};

// configuration.html

function loadNetworks() {
    getRequest(endpointNetworksFetch, onNetworksFetched);
}

function onNetworksFetched(data) {
    networkListEl = document.getElementById("networks-list");
    for (const element of data) {
        networkListEl.innerHTML += `
        <a href="#" class="table-row network-element" data-ssid="${element.ssid}">
            <div
                class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">
                ${element.ssid}
            </div>
            <div
                class="table-cell border-b border-slate-100 dark:border-slate-700 p-4 text-sm leading-none text-gray-600">
                ${element.rssi} dBm
            </div>
        </a>`;
    }

    document.querySelectorAll('.network-element').forEach(link => {
        link.addEventListener('click', function (event) {
            event.preventDefault();
            document.getElementById("wifi-ssid").value = this.dataset.ssid;
        });
    });
}

function onNetworkConfigFormSubmited(event) {
    event.preventDefault();

    const ssid = document.getElementById("wifi-ssid").value;
    const password = document.getElementById("wifi-password").value;

    let payload = { "ssid": ssid, "password": password };

    postRequest(endpointNetworkUpdate, payload); // TODO on fail
}

function onRestartButtonClicked(event){
    postRequest(endpointCoreRestart, {}); // TODO on fail
}