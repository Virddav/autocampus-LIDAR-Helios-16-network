import matplotlib.pyplot as plt

# Recupere les info des logs
def extract_values(log_file_path):
    values = []
    with open(log_file_path, 'r') as file:
        for line in file:
            if "[AOC] Rx statistics" in line:
                # Extract timestamp
                timestamp = line[:24] 
                # Find the part of the line with COUNT
                start_index = line.index("(COUNT) ") + len("(COUNT) ")
                px_string = line[start_index:].strip().split('\t')[0]  # Assuming no other characters after "34:120"
                # Split the "34:120" into two numbers
                first_value, second_value = map(int, px_string.split('/'))
                # Store the extracted values in the dictionary
                values.append([timestamp,first_value,second_value])
    return values

# Calcul le debit et le taux de taux d'erreur
def calcul_information(log_file_path, packets_size):
    values_lists = extract_values(log_file_path)
    last_message = max(values_lists, key=lambda x: x[2]) if values_lists else None
    first_message = min(values_lists, key=lambda x: x[2]) if values_lists else None

    #print(" First message = " + str(first_message))
    #print(" Last message = " + str(last_message))

    start = (float(last_message[0][10:12])*60) + float(last_message[0][13:21])
    end = (float(first_message[0][10:12])*60) + float(first_message[0][13:21])
    total_time = start - end
    #print(" Temps d'execution = " + str(total_time))

    debit = ((float(last_message[1])*packets_size)/total_time)/1000
    #print(" Debit = "+ str(debit))

    lost_rate = 1-((last_message[1]+first_message[2]-1)/last_message[2])

    return total_time, debit, lost_rate

# Affiche des infos pour debug
def info_print(log_file_path, packets_size):
    time, debit, rate= calcul_information(log_file_path, packets_size)
    print(time)
    print(debit)
    print(rate)

# Calcul les infos pour tous les intervales
def extracts_all(path, intervale, packets_size):
    info_array = []
    for i in range(len(intervale)):
        log_file_path = path + '/stderr'+ str(intervale[i])
        #info_print(log_file_path, packets_size)
        time, debit, rate= calcul_information(log_file_path, packets_size)
        info_array.append([time,debit,rate*100])
    return info_array

# Lance la création du plot des logs choisis 
def create_plot(intervale, packets_size, protocol, info, color):
    data_droid3 = [sublist[info] for sublist in extracts_all('../stderr/droid3_'+str(packets_size)+'_'+protocol, intervale, packets_size)]
    data_droid4 = [sublist[info] for sublist in extracts_all('../stderr/droid4_'+str(packets_size)+'_'+protocol, intervale, packets_size)]
    data =[]

    for a, b in zip(data_droid3,data_droid4):
        data.append((a+b)/2)

    plt.plot(intervale, data, marker='o', linestyle='-', color=color, label=str(packets_size), linewidth=2, markersize=8)

def create_bar(intervale, packets_size, protocol, info, color, bar_number):
    data_droid3 = [sublist[info] for sublist in extracts_all('../stderr/droid3_'+str(packets_size)+'_'+protocol, intervale, packets_size)]
    data_droid4 = [sublist[info] for sublist in extracts_all('../stderr/droid4_'+str(packets_size)+'_'+protocol, intervale, packets_size)]
    data =[]

    for a, b in zip(data_droid3,data_droid4):
        data.append((a+b)/2)

    bar_size = 3
    intervale_espaced = [x + bar_size*bar_number for x in intervale]

    plt.bar(intervale_espaced, data, color=color, label=str(packets_size), width=bar_size, edgecolor='black')

# Fonction à modifier pour integrer de nouveaux fichiers stderr 
def main():
    
    # --------------- PROTOCOL DSRC ---------------

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Débit obtenu en utilisant le protocole DSRC")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    intervale_1048_dsrc = [50, 100, 200, 500, 1000]
    create_plot(intervale_1048_dsrc, 1048, 'dsrc',1, 'blue')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur observé en utilisant DSRC")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()
    create_plot(intervale_1048_dsrc, 1048, 'dsrc',2, 'red')
    plt.legend()

    # --------------- PROTOCOL CV2X ---------------
    intervale = [1, 10, 50, 100, 200, 500, 1000]


    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit obtenu en utilisant le protocole CV2X")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    create_plot(intervale, 1048, 'cv2x',1, 'red')
    create_plot(intervale, 2096, 'cv2x',1, 'green')
    create_plot(intervale, 4192, 'cv2x',1, 'blue')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur observé en utilisant le protocole CV2X")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    create_plot(intervale, 1048, 'cv2x',2, 'red')
    create_plot(intervale, 2096, 'cv2x',2, 'green')
    create_plot(intervale, 4192, 'cv2x',2, 'blue')
    plt.legend()
    
    # --------------- BAR VERSION ---------------

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit obtenu en utilisant le protocole CV2X")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    create_bar(intervale, 1048, 'cv2x',1, 'red', 0)
    create_bar(intervale, 2096, 'cv2x',1, 'green', 1)
    create_bar(intervale, 4192, 'cv2x',1, 'blue', 2)
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur observé en utilisant le protocole CV2X")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    create_bar(intervale, 1048, 'cv2x',2, 'red', 0)
    create_bar(intervale, 2096, 'cv2x',2, 'green', 1)
    create_bar(intervale, 4192, 'cv2x',2, 'blue', 2)
    plt.legend()

    # --------------- DROID DISTANT ---------------

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit de deux OBU positionnés à une distance de 100 mètres")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    data = [sublist[1] for sublist in extracts_all('../stderr/droid_distant', [10,100], 4192)]
    plt.bar([10,100], data, color='blue', label=str(4192), width=6, edgecolor='black')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur de deux OBU positionnés à une distance de 100 mètres")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    data = [sublist[2] for sublist in extracts_all('../stderr/droid_distant', [10,100], 4192)]
    plt.bar([10,100], data, color='blue', label=str(4192), width=6, edgecolor='black')
    plt.legend()

    # --------------- DROID EN MOUVEMENT --------------- 

    bar_size = 3
    intervale0 = [10, 100]
    intervale1 = [x + bar_size for x in intervale0]

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit de deux OBU dont l'un est en mouvement et l'autre fixe avec une distance maximale de 100 mètres")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Debit (kb/s)')
    plt.tight_layout()

    data = [sublist[1] for sublist in extracts_all('../stderr/droid_moving_obu', [10,100], 4192)]
    plt.bar(intervale0, data, color='blue', label="OBU", width=3, edgecolor='black')
    data = [sublist[1] for sublist in extracts_all('../stderr/droid_moving_rsu', [10,100], 4192)]
    plt.bar(intervale1, data, color='red', label="RSU", width=3, edgecolor='black')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur de deux OBU dont l'un est en mouvement et l'autre fixe avec une distance maximale de 100 mètres")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    data = [sublist[2] for sublist in extracts_all('../stderr/droid_moving_obu', [10,100], 4192)]
    plt.bar(intervale0, data, color='blue', label="OBU", width=3, edgecolor='black')
    data = [sublist[2] for sublist in extracts_all('../stderr/droid_moving_rsu', [10,100], 4192)]
    plt.bar(intervale1, data, color='red', label="RSU", width=3, edgecolor='black')
    plt.legend()

    # --------------- OPTIMISATION DE L'INTERVALE ---------------

    bar_size = 0.35
    intervale0 = [5,8,10]
    intervale1 = [x + bar_size for x in intervale0]

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Recherche du débit optimal en protocole CV2X")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Debit (kb/s)')
    plt.tight_layout()

    data = [sublist[1] for sublist in extracts_all('../stderr/opti/OBU3', [5,8,10], 4192)]
    plt.bar(intervale0, data, color='red', label="OBU3", width=bar_size, edgecolor='black')
    data = [sublist[1] for sublist in extracts_all('../stderr/opti/OBU4', [5,8,10], 4192)]
    plt.bar(intervale1, data, color='blue', label="OBU4", width=bar_size, edgecolor='black')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Observation des taux d'erreurs sur les débits optimaux")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    data = [sublist[2] for sublist in extracts_all('../stderr/opti/OBU3', [5,8,10], 4192)]
    plt.bar(intervale0, data, color='red', label="OBU3", width=bar_size, edgecolor='black')
    data = [sublist[2] for sublist in extracts_all('../stderr/opti/OBU4', [5,8,10], 4192)]
    plt.bar(intervale1, data, color='blue', label="OBU4", width=bar_size, edgecolor='black')
    plt.legend()

    # --------------- DROID MULTIPLES ---------------
    # Impossible de faire automatiquement parce qu'il n'y a pas de difference entre les messages reçus de OBU3 et OBU4 et RSU 
    
    data_debit = [[0,0],[0,0],[0,0]]
    data_tx = [[0,0],[0,0],[0,0]]
    time, data_debit[0][1], rate = calcul_information("../stderr/droid_multiple/stderr100_RSU", 4192)
    data_tx[0][0] = 1-(11701/(330+11512))
    time, data_debit[1][1], rate = calcul_information("../stderr/droid_multiple/stderr100_OBU4", 4192)
    data_tx[1][0] = 1-(10650/(52+10608))
    time, data_debit[2][1], rate = calcul_information("../stderr/droid_multiple/stderr100_OBU3", 4192)
    data_tx[2][0] = 1-(12788/(904+11895))

    time, data_debit[1][0], rate = calcul_information("../stderr/droid_multiple/stderr10_RSU", 4192)
    data_tx[0][1] = 1-(18944/(3982+15163+1000))
    time, data_debit[1][0], rate = calcul_information("../stderr/droid_multiple/stderr10_OBU4", 4192)
    data_tx[1][1] = 1-(21400/(4275+17154+1000))
    time, data_debit[2][0], rate = calcul_information("../stderr/droid_multiple/stderr10_OBU3", 4192)
    data_tx[2][1] = 1-(18120/(3575+14566+1000))

    bar_size = 1
    intervale0 = [10,100]
    intervale1 = [x + bar_size for x in intervale0]
    intervale2 = [x - bar_size for x in intervale0]

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit obtenu en utilisant trois OBU")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    plt.bar(intervale0, data_debit[1], color='blue', label="RSU", width=bar_size, edgecolor='black')
    plt.bar(intervale1, data_debit[1], color='red', label="OBU4", width=bar_size, edgecolor='black')
    plt.bar(intervale2, data_debit[2], color='green', label="OBU3", width=bar_size, edgecolor='black')

    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur observé en utilisant trois OBU")
    plt.xlabel('Intervale (ms)')
    plt.ylabel("Taux d'erreur")
    plt.tight_layout()

    plt.bar(intervale0, data_tx[0], color='blue', label="RSU", width=bar_size, edgecolor='black')
    plt.bar(intervale1, data_tx[1], color='red', label="OBU4", width=bar_size, edgecolor='black')
    plt.bar(intervale2, data_tx[2], color='green', label="OBU3", width=bar_size, edgecolor='black')

    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main()

