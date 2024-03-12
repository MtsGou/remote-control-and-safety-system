'''
Criado por Matheus Gouveia,
    06/01/2023
    
RESUMO:

Pega dados do GPS por USB (formato csv)
Usuario seleciona quantos dados por vez armazenar
Adiciona linha por linha dados brutos GPS e dados GGA em csv ate o usuario parar a aplicacao
Chama a funcao para entregar o dataframe tratado e completo para plot
Salva dados tratados RTK em um arquivo CSV
'''

import csv
import serial
import time
from tratardadosRTK import get_snapshot_data

COM_PORT = str(input("A qual porta COM o dispositivo esta conectado? Digite no formato \'COMX\' \r\n"))
print('Tentando conectar na porta ' + COM_PORT + '...\r\n')

time.sleep(1)

try:
    device = serial.Serial(
        port=COM_PORT,
        baudrate=9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )
except:
    print('Nao foi possivel conectar ao dispositivo\r\n')
    exit()
else:
    print("Conectado com sucesso\r\n")
    
time.sleep(0.5)

with open('dadosGGAGPS.csv', 'w', newline='') as csv_file:
    writer = csv.writer(csv_file)
    writer.writerow(['Msg','time','lat',
                    'NS','lon','EW','quality',
                    'numSV','HDOP','alt','altUnit',
                    'sep','sepUnit',
                    'diffAge','diffStation/checksum'])

with open('dadosbrutosGPS.txt', 'w') as txt_file:
    txt_file.write("")

data_load = int(input("Digite um valor para a carga de dados (padrao 5 linhas por envio): \r\n"))
size = 0

print('Dados sendo coletados... acesse os arquivos .csv criados\r\n')

while(1):
    time.sleep(0.1)
    
    # read line from device
    data_device = str(device.readline().decode("utf-8")).replace('\r\n', "")
    newrow = data_device.split(",")
    
    with open('dadosbrutosGPS.txt', 'a') as txt_file:
        txt_file.write(data_device)
        txt_file.write('\n')
    
    if (newrow[0].find("$GNGGA") != -1 or newrow[0].find("$GPGGA") != -1):
        with open('dadosGGAGPS.csv', 'a', newline='') as csv_file:
            writer = csv.writer(csv_file)
            writer.writerow(newrow)
            size += 1
        
    if (size >= data_load):
        get_snapshot_data()
        size = 0

    