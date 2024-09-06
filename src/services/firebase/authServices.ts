import { signInWithEmailAndPassword, signOut, onAuthStateChanged, UserCredential, User } from "firebase/auth";
import { auth } from "./firebaseServices";

export async function login(email: string, password: string): Promise<UserCredential> {
    return signInWithEmailAndPassword(auth,email, password);
}

export async function logout(): Promise<void> {
    return signOut(auth);
}

export function onAuthChange(callback: (user: User | null) => void): () => void {
    const unsubscribe = onAuthStateChanged(auth, callback);
    return unsubscribe;
}