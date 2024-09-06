import { LoginForm } from "@/components/login/form";

export default async function Home() {

    return (
        <div className="w-full h-screen flex justify-center items-center ">
            <div className='lg:w-[640px] w-4/5 h-[600px] rounded-xl px-6 py-10 bg-zinc-900 shadow-shape space-y-16'>
                <div className='lg:space-y-6 space-y-2'>
                    <div className='flex justify-center items-center'>
                        <h2 className='text-white text-center text-lg lg:text-3xl font-semibold'>Login</h2>
                    </div>
                    <p className='text-zinc-400 text-sm lg:text-base '>
                        Para ter acesso ao maior e melhor sistema de controle de reservas de parqueamento nacional faça o seu login
                    </p>
                </div>

                <LoginForm />
            </div>
        </div>
    );
}
