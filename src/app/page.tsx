import { Button } from "@/components/ui/button";
import { Lock, User } from "lucide-react";
import Link from "next/link";

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

                <form className='lg:space-y-24 space-y-14'>
                    <div className="space-y-8">
                        <div className="h-14 px-5 bg-zinc-950 rounded-lg border border-zinc-800 flex items-center gap-2">
                            <User className='text-zinc-400 size-5' />
                            <input type="text" name="name" placeholder='Seu nome completo' className='placeholder:text-zinc-400 bg-transparent outline-none flex-1' />
                        </div>

                        <div className="space-y-5">

                            <div className="h-14 px-5 bg-zinc-950 rounded-lg border border-zinc-800 flex items-center gap-2">
                                <Lock className='text-zinc-400 size-5' />
                                <input type="password" name="password" placeholder='Sua password' className='placeholder:text-zinc-400 bg-transparent outline-none flex-1' />
                            </div>

                            <p className="text-end text-xs lg:text-base">
                                Ainda não tenho uma conta? <Link href={"/register"} className="text-lime-300 underline">Criar uma conta agora</Link>
                            </p>
                        </div>
                    </div>

                    <Button type="submit" className="w-full" >
                        Entrar
                    </Button>
                </form>
            </div>
        </div>
    );
}
