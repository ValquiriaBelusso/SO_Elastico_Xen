import subprocess
import json
import os
import time

LISTA_CPUS = []
LISTA_AUX = []
MAX_CPUS = 3

def alocandoCpu(n):
    # Escreve no xenstore para marcar a CPU como online
    print("ALOCANDO CPU", n)
    cmd = f"sudo xenstore-write -s /local/domain/1/cpu/{n}/availability online"
    print(cmd)
    os.system(cmd)
    
    LISTA_CPUS.append([])
    LISTA_AUX.clear()
       
    # Ativa a CPU 
    cmd2 = f"echo 1 > /sys/devices/system/cpu/cpu{n}/online"
    print(cmd2)
    os.system(cmd2)

def desalocandoCpu(n):
    # Escreve no xenstore para marcar a CPU como offline
    print("DESALOCANDO CPU", n)
    cmd = f"sudo xenstore-write -s /local/domain/1/cpu/{n}/availability offline"
    print(cmd)
    os.system(cmd)
    
    LISTA_CPUS.pop(n)
    if LISTA_AUX:
        LISTA_AUX[:] = [cpu for cpu in LISTA_AUX if cpu != n]

    #Desativa a CPU
    cmd2 = f"echo 0 > /sys/devices/system/cpu/cpu{n}/online"
    print(cmd2)
    os.system(cmd2)
   


def monitor_cpu_usage():
	
    LISTA_CPUS.append([])
    n = 0

    while True:
        try:        
            
            current_time = time.strftime('%H:%M:%S')
            print(current_time)
            
            x = (100 - (100 / os.cpu_count())) - (100 - (100 / os.cpu_count())) * 0.10
            print(x)
            
            a = subprocess.run(["mpstat", "-P", "ALL", "-o", "JSON", "1", "1"], capture_output=True).stdout
            json_obj = json.loads(a)
            cpu_no = json_obj.get('sysstat').get('hosts')[0].get('statistics')[0].get('cpu-load')[n+1].get('idle')
            cpu = json_obj.get('sysstat').get('hosts')[0].get('statistics')[0].get('cpu-load')[0].get('idle')            

            if(len(LISTA_CPUS[n]) < 5):
                LISTA_CPUS[n].append(cpu_no)
                LISTA_AUX.append(cpu)
            else:
                LISTA_CPUS[n].pop(0) 
                LISTA_AUX.pop(0)
                LISTA_CPUS[n].append(cpu_no) 
                LISTA_AUX.append(cpu)  
            if(len(LISTA_CPUS[n]) == 5):
                media = sum(LISTA_AUX) / 5
                for n, cpus in enumerate(LISTA_CPUS):
                    print ('CPU: ', n, 'MEDIA: ', media, ' - ', cpus)
                    print("\n")
                if media <= 15 and len(LISTA_CPUS) <= MAX_CPUS:
                    alocandoCpu(n+1)
                    n = n + 1
                elif (media >= 80 or media >= x) and n > 0:
                    desalocandoCpu(n)
                    n = n - 1
                           
        except IndexError:
            print("erro")
            break
       
        except KeyboardInterrupt:
            print("Programa interrompido pelo usuário.")
            break

        except Exception as e:
            print(f"Erro: {e}")
            break

monitor_cpu_usage()


