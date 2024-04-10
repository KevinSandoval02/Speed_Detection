# Descripción de la AMB82-MINI
## AMB82-MINI
![AMB82-MINI](https://github.com/KevinSandoval02/Speed_Detection/assets/104235535/51324c0c-405c-48ab-b07d-834e2ca1cd8b)

Nota: Tomado de Ameba ARDUINO: Getting Started with AMB82 MINI (RTL8735B).
Disponible en: https://www.amebaiot.com/en/amebapro2-amb82-mini-arduino-getting-started/

La placa de desarrollo AMB82-MINI, diseñada en base al SoC (System on Chip) Realtek RTL8735BDM, es una herramienta versátil destinada para simplificar la creación de aplicaciones de cámaras de red AI. A continuación, se amplía la información sobre sus características principales:
- SoC altamente integrado: El RTL8735BDM cuenta con un SoC 8 en 1 con IA y NPU (unidad de procesamiento neuronal) integrados. Este chip tiene arquitectura ARMv8M y admite una velocidad de hasta 500 MHz, además permite aplicaciones de cámara de red AI con un consumo de energía extremadamente bajo.

- ## Controlador principal RTL8735BDM.
![Controlador principal RTL8735BDM](https://github.com/KevinSandoval02/Speed_Detection/assets/104235535/fb8f5393-0e82-44ab-bbd1-40b70941bf2d)

Nota: Tomado de Getting Started with Realtek AMB82-Mini IoT AI Camera Arduino Dev. Board
Disponible en: https://how2electronics.com/getting-started-with-realtek-amb82-mini-iot-ai-camera-arduino-dev-board/

- Cámara de alta calidad: Equipada con el módulo de cámara JXF37, la placa AMB82-MINI cuenta con una cámara de 5 megapíxeles y resolución Full HD CMOS con un sensor de 1920×1080 píxeles. Su amplio ángulo de visión de 130 grados y la compensación de tono y contraste permiten capturar imágenes nítidas y detalladas en una variedad de condiciones.

- ## Módulo de cámara JXF37.
![Módulo de cámara JXF37](https://github.com/KevinSandoval02/Speed_Detection/assets/104235535/63046f8c-b944-4352-b686-14dfc8aa0965)

Nota: Tomado de: Getting Started with Realtek AMB82-Mini IoT AI Camera Arduino Dev. Board
Disponible en: https://how2electronics.com/getting-started-with-realtek-amb82-mini-iot-ai-camera-arduino-dev-board/

- **Eficiencia energética y rápido arranque:** La placa AMB82-MINI ofrece un arranque rápido en milisegundos y un consumo de energía ultra bajo en microamperios, lo que la hace ideal para dispositivos alimentados por baterías y aplicaciones de IoT.
- **Codificación en tiempo real de múltiples transmisiones:**
 Con soporte para codificación H.265/H.264/JPEG en tiempo real de múltiples transmisiones, la placa permite la transmisión de video de alta calidad en aplicaciones de vigilancia y monitoreo.
- **Compatibilidad con diversos ecosistemas:**
Respaldada por ecosistemas de IA e IoT alimentados por baterías, la AMB82-MINI ofrece una plataforma versátil para el desarrollo de aplicaciones inteligentes y conectadas.
- **SDK compatible con varios entornos de desarrollo:**
El SDK proporcionado admite varios entornos de desarrollo, incluidos RTOS, IAR, GCC y Arduino IDE, lo que facilita la programación y el desarrollo de aplicaciones de IoT y IA.

## Características de la AMB82-MINI
La AMB82-MINI se distingue por las siguientes características adicionales:
- **Procesador de señal de imagen avanzado (ISP):** El ISP integrado en la AMB82-MINI ofrece características avanzadas de reducción de ruido, soporte para HDR y WDR, compensación de píxeles muertos, corrección de sombreado de lente y ajuste de contraste y nitidez, entre otras funciones, garantizando la calidad óptima de las imágenes capturadas.
- **Conectividad y expansión:** Con múltiples interfaces, incluyendo UART, GPIO, ADC, PWM, IIC, SPI y MIPI CSI-2, la placa AMB82-MINI puede conectarse con una amplia gama de componentes electrónicos y sensores externos para expandir su funcionalidad y aplicaciones.
- **Flexibilidad y versatilidad: **La placa proporciona una amplia gama de interfaces periféricas, incluidas WiFi, BLE, GPIO, I2C, UART, SPI, PWM y ADC, lo que permite la conexión con una variedad de componentes electrónicos y sensores para aplicaciones de IoT.
- **Bajo costo:** Con un costo de 25 USD, la placa AMB82-MINI ofrece una solución rentable para el desarrollo de sistemas embebidos con capacidades de IA y IoT.

En resumen, la AMB82-MINI ofrece una solución integral para el desarrollo de aplicaciones de IoT y AI, combinando eficiencia energética, rendimiento de procesamiento avanzado, precio asequible y una amplia gama de características para satisfacer las necesidades de los desarrolladores en el campo de la visión artificial y el Internet de las cosas.

# Dataset
## Fotogramas extraídos (2.051)
Link: https://drive.google.com/drive/folders/10hQ8nfDVo2Ck9GHXqOT_V2CleYAyBXKl?usp=sharing
## Imágenes etiquetadas en Roboflow (2.051)
Link: https://app.roboflow.com/setdedatos/data_cars_d/browse?queryText=&pageSize=50&startingIndex=0&browseQuery=true
## Imágenes etiquetadas en formato YOLO (4929)
Link: https://drive.google.com/drive/folders/14F2FvHScWwpUnKARl2jV2hscndO90S94?usp=sharing

# Entrenamiento de YOLOv4 Tiny
Train YOLOv4 Tiny Colab Notebook used: https://colab.research.google.com/drive/1Bta3F2dt90FczyhJYJ0oySlgJlrVNqmN?usp=sharing

Código de entrenamiento original (Tutorial) YOLOv4-tiny Colab Notebook: https://colab.research.google.com/drive/1PWOwg038EOGNddf6SXDG5AsC8PIcAe-G?ref=blog.roboflow.com#scrollTo=NjKzw2TvZrOQ
Provided by:
> Jacob Solawetz, Samrat Sahoo. (Jul 1, 2020). Train YOLOv4-tiny on Custom Data - Lightning Fast Object Detection. Roboflow Blog: https://blog.roboflow.com/train-yolov4-tiny-on-custom-data-lighting-fast-detection/


Luego de entrenarse el modelo, se generan los archivos de los pesos (.weights) y el de configuración (.cfg), se seleccionó el archivo **custom-yolov4-tiny-detector_best.weights** y el archivo **custom-yolov4-tiny-detector.cfg**.

Archivos de pesos y configuración: https://drive.google.com/drive/folders/19gS7S1jcr8QG-PekcEJFxwaULLCxPw7S?usp=sharing

## Pasos para configurar el entorno de desarrollo:

**Paso 1: Entorno del sistema operativo**

La placa AMB82 Mini es compatible con varios sistemas operativos, incluidos Windows de 64 bits (Windows 10 y superior), Linux (Ubuntu 22 y superior) y MacOS (Intel y Apple Silicon). Se recomienda utilizar la última versión del sistema operativo para garantizar la compatibilidad y el rendimiento óptimo.

**Paso 2:** Instalación del controlador
Al conectar la placa AMB82 Mini a la computadora mediante un cable Micro USB, el controlador USB de la placa se instalará automáticamente en la mayoría de los casos. En caso de problemas con el controlador, se puede descargar desde el sitio web oficial del fabricante.

**Paso 3:** Configuración de Arduino IDE
Arduino IDE es una herramienta de desarrollo ampliamente utilizada que admite hardware de terceros, incluida la placa AMB82 Mini. A fin de configurar Arduino IDE para el desarrollo en la placa se siguen estos pasos:

**1.**	Descargar Arduino IDE desde el sitio web oficial de Arduino.

**2.**	Abrir Arduino IDE e ir a "Archivo" -> "Preferencias".

**3.**	Pegar la siguiente URL “https://github.com/ambiot/ambpro2_arduino/raw/main/Arduino_package/package_realtek_amebapro2_index.json”.

**4.**	Ir a "Herramientas" -> "Tablero" -> "Administrador de tableros" y buscar "Realtek Ameba Boards" en la lista.

**5.**	Hacer clic en "Instalar" y esperar a que se descarguen los archivos necesarios.

**6.**	Seleccionar la placa AMB82 Mini en "Herramientas" -> "Board" -> "AmebaPro2 ARM (32-bits) Boards" -> "AMB82-MINI".

Con estos pasos, el entorno de desarrollo estará configurado para comenzar a desarrollar aplicaciones para la placa AMB82 Mini.


## Integración del modelo de YOLO v4 Tiny en la placa AMB82 Mini

**1.	Entrenamiento del modelo y extracción de archivos .cfg y .weights:**

•	Se entrena el modelo YOLO v4 Tiny utilizando una base de imágenes recolectada previamente.

•	Una vez entrenado el modelo, se generan los archivos .cfg y .weights, que contienen la arquitectura de la red neuronal y los pesos aprendidos, respectivamente.

**2.	Acceso a la página de conversión de modelos:**

•	Se accede a la página oficial de AMB82-MINI, específicamente al apartado dedicado a la conversión de modelos AI. La URL de acceso es https://www.amebaiot.com/en/amebapro2-ai-convert-model/.

**3.	Conversión del modelo:**

•	En la página web mencionada, se completan varios campos obligatorios:

•	Se proporciona una dirección de correo electrónico para recibir el enlace de descarga del archivo convertido.

•	Se selecciona el tipo de modelo, en este caso, YOLO-TINY.

•	Se elige el tipo de cuantización, siendo UINT8 una opción común.

•	Se carga un archivo ZIP que incluye el archivo .cfg y el archivo .weights.

•	Se carga al menos una imagen JPEG para fines de cuantización.

•	Una vez cargados todos los archivos requeridos, se envía la solicitud.

**4.	Recepción del archivo convertido:**

•	Después de enviar la solicitud, se recibe un correo electrónico con un enlace de descarga.

•	Al hacer clic en el enlace, se descarga un archivo llamado network_binary.nb.

##### **4.1. Modelo obtenido**
- Archivo del modelo YOLOv4 Tiny personalizado: https://drive.google.com/drive/folders/1c3qgQ-yhb5WAtNNx5YPxl-rtV5B41K-I?usp=sharing 


**5.	Renombrar y reemplazar el archivo convertido:**

•	Se renombra el archivo network_binary.nb a yolov4_tiny.nb.

•	Este archivo renombrado se reemplaza en la ruta específica del sistema operativo, que en este caso es: C:\Users\User\AppData\Local\Arduino15\packages\realtek\hardware\AmebaPro2\4.0.5\variants\common_nn_models.

**6.	Ajuste del archivo ObjectClassList.h:**

•	Se abre el archivo ObjectClassList.h, que es un archivo de encabezado en formato C. En este archivo, se ajusta el código para reflejar el número de clases que el modelo identificará y el nombre de cada clase.

•	Se asegura de que el número de clases y los nombres coincidan con los resultados de la capacitación del modelo.

•	El archivo ObjectClassList.h se guarda con los cambios realizados.

**7.	Prueba del modelo en la placa AMB82 Mini:**

•	Con todos los pasos anteriores completados, el modelo YOLO v4 Tiny está listo para ser cargado y ejecutado en la placa AMB82 Mini.

•	Se carga el código ejecutable Arduino junto con el archivo de clases actualizado en la placa, ambos deben estar guardado en la misma ruta.
Este proceso asegura que el modelo YOLO v4 Tiny esté completamente integrado y funcional en la placa AMB82 Mini, lo que permite su implementación en aplicaciones de detección de objetos en tiempo real.




