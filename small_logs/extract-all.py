import os

intervals = [1, 10, 50, 100, 200, 500, 1000]
packet_size = 1048
tx_mode = 'dsrc'

for droid in [3,4]:
    for interval in intervals:
        file_name = f'log_droid_{droid}_{interval}ms.tar.gz'
        os.system('tar -xf ' + file_name)
        os.system(f'cp 2024*/stderr droid{droid}_{packet_size}_{tx_mode}/{interval}')
        os.system(f'rm -rf 2024*/')

