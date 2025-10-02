#!/usr/bin/env python3
import socket
import time

def test_admin_features():
    """Testa as funcionalidades de admin do servidor IRC"""
    
    print("=== Testando Sistema de Admin IRC ===\n")
    
    try:
        # Conectar ao servidor
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('localhost', 6667))
        print("✓ Conectado ao servidor IRC")
        
        def send_cmd(cmd):
            sock.send(f"{cmd}\r\n".encode())
            print(f">> {cmd}")
            time.sleep(0.1)
            
        def read_response():
            try:
                data = sock.recv(1024)
                if data:
                    response = data.decode('utf-8', errors='ignore').strip()
                    print(f"<< {response}")
                    return response
            except:
                pass
            return ""
        
        # 1. Autenticação como admin
        print("\n1. Autenticando como admin (luis-fif)...")
        send_cmd("PASS teste")
        send_cmd("NICK luis-fif")
        send_cmd("USER luis-fif 0 * :Admin User")
        
        time.sleep(0.5)
        read_response()
        
        # 2. Testar comando OPER
        print("\n2. Testando comando OPER...")
        send_cmd("OPER luis-fif mypassword123")
        
        time.sleep(0.5)
        read_response()
        
        # 3. Testar JOIN em canal auto-op
        print("\n3. Testando JOIN em canal auto-op (#test)...")
        send_cmd("JOIN #test")
        
        time.sleep(0.5)
        read_response()
        
        # 4. Testar definição de mode +k no canal
        print("\n4. Testando MODE +k (definir senha do canal)...")
        send_cmd("MODE #test +k secretpass")
        
        time.sleep(0.5)
        read_response()
        
        # 5. Testar INVITE
        print("\n5. Testando comando INVITE...")
        send_cmd("INVITE testuser #test")
        
        time.sleep(0.5)
        read_response()
        
        print("\n=== Teste Concluído ===")
        
        # Desconectar
        send_cmd("QUIT :Teste finalizado")
        
    except Exception as e:
        print(f"Erro: {e}")
    finally:
        sock.close()

if __name__ == "__main__":
    test_admin_features()