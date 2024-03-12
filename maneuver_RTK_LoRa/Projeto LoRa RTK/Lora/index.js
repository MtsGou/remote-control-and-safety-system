let payload = [{ applicationID: '1',
  applicationName: 'GPS1',
  devEUI: 'ac1f09fffe067188',
  deviceName: 'GPS1',
  timestamp: 1664472504,
  fCnt: 223,
  fPort: 2,
  data: 'MSwtMjEuODM5MDQzLC00My4zNzY1NTMsOCwxMTQ=',
  data_encode: 'base64',
  adr: false,
  rxInfo:
   [ { gatewayID: 'ac1f09fffe06fccf',
       loRaSNR: 12.8,
       rssi: -32,
       location: [Object],
       time: '2022-09-29T17:28:24.583735Z' } ],
  txInfo: { frequency: 916200000, dr: 2 } },
  { 
      applicationID: '1',
  applicationName: 'GPS1',
  devEUI: 'ac1f09fffe067188',
  deviceName: 'GPS1',
  timestamp: 1664472504,
  fCnt: 223,
  fPort: 2,
  data: 'MCwtMjkuODM5MDQzLC0zMy4zNzY1NTMsMjMsMjk0',
  data_encode: 'base64',
  adr: false,
  rxInfo:
   [ { gatewayID: 'ac1f09fffe06fccf',
       loRaSNR: 12.8,
       rssi: -32,
       location: [Object],
       time: '2022-09-29T17:28:24.583735Z' } ],
  txInfo: { frequency: 916200000, dr: 2 } }];

const ignore_vars = ["metadata"];

function toTagoFormat(object_item, /*serie,*/ prefix = '') {
  const result = [];
  for (const key in object_item) {
    if (ignore_vars.includes(key)) continue;

    if (key == 'data') {
      const buffer = Buffer.from(object_item['data'], 'base64').toString('utf-8');
      const status_GPS = buffer.split(",")[0];
      const lat = buffer.split(",")[1];
      const lng = buffer.split(",")[2];
      const num_GPS = buffer.split(",")[3];
      const accuracy = buffer.split(",")[4];
      result.push(
        {
            variable: "location", //object_item[key].variable || `${prefix}${key}`,
            value: "My address",
            //metadata: object_item[key].metadata,
            location: {
                "lat": parseFloat(lat, 6),
                "lng": parseFloat(lng, 6)
            },
            //unit: object_item[key].unit
        },

        {
            variable: "status",
            value: parseInt(status_GPS,10)
        },
        
        {
            variable: "gps",
            value: parseInt(num_GPS,10)
        },

        {
            variable: "precision",
            value: parseInt(accuracy,10)
        });
    }
  }

  return result;
}

//payload = toTagoFormat(payload[0]);
if (!payload[0].variable) {
    const serie = payload[0].serie || new Date().getTime();

    payload = toTagoFormat(payload[0], serie);
}


