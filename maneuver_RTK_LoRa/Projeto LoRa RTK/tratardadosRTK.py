'''
Criado por Matheus Gouveia,
    05/01/2023
    
RESUMO:

df_coord_RTK_status: Dataframe completo e tratado, para plotar (lat, lon, status)

data_GGA: Dados apenas do protocolo GGA, usado para avaliar status do RTK
lat: Dados de latitude somente dos dados GGA (formato decimal)
lon: Dados de longitude somente dos dados GGA (formato decimal)
coordenadas_GPS: Dados das coordenadas (lat e lon) decimal dos dados GGA
'''
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
    print('\r')
    print(df_coord_RTK_status)
    
    # Salvar arquivo csv dos dados
    df_coord_RTK_status.to_csv('Dados_RTK.csv')

