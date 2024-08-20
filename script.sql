CREATE TABLE Sensores (
    sensor_id SERIAL PRIMARY KEY,
    nombre_sensor TEXT NOT NULL,
    tipo_sensor TEXT NOT NULL,
    descripcion TEXT
);

CREATE TABLE DatosSensores (
    dato_id SERIAL PRIMARY KEY,
    sensor_id INT REFERENCES Sensores(sensor_id),
    timestamp TIMESTAMPTZ NOT NULL,
    valor TEXT NOT NULL
);

CREATE TABLE DatosGPS (
    gps_dato_id SERIAL PRIMARY KEY,
    sensor_id INT REFERENCES Sensores(sensor_id),
    timestamp TIMESTAMPTZ NOT NULL,
    latitud NUMERIC(10, 8) NOT NULL,
    longitud NUMERIC(11, 8) NOT NULL,
    altitud NUMERIC(10, 2),
    precision NUMERIC(5, 2)
);

CREATE TABLE DatosAcelerometro (
    acelerometro_dato_id SERIAL PRIMARY KEY,
    sensor_id INT REFERENCES Sensores(sensor_id),
    timestamp TIMESTAMPTZ NOT NULL,
    eje_x NUMERIC(10, 6) NOT NULL,
    eje_y NUMERIC(10, 6) NOT NULL,
    eje_z NUMERIC(10, 6) NOT NULL
);

INSERT INTO sensores (nombre_sensor, tipo_sensor, descripcion)
VALUES ('Acelerometro', 'Digital', 'Sensor Digital de aceleración de gravedad para Arduino, placa de inclinación para módulo IIC SPI, reemplazo ADXL335, GY-291, ADXL345, tres ejes triples');

INSERT INTO sensores (nombre_sensor, tipo_sensor, descripcion)
VALUES ('Temperatura Corporal', 'Digital', 'GY-906 MLX90614ESF, nuevo módulo de Sensor de temperatura sin contacto para Arduino, Compatible con MLX90614');

INSERT INTO sensores (nombre_sensor, tipo_sensor, descripcion)
VALUES ('Ritmo Cardiaco', 'Digital', 'Módulo de Sensor de ritmo cardíaco, detección Puls, prueba de concentración de oxígeno en sangre para Arduino, potencia ultrabaja, MH-ET LIVE MAX30102');

INSERT INTO sensores (nombre_sensor, tipo_sensor, descripcion)
VALUES ('Modulo GPS', 'Digital', 'Módulo GPS de GY-NEO6MV2 con Control de vuelo, controlador de vuelo GPS, NEO6MV2, EEPROM, MWC, APM2.5, 3V-5V, antena grande para Arduino');
