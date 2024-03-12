'''
Criado por Matheus Gouveia,
    13/03/2023

OBS: DEIXE O ARQUIVO TXT CONTENDO OS DADOS DO GPS NA MESMA PASTA DESSA APLICACAO
    
RESUMO:
Pega dados do GPS salvos em arquivo TXT na mesma pasta
Usuario seleciona quantos dados por vez armazenar
Adiciona linha por linha dados brutos GPS e dados GGA em csv ate o usuario parar a aplicacao
Chama a funcao para entregar o dataframe tratado e completo para plot
Salva dados tratados RTK em um arquivo CSV
'''

import csv
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
    df_coord_RTK_status = pd.concat([data_GGA["Msg"], coordenadas_GPS], axis=1, join='inner')
    df_coord_RTK_status = pd.concat([df_coord_RTK_status, data_GGA['RTK status']], axis=1, join='inner')
    df_coord_RTK_status = df_coord_RTK_status.dropna()
    df_coord_RTK_status.columns = ['Time','Lat', 'Lon', 'Status']
    df_coord_RTK_status['Time'] = df_coord_RTK_status['Time'].str.replace('  [$]\w*', '')
    df_coord_RTK_status.set_index('Time', inplace=True)
    
    # Apagar colunas criadas para tratamento dos dados
    data_GGA.drop('multiplierNS', inplace=True, axis=1)
    data_GGA.drop('multiplierWE', inplace=True, axis=1)
    data_GGA.drop('RTK status', inplace=True, axis=1)
    
    #print(data_GGA)
    #print('\r')
    #print(df_coord_RTK_status)
    
    # Salvar arquivo csv dos dados
    df_coord_RTK_status.to_csv('Dados_RTK.csv')


with open('dadosGGAGPS.csv', 'w') as csv_file:
    writer = csv.writer(csv_file)
    writer.writerow(['Msg','time','lat',
                    'NS','lon','EW','quality',
                    'numSV','HDOP','alt','altUnit',
                    'sep','sepUnit',
                    'diffAge','diffStation/checksum'])
    
lines = []
    
with open("Dados.txt") as file_in:
    for line in file_in:
        lines.append(line)

for line in lines:
    # read line from device
    newrow = line.replace('\n',"").split(",")
    
    if (newrow[0].find("GGA") != -1):
        with open('dadosGGAGPS.csv', 'a', newline= "") as csv_file:
            writer = csv.writer(csv_file)
            writer.writerow(newrow)

get_snapshot_data()
print('\nFim.')

    
