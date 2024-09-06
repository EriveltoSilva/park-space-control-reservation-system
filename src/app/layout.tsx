import type { Metadata } from "next";
import localFont from "next/font/local";
import "./globals.css";

const geistSans = localFont({
  src: "./fonts/GeistVF.woff",
  variable: "--font-geist-sans",
  weight: "100 900",
});
const geistMono = localFont({
  src: "./fonts/GeistMonoVF.woff",
  variable: "--font-geist-mono",
  weight: "100 900",
});

export const metadata: Metadata = {
  title: "Spark | ",
  description: "Sistema de reserva e controle de vaga de parqueamento",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="pt-ao">
      <body
        className={`${geistSans.variable} ${geistMono.variable}bg-zinc-950 text-zinc-5 antialiased`}
      >
        {children}
      </body>
    </html>
  );
}
