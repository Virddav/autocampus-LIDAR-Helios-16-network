import matplotlib.pyplot as plt

def extract_px_values(log_file_path):
    px_values = []
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
                px_values.append([timestamp,first_value,second_value])
    return px_values


def extract_information(log_file_path, packets_size):
    px_values_lists = extract_px_values(log_file_path)
    last_message = max(px_values_lists, key=lambda x: x[2]) if px_values_lists else None
    first_message = min(px_values_lists, key=lambda x: x[2]) if px_values_lists else None

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

def info_print(log_file_path, packets_size):
    time, debit, rate= extract_information(log_file_path, packets_size)
    print(time)
    print(debit)
    print(rate)

def extracts_all(path, n, packets_size):
    info_array = []
    for i in range(n):
        log_file_path = path + '/stderr'+ str(i)
        #info_print(log_file_path)
        time, debit, rate= extract_information(log_file_path, packets_size)
        info_array.append([time,debit,rate])
    return info_array


def create_plot(debit, intervale, color):

    # Création du plot

    plt.plot(intervale, debit, marker='o', linestyle='-', color=color, linewidth=2, markersize=8)

    # Ajout de titres et de labels
    plt.xlabel('Intervale (ms)')
    plt.ylabel('Debit (kb/s)')

    # Affichage du plot
    plt.tight_layout()

debit_droid3_1048 = [sublist[1] for sublist in extracts_all('../stderr/droid3_1048', 7, 1048)]
debit_droid4_1048 = [sublist[1] for sublist in extracts_all('../stderr/droid4_1048', 7, 1048)]

intervale = [1, 10, 50, 100, 200, 500, 1000]
plt.figure(figsize=(10, 6))
create_plot(debit_droid3_1048,intervale, 'red')
create_plot(debit_droid4_1048,intervale, 'blue')


debit_droid3_2096 = [sublist[1] for sublist in extracts_all('../stderr/droid3_2096', 6, 2096)]
debit_droid4_2096 = [sublist[1] for sublist in extracts_all('../stderr/droid4_2096', 6, 2096)]

intervale = [1, 10, 50, 200, 500, 1000]
plt.figure(figsize=(10, 6))
create_plot(debit_droid3_2096,intervale, 'red')
plt.title("Comparaison des débits en fonction de l'intervalles d'envoi avec des packets de 1048")
create_plot(debit_droid4_2096,intervale, 'blue')
plt.title("Comparaison des débits en fonction de l'intervalles d'envoi avec des packets de 2096")

plt.show()