let p1Image1 = document.getElementById('p1Image1');
let p1Image2 = document.getElementById('p1Image2');
let p1Image4 = document.getElementById('p1Image4');
let p1Vaga1 = document.getElementById('p1Vaga1');
let p1Vaga2 = document.getElementById('p1Vaga2');
let p1Vaga4 = document.getElementById('p1Vaga4');
let p1Vaga1LastUpdate = document.getElementById('p1Vaga1LastUpdate');
let p1Vaga2LastUpdate = document.getElementById('p1Vaga2LastUpdate');
let p1Vaga4LastUpdate = document.getElementById('p1Vaga4LastUpdate');
let btnReservation1 = document.getElementById("btnReservation1");
let btnReserve1 = document.getElementById("btnReserve1");
let btnOpenReservation1 = document.getElementById("btnOpenReservation1");

let p2Image1 = document.getElementById('p2Image1');
let p2Image2 = document.getElementById('p2Image2');
let p2Image4 = document.getElementById('p2Image4');
let p2Vaga1 = document.getElementById('p2Vaga1');
let p2Vaga2 = document.getElementById('p2Vaga2');
let p2Vaga4 = document.getElementById('p2Vaga4');
let p2Vaga1LastUpdate = document.getElementById('p2Vaga1LastUpdate');
let p2Vaga2LastUpdate = document.getElementById('p2Vaga2LastUpdate');
let p2Vaga4LastUpdate = document.getElementById('p2Vaga4LastUpdate');
let btnReservation2 = document.getElementById("btnReservation2");
let btnReserve2 = document.getElementById("btnReserve2");
let btnOpenReservation2 = document.getElementById("btnOpenReservation2");


window.addEventListener('load', () => { window.localStorage.clear(); setInterval(receiveData, 1000); });


btnOpenReservation1.addEventListener("click", () => {
    if (window.localStorage.getItem("username") && window.localStorage.getItem("password"))
        sendReq("/openReserve1?username=" + window.localStorage.getItem("username"))
    else
        alert("FAÇA UMA RESERVA PRIMEIRO!");
});

btnOpenReservation2.addEventListener("click", () => {
    if (window.localStorage.getItem("username") && window.localStorage.getItem("password"))
        sendReq("/openReserve2?username=" + window.localStorage.getItem("username"))
    else
        alert("FAÇA UMA RESERVA PRIMEIRO!");
});

btnReserve1.addEventListener("click", () => {
    let username = document.getElementById("username1").value;
    let password = document.getElementById("password1").value;
    sendReserve("/reservation1?username=" + username + "&password=" + password, username, password);
});

btnReserve2.addEventListener("click", () => {
    let username = document.getElementById("username2").value;
    let password = document.getElementById("password2").value;
    sendReserve("/reservation2?username=" + username + "&password=" + password, username, password);
})

function sendReserve(url, username, password) {
    fetch(url)
        .then(resp => resp.json())
        .then((resp) => {
            console.log(resp);
            if (resp["status"] == "success") {
                window.localStorage.setItem("username", username);
                window.localStorage.setItem("password", password);
            }
            else
                window.localStorage.clear();
            alert(resp["message"]);
            cleanFields();
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); })
}


function cleanFields() {
    document.getElementById("username1").value = "";
    document.getElementById("password1").value = "";
    document.getElementById("username2").value = "";
    document.getElementById("password2").value = "";
}


function sendReq(url) {
    fetch(url)
        .then(resp => resp.json())
        .then((resp) => {
            console.log(resp);
            if (resp["status"] == "success")
                console.log("Aberto!");
            else
                alert(resp["message"]);
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); })
}


function receiveData() {
    fetch('/dados')
        .then(resp => resp.json())
        .then((resp) => {
            console.log(resp);
            let dados = resp["data"].split('*');
            if (dados[0] == 'D') {
            //  String txt = ("D*" + String(p1Vaga1) + "*" + String(p1Vaga2) + "*" + String(p1Reservation) + "*" + String(p2Vaga1) + "*" + String(p2Vaga2) + "*" + String(p2Reservation) + "*");
            //                 0           1                        2                         3                           4                       5                    6                          
                setPark(numberPark = 1, numberVaga = 1, status = (dados[1] == '1') ? "ocupada" : "livre");
                setPark(numberPark = 1, numberVaga = 2, status = (dados[2] == '1') ? "ocupada" : "livre");
                if (dados[7] == '1')
                    setPark(numberPark = 1, numberVaga = 3, status = "reservada", dados[9]+"s");
                else
                    setPark(numberPark = 1, numberVaga = 3, status = (dados[3] == '1') ? "ocupada" : "livre");

                setPark(numberPark = 2, numberVaga = 1, status = (dados[4] == '1') ? "ocupada" : "livre");
                setPark(numberPark = 2, numberVaga = 2, status = (dados[5] == '1') ? "ocupada" : "livre");
                if (dados[8] == '1')
                    setPark(numberPark = 2, numberVaga = 3, status = "reservada", dados[10]+"s");
                else
                    setPark(numberPark = 2, numberVaga = 3, status = (dados[6] == '1') ? "ocupada" : "livre");
            }
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); })
}

function setPark(numberPark, numberVaga, status, timeNow = new Date().toLocaleString()) {
    const images = {
        "livre": "vaga-livre.png",
        "ocupada": "vaga-ocupada.png",
        "ocupado": "vaga-ocupada.png",
        "reservada": "vaga-ocupada.png",
        "reservado": "vaga-ocupada.png",
    }
    if (numberPark == 1) {
        switch (numberVaga) {
            case 1:
                p1Vaga1.innerHTML = status.toUpperCase();
                p1Image1.src = images[status.toLowerCase()];
                p1Vaga1LastUpdate.innerHTML = timeNow;
                break;
            case 2:
                p1Vaga2.innerHTML = status.toUpperCase();
                p1Image2.src = images[status.toLowerCase()];
                p1Vaga2LastUpdate.innerHTML = timeNow;
                break;
            case 3:
                p1Vaga4.innerHTML = status.toUpperCase();
                p1Image4.src = images[status.toLowerCase()];
                p1Vaga4LastUpdate.innerHTML = timeNow;
                if (status == "livre")
                    btnReservation1.classList.remove("disabled");
                else
                    btnReservation1.classList.add("disabled");
                break;
        }
    }
    else {
        switch (numberVaga) {
            case 1:
                p2Vaga1.innerHTML = status.toUpperCase();
                p2Image1.src = images[status.toLowerCase()];
                p2Vaga1LastUpdate.innerHTML = timeNow;
                break;
            case 2:
                p2Vaga2.innerHTML = status.toUpperCase();
                p2Image2.src = images[status.toLowerCase()];
                p2Vaga2LastUpdate.innerHTML = timeNow;
                break;
            case 3:
                p2Vaga4.innerHTML = status.toUpperCase();
                p2Image4.src = images[status.toLowerCase()];
                p2Vaga4LastUpdate.innerHTML = timeNow;
                if (status == "livre")
                    btnReservation2.classList.remove("disabled");
                else
                    btnReservation2.classList.add("disabled");
                break;
        }
    }
}

