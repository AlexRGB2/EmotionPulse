# EmotionPulse
Documentación del proyecto EmotionPulse para la matería Desarrollo de Dispositivos Inteligentes.

---

## Integrantes
- Marisol Núñez Monasterio
- Juan Pablo Jimenez Jaime
- José Alexis Martínez Bárcenas

---

## Visión
Para aquellas personas deportistas quienes tienen la necesidad de realizar seguimiento de sus entrenamientos, asi como su estado de ánimo durante el mismo. EmotionPulse es una solución para esté tipo de necesidades y muchas más, ya que además de todo lo mencionado también se puede realizar un seguimiento preciosa de trayectos al momento de realizar running, también gracias a esto se puede medir el recorrido realizado. A diferencia de muchas otras smartbands, nuestro producto es más accesible no solo para deportistas, si no para cualquier persona que se interese en tener un mejor control sobre su salud física y emocional.

---

## Software empleado
|Nombre|Licencia|
|------|--------|
|Thonny|The MIT License (MIT)|
|Arduino IDE|AGPL-3.0 license|
|Fritzing|GNU GPL v3|
|Node-RED|Apache License 2.0|
|Visual Studio Code|Licencia MIT Binarios: Freeware|
|Tinkercad|Free for everyone|
|NEO-6M_GPS|The MIT License (MIT)|
|SparkFun MAX3010x|Creative Commons Share-alike 4.0 International|
|Visual Studio|Microsoft Software License Terms|
|Flutter|BSD 3-Clause License|
|fl_chart|MIT license|
|flutter_map|BSD-3-Clause license|
|latlong|Apache 2.0|
|mqtt_client|MIT License|
|supabase_flutter|Copyright (c) 2020 Supabase|

---

## Hardware empleado
|Nombre|Imagen|Descripción|Cantidad|
|------|------|-----------|--------|
|Módulo GPS de GY-NEO6MV2|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/9211ccad-cb2d-4c8f-ad27-316319f0804a)|El módulo controlador de drones GPS mide con precisión parámetros como la posición, la velocidad y el tiempo, lo que lo hace adecuado para aplicaciones que requieren un posicionamiento preciso, como drones, vehículos de control remoto y barcos.|1|
|Módulo de Sensor de temperatura sin contacto|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/d888d2fb-58a8-4ccc-9426-8aedebdfab77)|Termómetro infrarrojo sin contacto MELEXIS para usar con Arduino, o cualquier microcontrolador que pueda comunicarse con él a través de su interfaz I2C.|1|
|Módulo de Sensor de ritmo cardíaco|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/e77beec2-d67c-4417-a86a-c0900bf5f697)|El MAX30102 es un oxímetro de pulso y monitor de frecuencia cardíaca integrado que transmite el módulo del sensor. Integra un LED rojo y un LED infrarrojo, detectores fotoeléctricos, dispositivos ópticos y circuitos electrónicos de bajo ruido con supresión de luz ambiental.|1|
|Sensor Digital de aceleración de gravedad|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/3c5761f5-84ea-44cf-8721-45a587ce979c)|ADXL345 es un acelerómetro de tres ejes pequeño, delgado y de baja potencia que puede ser alto. ADXL345 es muy adecuado para aplicaciones de dispositivos móviles. Puede inclinar aplicaciones de detección. Midiendo la aceleración estática de la gravedad, también puede medir la dinámica agregar movimiento o tras un choque.|1|
|Altavoz de Audio y sonido|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/31d59aae-bd45-43f0-9093-09becb005dbe)|Este módulo puede conducir directamente un pequeño altavoz 8 Ohm 0,5 W|1|
|Módulo LED RGB|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/dd17a98f-86f9-4d62-9b52-935fb566a73a)|**Tamaño:** 25*21mm, **Color:** RGB, **Brillo:** resaltado, **Voltaje:** 5 V, **Entrada:** señal digital|1|
|Jumpers Macho - Macho|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/71b6317e-de20-4030-ba54-61f68d761717)|Un jumper o saltador es un elemento que permite cerrar el circuito eléctrico del que forma parte dos conexiones. La función del cable macho-macho es con frecuencia usado en el tablero protoboard haciendo posible la conexión de dos elementos ingresados en dicho tablero.|30|
|Soldador|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/fc5a82e1-760e-41a1-afff-2f87b009fda1)|Un soldador eléctrico o de estaño es una herramienta eléctrica que se utiliza para realizar soldadura blanda, es decir, con material que tiene una baja temperatura de fusión, como por ejemplo el Estaño.|1|
|ESP32|![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/fa57010d-71b8-4d84-b748-d153d7f7c334)|ESP32 es la denominación de una familia de chips SoC de bajo coste y consumo de energía, con tecnología Wi-Fi y Bluetooth de modo dual integrada.|1|
|Estaño| ![image](https://github.com/user-attachments/assets/cb6ca65c-81a8-4997-a1bf-9d83e578dd2c) | El estaño para soldar no es estaño puro, sino una aleación, normalmente de estaño y plomo. La composición ideal es del 60% de estaño por un 40% de plomo con adición de resina para facilitar el estañado. | 17gr |
| Flux | ![image](https://github.com/user-attachments/assets/1a3003bf-0f5a-4244-98c4-1e966d5a9ac7) | El flux para soldar, también conocido como fundente para soldar es un producto químico, generalmente en forma de pasta fluida, que al ser aplicado sobre los componentes que vamos a soldar elimina el óxido existente entre ellos aumentando sustancialmente la calidad de la soldadura. | 1 |

---

## Historias de usuario (épicas)
|Historia de Usuario|
|-------------------|
|**Número:** 1|
|**Nombre:** Seguimiento de Salud Avanzado|
|**Usuario:** Usuario|
|**Descripción:** Ampliar las capacidades de seguimiento de salud de la pulsera inteligente para incluir mediciones adicionales, como el promedio de pulsaciones por minuto del usuario se realizar un análisis avanzado de datos para detectar posibles problemas de salud y alertar al usuario cuando sea necesario.|

|Historia de Usuario|
|-------------------|
|**Número:** 2|
|**Nombre:** Funcionalidades de Conectividad Avanzada|
|**Usuario:** Usuario|
|**Descripción:** Mejorar las funciones de conectividad de la pulsera inteligente para garantizar una comunicación fluida y una integración sin problemas con otros dispositivos, como podría ser un dispositivo con pantalla inteligente en la cual desplegar un recuento de los datos recabados.|

|Historia de Usuario|
|-------------------|
|**Número:** 3|
|**Nombre:** Funcionalidades de Conectividad Avanzada|
|**Usuario:** Usuario|
|**Descripción:** Permitir una personalización más profunda de la experiencia del usuario con la pulsera inteligente. Esto incluye opciones adicionales para adaptar la interfaz y las funciones según las preferencias individuales.|

---

## Esquema Base de Datos
![image](https://github.com/user-attachments/assets/7543a3e0-bcb3-437f-9b65-f16f6c8f08e9)

---

## Prototipo
![Imagen de WhatsApp 2024-05-31 a las 09 33 08_25c2c33b](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/e6001b40-e220-4bb8-af91-fbe0bdff238f)

---

## Arquitectura del proyecto
![Untitled](https://github.com/AlexRGB2/EmotionPulse/assets/107822697/8b39478c-615c-4452-b21d-c180e6a1edd5)

---

## Tablero Kanban
![image](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/81d248ff-4534-4ef1-9ff2-cf64e3414fd9)

---

## Circuito diseñado
![EsquemaEmotionPulse_bb](https://github.com/AlexRGB2/EmotionPulse/assets/41849043/6805ecbe-1568-44bc-ac4f-fa7175b2f4e9)

---

## Resultados
