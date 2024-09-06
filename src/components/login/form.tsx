
"use client";

import { login, onAuthChange } from "@/services/firebase/authServices";
import { User as authUser } from "firebase/auth";
import { Lock, User } from "lucide-react";
import Link from "next/link";
import { FormEvent, useEffect, useState } from "react";
import { Button } from "../ui/button";

interface LoginFormProps {

}



export const LoginForm = ({ }: LoginFormProps) => {
    const [user, setUser] = useState<authUser | null>(null);

    const isCredencialValid = (email: string, password: string): string[] => {
        const errors = [];
        if (email === "")
            errors.push("E-mail é obrigatório");
        if (!/^\S+@\S+\.\S+$/.test(email))
            errors.push("E-mail inválido");
        if (password === "")
            errors.push("Senha é obrigatória");
        if (password.length < 8)
            errors.push("Senha precisa ter pelo menos 8 caracteres");
        return errors;
    }

    const handleLoginFormSubmit = (event: FormEvent<HTMLFormElement>) => {
        event.preventDefault();
        const data = new FormData(event.currentTarget);
        const email = data.get("email") as string;
        const password = data.get("password") as string;

        const errors = isCredencialValid(email, password);

        if (errors.length > 0) {
            console.error("Erros:", errors);
            return;
        }

        login(email, password).then((user) => console.log(user)).catch((error) => console.error(error));
        console.log("Email:", email);
        console.log("Password:", password);
    }

    useEffect(() => {
        const unsubscribe = () => {
            return onAuthChange((user) => {
                setUser(user);
            })
        }
        return unsubscribe();
    }, []);

    return (
        <form onSubmit={handleLoginFormSubmit} className='lg:space-y-24 space-y-14'>
            <div className="space-y-8">
                <div className="h-14 px-5 bg-zinc-950 rounded-lg border border-zinc-800 flex items-center gap-2">
                    <User className='text-zinc-400 size-5' />
                    <input type="email" name="email" placeholder='Seu e-mail' className='placeholder:text-zinc-400 bg-transparent outline-none flex-1' />
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
    )
}
