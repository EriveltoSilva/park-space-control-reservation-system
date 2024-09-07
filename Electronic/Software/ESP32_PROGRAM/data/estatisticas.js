window.addEventListener('load', () => { window.localStorage.clear();setInterval(receiveReservesData, 5000);setInterval(receiveUsersData, 5000);});

function receiveReservesData() {
    fetch('/reserves-estatisticas')
        .then(resp => resp.json())
        .then((data) => {
            const cleanedData = Object.values(data).map(item => {
                const cleanString = item.replace(/"/g, ''); // Remove aspas
                return cleanString.split('|'); // Divide os dados em array
            });
            
            populateReserveTable(cleanedData); // Chama a função para popular a tabela
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); });
}

function receiveUsersData() {
    fetch('/users-estatisticas')
        .then(resp => resp.json())
        .then((data) => {
            // Limpar os dados e remover aspas desnecessárias
            const cleanedData = Object.values(data).map(item => {
                const cleanString = item.replace(/"/g, ''); // Remove aspas
                return cleanString.split('|'); // Divide os dados em array
            });

            populateUsersTable(cleanedData); // Chama a função para popular a tabela
        })
        .catch(error => { console.error("## ERRO PEGANDO OS DADOS:" + error); });
}




function populateReserveTable(data) {
    const tableBody = document.getElementById('tableReserves');
    tableBody.innerHTML = ''; // Limpa o conteúdo anterior

    data.forEach((row, index) => {
        // Criar uma nova linha de tabela
        const tr = document.createElement('tr');

        // Checkbox
        const tdCheckbox = document.createElement('td');
        const checkbox = document.createElement('input');
        checkbox.setAttribute('type', 'checkbox');
        checkbox.classList.add('form-check-input');
        tdCheckbox.appendChild(checkbox);
        tr.appendChild(tdCheckbox);

        // Preencher as colunas com os dados
        ['user', 'marca', 'matricula', 'tempo'].forEach((col, i) => {
            const td = document.createElement('td');
            td.textContent = row[i];
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