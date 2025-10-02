#!/usr/bin/env python3
import socket
import sys
import threading
import time

def read_from_server(sock):
    """Lê mensagens do servidor e as imprime"""
    try:
        while True:
            data = sock.recv(1024)
            if not data:
                break
            print(f"<< {data.decode('utf-8', errors='ignore').strip()}")
    except Exception as e:
        print(f"Erro ao ler do servidor: {e}")

def main():
    if len(sys.argv) != 4:
        print("Uso: python3 irc_client.py <host> <port> <nickname>")
        sys.exit(1)
    
    host = sys.argv[1]
    port = int(sys.argv[2])
    nickname = sys.argv[3]
    
    try:
        # Conectar ao servidor
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        print(f"Conectado ao servidor IRC em {host}:{port}")
        
        # Thread para ler mensagens do servidor
        read_thread = threading.Thread(target=read_from_server, args=(sock,))
        read_thread.daemon = True
        read_thread.start()
        
        # Autenticação básica IRC
        sock.send(f"PASS teste\r\n".encode())
        sock.send(f"NICK {nickname}\r\n".encode())
        sock.send(f"USER {nickname} 0 * :{nickname}\r\n".encode())
        
        print(f"Enviado: PASS teste")
        print(f"Enviado: NICK {nickname}")
        print(f"Enviado: USER {nickname} 0 * :{nickname}")
        
        # Loop de entrada do usuário
        print("\nComandos disponíveis:")
        print("  JOIN #canal [senha]")
        print("  MODE #canal +k senha")
        print("  OPER nickname senha")
        print("  PRIVMSG #canal :mensagem")
        print("  QUIT")
        print("")
        
        while True:
            try:
                user_input = input("IRC> ")
                if user_input.upper() == 'QUIT':
                    sock.send("QUIT :Saindo\r\n".encode())
                    break
                elif user_input:
                    sock.send(f"{user_input}\r\n".encode())
                    print(f">> {user_input}")
            except KeyboardInterrupt:
                print("\nSaindo...")
                sock.send("QUIT :Ctrl+C\r\n".encode())
                break
                
    except Exception as e:
        print(f"Erro: {e}")
    finally:
        sock.close()

if __name__ == "__main__":
    main()