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

    lost_rate = 1-(last_message[1]/last_message[2])

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
        #info_print(log_file_path)
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

# Fonction à modifier pour integrer de nouveaux fichiers stderr 
def main():
    intervale = [1, 10, 50, 200, 500, 1000]

    # DEBIT 
    plt.figure(figsize=(20, 10))
    plt.title("Débit en fonction de l'intervale d'envoi des messages et suivant la taille des packets")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Débit (kb/s)')
    plt.tight_layout()

    create_plot(intervale, 1048, 'cv2x',1, 'red')
    create_plot(intervale, 2096, 'cv2x',1, 'green')
    create_plot(intervale, 4192, 'cv2x',1, 'blue')
    plt.legend()

    # TAUX D'ERROR
    plt.figure(figsize=(20, 10))
    plt.title("Taux d'erreur en fonction de l'intervale d'envoi des messages et suivant la taille des packets")
    plt.xlabel('Intervale (ms)')
    plt.ylabel('debit')
    plt.tight_layout()

    create_plot(intervale, 1048, 'cv2x',2, 'red')
    create_plot(intervale, 2096, 'cv2x',2, 'green')
    create_plot(intervale, 4192, 'cv2x',2, 'blue')
    plt.legend()

    plt.show()

    # DSRC
    #intervale_1048_dsrc = [50, 100, 200, 500, 1000]
    #create_plot(intervale_1048_dsrc, 1048, 'dsrc',1, 'blue')
    #create_plot(intervale_1048_dsrc, 1048, 'dsrc',2)



if __name__ == "__main__":
    main()