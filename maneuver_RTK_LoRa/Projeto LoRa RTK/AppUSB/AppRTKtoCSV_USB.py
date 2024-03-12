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
import pandas as pd

def get_snapshot_data():
    # Importar todos os dados (CSV)
    data_GGA = pd.read_csv('dadosGGAGPS.csv')

    # Nomear colunas
    # data_GGA = data_GGA[data_GGA.iloc[:,0] == '$GNGGA']
    data_GGA.columns = ['Msg','time','lat',
                    'NS','lon','EW','quality',
                    'numSV','HDOP','alt','altUnit',
                    'sep','sepUnit',
                    'diffAge','diffStation/checksum']
    
    # Multiplicador para eliminar N,S,W,E
    data_GGA.loc[data_GGA['NS'] == 'N', 'multiplierNS'] = 1
    data_GGA.loc[data_GGA['NS'] == 'S', 'multiplierNS'] = -1
    data_GGA.loc[data_GGA['EW'] == 'E', 'multiplierWE'] = 1
    data_GGA.loc[data_GGA['EW'] == 'W', 'multiplierWE'] = -1

    # Latitude e Longitude ajustadas para formato decimal
    lat = data_GGA['lat']
    lon = data_GGA['lon']
    lat = (lat - lat%100)/100 + (lat%100)/60
    lon = (lon - lon%100)/100 + (lon%100)/60
    lat = lat*data_GGA['multiplierNS']
    lon = lon*data_GGA['multiplierWE']
    
    # Data frame de coordenadas ajustadas
    coordenadas_GPS = pd.concat([lat, lon], axis=1, join='inner')
    coordenadas_GPS.columns = ['Lat', 'Lon']
    
    # Verificar se ha RTK (4 ou 5)
    data_GGA.loc[(data_GGA['quality'] == 4) | (data_GGA['quality'] == 5), 'RTK status'] = 'S'
    data_GGA.loc[(data_GGA['quality'] != 4) & (data_GGA['quality'] != 5), 'RTK status'] = 'N'
    
    # Data frame completo para PLOTAR EM GRAFICO
    df_coord_RTK_status = coordenadas_GPS
    df_coord_RTK_status = pd.concat([df_coord_RTK_status, data_GGA['RTK status']], axis=1, join='inner')
    df_coord_RTK_status = df_coord_RTK_status.dropna()
    df_coord_RTK_status.columns = ['Lat', 'Lon', 'Status']
    
    # Apagar colunas criadas para tratamento dos dados
    data_GGA.drop('multiplierNS', inplace=True, axis=1)
    data_GGA.drop('multiplierWE', inplace=True, axis=1)
    data_GGA.drop('RTK status', inplace=True, axis=1)
    
    #print(data_GGA)
    print('\r')
    print(df_coord_RTK_status)
    
    # Salvar arquivo csv dos dados
    df_coord_RTK_status.to_csv('Dados_RTK.csv')

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

print('Dados sendo coletados... Aguarde. Acesse os arquivos .csv criados\r\n')

while(1):
    time.sleep(0.1)
    
    # read line from device
    try:
        data_device = device.readline().decode("utf-8").replace("\r\n", "")
        newrow = data_device.split(",")
    except:
        print("Houve um erro. Aguarde. Error. Wait for it.")
        continue
    
    with open('dadosbrutosGPS.txt', 'a') as txt_file:
        txt_file.write(data_device)
        txt_file.write('\n')
    
    if (newrow[0].find("GGA") != -1):
        with open('dadosGGAGPS.csv', 'a', newline='') as csv_file:
            writer = csv.writer(csv_file)
            writer.writerow(newrow)
            size += 1
        
    if (size >= data_load):
        get_snapshot_data()
        size = 0

    
