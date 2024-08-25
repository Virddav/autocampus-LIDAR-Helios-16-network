import os

intervals = [1, 10, 50, 100, 200, 500, 1000]
packet_sizes = [1048, 2096, 4192]
tx_modes = ['dsrc', 'cv2x']

for tx_mode in tx_modes:
    for packet_size in packet_sizes:
        for droid in [3,4]:
            for interval in intervals:
                file_name = f'droid{droid}_{packet_size}_{tx_mode}/{interval}'
                os.system('echo ' + file_name)
                os.system(f'cat {file_name} | grep AOC > droid{droid}_{packet_size}_{tx_mode}/small_{interval}')

