let tableData = document.getElementById('tableData');


window.addEventListener('load', () => { 
    window.localStorage.clear(); 
    setInterval(receiveReservesData, 5000);
    setInterval(receiveUsersData, 5000); 
});


function receiveReservesData() {
    fetch('/reserves-estatisticas')
        .then(resp => resp.json())
        .then((resp) => {
            console.log(resp);
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); })
}

function receiveUsersData() {
    fetch('/users-estatisticas')
        .then(resp => resp.json())
        .then((resp) => {
            console.log(resp);
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); })
}
