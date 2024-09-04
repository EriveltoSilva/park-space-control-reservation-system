const email = document.getElementById('email');
const password = document.getElementById('password');
const button = document.getElementById('btn');

button.addEventListener("click", (e)=>{
    e.preventDefault();
    let result = validateForm();
    if(result===null)
        sendData();
    else
        alert('Login!\n'+result);
        // Swal.fire('Login!',result,'error');
});

function validateForm(){
    let TAMANHO_MINIMO_SENHA = 4;
    let regex = /\S+@\S+\.\S+/;
    if(email.value==="")
        return "Campo Email Vazio!";
    else if(!regex.test(email.value))
        return "O email não está no formato: name@example.com! Retifique por favor.";
    else if(password.value==="")
        return "Campo Senha Vazio!";
    else if(password.value.length < TAMANHO_MINIMO_SENHA)
        return "A senha deve conter no mínimo "+TAMANHO_MINIMO_SENHA+" caracteres!";
    return null;
}

function sendData()
{
    fetch('/login', {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({email:email.value, password:password.value}) 
    })
    .then(resp => resp.json())
    .then(data=>{
        if(data.status=='success')        
            window.location.assign("/main/"+data.data.id);
        Swal.fire('Ups!',data.message,data.status)
        clean();
    }).catch(exception=>{
        console.log(exception);
    }); 
}

function clean() {
    password.value = "";
}