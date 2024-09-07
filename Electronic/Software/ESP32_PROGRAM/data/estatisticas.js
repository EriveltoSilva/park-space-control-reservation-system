window.addEventListener('load', () => { window.localStorage.clear();setInterval(receiveReservesData, 5000);setInterval(receiveUsersData, 5000);});
function receiveUsersData() {
    fetch('/users-estatisticas')
        .then(resp => resp.json())
        .then((data) => {
            // Limpar os dados e remover aspas desnecessárias
            const cleanedData = Object.values(data).map(item => {
                const cleanString = item.replace(/"/g, ''); // Remove aspas
                return cleanString.split('|'); // Divide os dados em array
            });
            
            // Definir o total de usuários
            document.getElementById('totalUsers').textContent = cleanedData.length;

            populateUsersTable(cleanedData); // Chama a função para popular a tabela
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); });
}

function receiveReservesData() {
    fetch('/reserves-estatisticas')
        .then(resp => resp.json())
        .then((data) => {
            // Limpar os dados e remover aspas desnecessárias
            const cleanedData = Object.values(data).map(item => {
                const cleanString = item.replace(/"/g, ''); // Remove aspas
                return cleanString.split('|'); // Divide os dados em array
            });

            // Contar quantas reservas estão no Parque1 e no Parque2
            const parque1Count = cleanedData.filter(reserve => reserve[4] === 'Parque1').length;
            const parque2Count = cleanedData.filter(reserve => reserve[4] === 'Parque2').length;
            
            document.getElementById('totalReserves').textContent = cleanedData.length;
            document.getElementById('totalP1').textContent = parque1Count;
            document.getElementById('totalP2').textContent = parque2Count;

            populateReservesTable(cleanedData); // Chama a função para popular a tabela
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); });
}


function populateReservesTable(data) {
    const tableBody = document.getElementById('tableReserves');
    tableBody.innerHTML = ''; // Limpa o conteúdo anterior da tabela

    data.forEach(row => {
        // Criar uma nova linha de tabela
        const tr = document.createElement('tr');

        // Adicionar checkbox
        const checkboxTd = document.createElement('td');
        const checkbox = document.createElement('input');
        checkbox.setAttribute('type', 'checkbox');
        checkbox.classList.add('form-check-input');
        checkboxTd.appendChild(checkbox);
        tr.appendChild(checkboxTd);

        // Preencher as colunas com os dados
        ['user', 'marca', 'matricula', 'tempo', 'parque'].forEach((col, i) => {
            const td = document.createElement('td');
            td.textContent = row[i]; // Preencher as células com os valores corretos
            tr.appendChild(td);
        });

        // Adicionar a nova linha à tabela
        tableBody.appendChild(tr);
    });
}

function populateUsersTable(data) {
    const tableBody = document.getElementById('tableUsers');
    tableBody.innerHTML = ''; // Limpa o conteúdo anterior

    data.forEach(row => {
        // Criar uma nova linha de tabela
        const tr = document.createElement('tr');

        // Preencher as colunas com os dados
        ['user', 'matricula', 'marca'].forEach((col, i) => {
            const td = document.createElement('td');
            td.textContent = row[i]; // Preencher as células com os valores corretos
            tr.appendChild(td);
        });

        // Adicionar a nova linha à tabela
        tableBody.appendChild(tr);
    });
}