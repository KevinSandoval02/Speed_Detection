#include <Arduino.h>
#include "WiFi.h"
#include "StreamIO.h"
#include "VideoStream.h"
#include "RTSP.h"
#include "NNObjectDetection.h"
#include "VideoStreamOverlay.h"
#include "ObjectClassList.h"
#include "MP4Recording.h"
#include "AmebaFatFS.h"
#include "Base64.h"


#define CHANNEL 0 //Canal dedicado a la visualización de video vía rtsp 
#define CHANNELNN 3 //Canal dedicado a la detección de objetos
#define CHANNEL_MP4 2 //Canal dedicaco al almacenamiento de video en tarjeta micro SD
#define CHANNEL_MP4_2 1 //Canal #2 dedicaco al almacenamiento de video en tarjeta micro SD

//-----------------------------IFTTT--------------------------------------------
#define FILENAME    "image.jpg" // Nombre del archivo imágen que se capturará en el momento de una infracción de velocidad

// Enter your Google Script and details
String myScript = "/macros/s/AKfycbyx-J0RdD4Hn9_6VCMBSLGRadrcWfW4jjcWy1pNSFDcH8AFjRDMum7kYvm0CPQNfTme/exec";  // Create your Google Apps Script and replace the "myScript" path.
String myFoldername = "&myFoldername=AMB82";  // Set the Google Drive folder name to store your file
String myFilename = "&myFilename=image.jpg";  // Set the Google Drive file name to store your data
String myImage = "&myFile=";

uint32_t img_addr = 0;
uint32_t img_len = 0;

AmebaFatFS fs;
WiFiSSLClient wifiClient;


char buf[512];
char *p;


//--------------------------------------------------------------------------------------------------------------

#define GREEN_LED LED_G
#define BLUE_LED LED_B

//------------------------------Configuraciones de video-----------------------------------

VideoSetting config(VIDEO_D1, CAM_FPS, VIDEO_H264_JPEG, 1);  // High resolution video for streaming
VideoSetting configNN(VIDEO_VGA, 10, VIDEO_RGB, 0);
VideoSetting configMP4(VIDEO_VGA, 10, VIDEO_RGB, 0);

NNObjectDetection ObjDet;
RTSP rtsp;
StreamIO videoStreamer(1, 1);
StreamIO videoStreamerNN(1, 1);
StreamIO videoStreamerMP4(1, 1);
StreamIO videoStreamerMP4_2(1, 1);
extern VideoStreamOverlay OSD;

//-------------------------Asignación de red WIFI---------------------------------------------
char ssid[] = "ARLEN_TERRAZA";   // your network SSID (name)
char pass[] = "yoamolacarranga";       // your network password
int status = WL_IDLE_STATUS;

IPAddress ip;
int rtsp_portnum;


//--Coordenadas iniciales de la líneas guía para medición de velocidad------
// Line coordinates
int line1_x1 = 75;
int line1_y1 = 75;
int line2_x2 = 220;
int line2_y2 = 220;

//-----------------------------Variables para el cálculo de velocidad---------------------------------------
unsigned long startTime = 0;
unsigned long endTime = 0;
float distanceBetweenLines = 15; // Distancia entre las líneas en metros------------------------------------(AJUSTAR SEGÚN SEA NECESARIO)
float speedx = 0.0; // Velocidad calculada en km/h
bool timingStarted = false; // Bandera para indicar si se inició el temporizador

int CarCount = 0;//Variable para el conteo vehícular y posterior actualización del historial de velocidades en pantalla


struct SpeedHistory {//----------Estructura para almacenamiento de la últimas 4 velocidades
    float speeds[6];
    uint8_t index;
};

SpeedHistory speedHistory = {{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 0}; // Inicializar el historial con ceros

//-----------------------------------------------------------------------------------



//----------------Estructura para el seguimiento de vehículos-----------------------------
struct Vehicle {
    unsigned long startTime;
    bool timingStarted;
    MP4Recording mp4; // Grabador MP4 para vehículo
    MP4Recording mp4_2; //Grabador MP4 n2 para vehículo
};

// Arreglo de vehículos
Vehicle vehicles[20000];//------------------------------------------------------------(AJUSTAR)


int Nvh = 0;
int Vhc = 0;



// Variables booleanas para detección de velocidad de varios vehículos al tiempo
bool flag = true;
bool flag2= true;
bool flag3= false;
bool flag4= false;


int TopSpeed = 100;// Velocidad máxima permitida en el área de interés------------------------------(AJUSTARSE EN CASO DE SER UTILIZADA)

float Tolerance = 0.2; //Esta variable se utiliza para graduar las zonas de activación

int NRV = 0; //Número de vehiculo grabado

//Contadores para infraccciones 
int NVI = 1;
int NI = 1;

// Función para calcular la distancia entre dos puntos
float dist(int x, int y, int x1, int y1) {
    return sqrt(pow(x - x1, 2) + pow(y - y1, 2));
}

// Función para verificar si un punto está en una línea con una tolerancia dada
bool isPointOnLine(int px, int py, int x1, int y1, int x2, int y2, float tolerance) {
    float d1 = dist(px, py, x1, y1);
    float d2 = dist(px, py, x2, y2);
    float lineLen = dist(x1, y1, x2, y2);

    return (d1 + d2 <= lineLen + tolerance);
}

// Función para verificar si el centro del rectángulo toca una línea con una tolerancia dada
bool touchesLineCenter(int xmin, int ymin, int xmax, int ymax, int x1, int y1, int x2, int y2, float tolerance) {
    int centerX = (xmin + xmax) / 2;
    int centerY = (ymin + ymax) / 2;

    return isPointOnLine(centerX, centerY, x1, y1, x2, y2, tolerance);
}



void setup() {
    uint16_t im_h = config.height();
    uint16_t im_w = config.width();

  
    Serial.begin(115200);
    
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);

        // wait 2 seconds for connection:
        delay(2000);
    }

    ip = WiFi.localIP();

    // Configure camera video channels with video format information
    // Adjust the bitrate based on your WiFi network quality
    config.setBitrate(2 * 1024 * 1024);     // Recommend using 2Mbps for RTSP streaming to prevent network congestion
    Camera.configVideoChannel(CHANNEL, config);
    Camera.configVideoChannel(CHANNELNN, configNN);
    Camera.configVideoChannel(CHANNEL_MP4, config);
    Camera.configVideoChannel(CHANNEL_MP4_2, config);
    Camera.videoInit();

    // Configure RTSP with corresponding video format information
    rtsp.configVideo(config);
    rtsp.begin();
    rtsp_portnum = rtsp.getPort();

    ObjDet.configVideo(configNN);
    ObjDet.setResultCallback(ODPostProcess);
    ObjDet.modelSelect(OBJECT_DETECTION, DEFAULT_YOLOV4TINY, NA_MODEL, NA_MODEL);
    ObjDet.begin();

    // Configure StreamIO object to stream data from video channel to RTSP
    videoStreamer.registerInput(Camera.getStream(CHANNEL));
    videoStreamer.registerOutput(rtsp);
    if (videoStreamer.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start data stream from the video channel
    Camera.channelBegin(CHANNEL);

    // Configure StreamIO object to stream data from RGB video channel to object detection
    videoStreamerNN.registerInput(Camera.getStream(CHANNELNN));
    videoStreamerNN.setStackSize();
    videoStreamerNN.setTaskPriority();
    videoStreamerNN.registerOutput(ObjDet);
    if (videoStreamerNN.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    // Start the video channel for NN
    Camera.channelBegin(CHANNELNN);

    // Start OSD drawing on the RTSP video channel
    OSD.configVideo(CHANNEL, config);
    OSD.begin();
    delay(2000);
    
}

void loop() {
    
    if (flag4){//----Activar la función de captura de imágen---
      flag4=false;
      CaptureImage();
      delay(100);
    } 

}

// User callback function for post-processing of object detection results
void ODPostProcess(std::vector<ObjectDetectionResult> results) {
    uint16_t im_h = config.height();
    uint16_t im_w = config.width();
    
    OSD.createBitmap(CHANNEL);
    //PlotLines();
    OSD.drawLine(CHANNEL, 325, line2_x2, 610, line2_y2, 2, OSD_COLOR_RED);
    //OSD.drawLine(CHANNEL, 0,  ((line1_x1+line2_x2)/2), im_w, ((line1_y1+line2_y2)/2), 2, OSD_COLOR_GREEN);
    OSD.drawLine(CHANNEL, 310, line1_x1, 500, line1_y1, 2, OSD_COLOR_BLUE);
    //OSD.drawLine(CHANNEL, 0, line2_x2+45, im_w, line2_y2+45, 2, OSD_COLOR_GREEN);

    // Dibujar líneas simuladas en OSD

    if (ObjDet.getResultCount() > 0) {
        for (uint32_t i = 0; i < ObjDet.getResultCount(); i++) {
            int obj_type = results[i].type();
            if (itemList[obj_type].filter) {
                ObjectDetectionResult item = results[i];
                int xmin = (int)(item.xMin() * im_w);
                int xmax = (int)(item.xMax() * im_w);
                int ymin = ((int)(item.yMin() * im_h)) + 25;
                int ymax = ((int)(item.yMax() * im_h)) + 25;

                printf("Item %d %s:\t%d %d %d %d\n\r", i, itemList[obj_type].objectName, xmin, xmax, ymin, ymax);
                OSD.drawRect(CHANNEL, xmin, ymin, xmax, ymax, 3, OSD_COLOR_WHITE);
                OSD.drawPoint(CHANNEL, (xmin + xmax) / 2, (ymin + ymax) / 2, 10, OSD_COLOR_GREEN);

                char text_str[20];
                snprintf(text_str, sizeof(text_str), "%s %d", itemList[obj_type].objectName, item.score());
                OSD.drawText(CHANNEL, xmin, ymin - OSD.getTextHeight(CHANNEL), text_str, OSD_COLOR_CYAN);

                // Verificar si el punto toca la línea azul
                if (touchesLineCenter(xmin, ymin, xmax, ymax, 0, line1_x1, im_w, line1_y1, Tolerance) && flag) {
                    // Verificar si ha pasado la distancia mínima desde la última detección

                    vehicles[Nvh].startTime = 0;
                    vehicles[Nvh].timingStarted = false;
      
                        if (!vehicles[Nvh].timingStarted) {
                            vehicles[Nvh].startTime = millis();
                            vehicles[Nvh].timingStarted = true;                            
                            NRV++;
           
                            if (NRV % 2 != 0) {
                              // Este bloque de código se ejecutará solo si NVH es impar
                              vehicles[Nvh].mp4.configVideo(config);
                              vehicles[Nvh].mp4.setRecordingFileName("Vehicle_Recording_" + String(NRV)); // Nombre base del archivo de grabación
                              vehicles[Nvh].mp4.setRecordingDataType(STORAGE_VIDEO); // Grabar solo datos de video
                              videoStreamerMP4.registerInput(Camera.getStream(CHANNEL_MP4));                            
                              videoStreamerMP4.registerOutput(vehicles[Nvh].mp4);
                            } else {
                              // Este bloque de código se ejecutará si NVH es par
                              vehicles[Nvh].mp4_2.configVideo(config);
                              vehicles[Nvh].mp4_2.setRecordingFileName("Vehicle_Recording_" + String(NRV)); // Nombre base del archivo de grabación
                              vehicles[Nvh].mp4_2.setRecordingDataType(STORAGE_VIDEO); // Grabar solo datos de video
                              videoStreamerMP4_2.registerInput(Camera.getStream(CHANNEL_MP4_2));                            
                              videoStreamerMP4_2.registerOutput(vehicles[Nvh].mp4_2);
                            }

                            if(NRV == 1){//Se ejecuta solo una vez para el canal NRV impar
                              if (videoStreamerMP4.begin() != 0) {
                                  Serial.println("StreamIO link start failed");
                              }
                              
                            Camera.channelBegin(CHANNEL_MP4);
                            }

                            if(NRV == 2){//Se ejecuta solo una vez para el canal NRV par
                              if (videoStreamerMP4_2.begin() != 0) {
                                  Serial.println("StreamIO link start failed");
                              }
                              
                            Camera.channelBegin(CHANNEL_MP4_2);
                            }
                            
                            if (NRV % 2 != 0) {
                              vehicles[Nvh].mp4.begin();
                            }else{
                              vehicles[Nvh].mp4_2.begin();
                            }
                            
                            //printInfo();
                            digitalWrite(GREEN_LED, HIGH);

                            Nvh++;
                        }         
                          flag = false; 
                    }

                    if(touchesLineCenter(xmin, ymin, xmax, ymax, 0, ((line1_x1+line2_x2)/2), im_w, ((line1_y1+line2_y2)/2), Tolerance) && !flag) {//Este condicional permite realizar la detección de velocidad de otro vehículo al tiempo
                          flag=true;
                          }


                    // Calcular velocidad cuando el punto toca las línea roja
                if (touchesLineCenter(xmin, ymin, xmax, ymax, 0, line2_x2, im_w, line2_y2, Tolerance) && vehicles[Vhc].timingStarted && flag2) {

                    Serial.println("----Nvh----: ");
                    Serial.println(Nvh);
                    
                    Serial.println("----Vhc----: ");
                    Serial.println(Vhc);
                    endTime = millis();
                    CarCount++;
                    float elapsedTime = (endTime - vehicles[Vhc].startTime) / 1000.0;  // Tiempo en segundos
                    speedx = (distanceBetweenLines / elapsedTime) * 3.6;  // Velocidad en km/h
                    digitalWrite(GREEN_LED, LOW);
      
                    if (CarCount == 7) {
                      speedHistory.speeds[0]=0.0;
                      speedHistory.speeds[1]=0.0;
                      speedHistory.speeds[2]=0.0;
                      speedHistory.speeds[3]=0.0;
                      speedHistory.speeds[4]=0.0;
                      speedHistory.speeds[5]=0.0;
                      CarCount = 1;
                    }
                    
                    // Actualizar el historial de velocidades
                    speedHistory.speeds[speedHistory.index] = speedx;
                    speedHistory.index = (speedHistory.index + 1) % 6;
                    
                    if ((Vhc+1) % 2 != 0) {
                      vehicles[Vhc].mp4.end();
                      Serial.println("------------------------------");
                      Serial.println("- Summary of Streaming -");
                      Serial.println("------------------------------");
                      Camera.printInfo();
                      Serial.println("- MP4 Recording Information -");
                      vehicles[Vhc].mp4.printInfo();
                    }else{
                      vehicles[Vhc].mp4_2.end();
                      Serial.println("------------------------------");
                      Serial.println("- Summary of Streaming 2 -");
                      Serial.println("------------------------------");
                      Camera.printInfo();
                      Serial.println("- MP4_2 Recording Information -");
                      vehicles[Vhc].mp4_2.printInfo();
                    }

                    SpeedHistory();
                    delay(250);

                    Vhc++;

                    if(speedx>TopSpeed){//--------------Almacenar imágen si se excede el límite de velocidad permitido en la zona--------------------
                          Serial.println("- TOPE DE VELOCIDAD EXCEDIDA -");
                          flag4=true;
                          delay(100);
                          //flag3=true;
                          //RenameVideo();
                          NI++;
                          }else{
                            //DeleteVideo();
                          }

                    flag2 = false;
                    }
                    
                    if(touchesLineCenter(xmin, ymin, xmax, ymax, 0, (line2_x2+30), im_w, (line2_y2+30), Tolerance) && !flag2) {
                          flag2=true;
                          }
            }
        }
    }
    delay(200);
    SpeedHistory();
}


void SpeedHistory(){
  // Mostrar la velocidad y el historial
    char speedText0[30];
    sprintf(speedText0, "--SPEED HISTORY--");
    OSD.drawText(CHANNEL, 3, 10, speedText0, OSD_COLOR_YELLOW);

    char speedText00[30];
    sprintf(speedText00, "---LAST 3 CARS---");
    OSD.drawText(CHANNEL, 3, 35, speedText00, OSD_COLOR_YELLOW);

    char speedText1[30];
    sprintf(speedText1, "Car 1: %.2f kmh", speedHistory.speeds[0]);
    OSD.drawText(CHANNEL, 10, 65, speedText1, OSD_COLOR_WHITE);

    char speedText2[30];
    sprintf(speedText2, "Car 2: %.2f kmh", speedHistory.speeds[1]);
    OSD.drawText(CHANNEL, 10, 90, speedText2, OSD_COLOR_WHITE);

    char speedText3[30];
    sprintf(speedText3, "Car 3: %.2f kmh", speedHistory.speeds[2]);
    OSD.drawText(CHANNEL, 10, 115, speedText3, OSD_COLOR_WHITE);

    char speedText4[30];
    sprintf(speedText4, "Car 4: %.2f kmh", speedHistory.speeds[3]);
    OSD.drawText(CHANNEL, 10, 140, speedText4, OSD_COLOR_WHITE);

    char speedText5[30];
    sprintf(speedText5, "Car 5: %.2f kmh", speedHistory.speeds[4]);
    OSD.drawText(CHANNEL, 10, 165, speedText5, OSD_COLOR_WHITE);

    char speedText6[30];
    sprintf(speedText6, "Car 6: %.2f kmh", speedHistory.speeds[5]);
    OSD.drawText(CHANNEL, 10, 190, speedText6, OSD_COLOR_WHITE);

    OSD.update(CHANNEL);
  
}

void CaptureImage(){

  // SD card init
        if (!fs.begin()) {
            pinMode(LED_B, OUTPUT);
            digitalWrite(LED_B, HIGH);
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] SD Card Mount Failed(CAPTURE_IMAGE) !!!");
            Serial.println("================================");
            while(1);
        }

        // List root directory and put results in buf
        memset(buf, 0, sizeof(buf));
        String Rpath = String(fs.getRootPath()) + "/images";
        char dirPath[Rpath.length() + 1];
        Rpath.toCharArray(dirPath, Rpath.length() + 1);
        fs.readDir(dirPath, buf, sizeof(buf));
        String filepath = String(fs.getRootPath()) + String("/images/")+ String(FILENAME);
        
        File file = fs.open(filepath);
        if (!file) {
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] Failed to open file for reading(CAPTURE_IMAGE)");
            Serial.println("================================");
            fs.end();
        }
        Serial.println("Files under: " + String(fs.getRootPath() + String("/images")));
        Serial.println("Read from file: " + filepath);
        Serial.println("file size: " + String(file.size()));
        delay(100);

        // Taking Photo
        //CamFlash();
        Camera.getImage(CHANNEL, &img_addr, &img_len);
        file.write((uint8_t *)img_addr, img_len);
        file.close();
        // upadte OSD
        OSD.update(CHANNEL);
        delay(100);
        Serial.println("===================================");
        Serial.println("[INFO] Photo Captured ...");
        Serial.println("===================================");

        // File Processing
        p = buf;
        while (strlen(p) > 0) {
            /* list out file name image will be saved as "image.jpg" */
            if (strstr(p, FILENAME) != NULL) {
                Serial.println("Found 'image.jpg' in the string.");
            } else {
                // Serial.println("Substring 'image.jpg' not found in the
                // string.");
            }
            p += strlen(p) + 1;
        }

        uint8_t *fileinput;
        file = fs.open(filepath);
        unsigned int fileSize = file.size();
        fileinput = (uint8_t *)malloc(fileSize + 1);
        file.read(fileinput, fileSize);
        fileinput[fileSize] = '\0';
        file.close();
        //fs.end();     
        char *input = (char *)fileinput;
        String imageFile = "data:image/jpg;base64,";
        char output[base64_enc_len(3)];
        Serial.println("Viene el for");
        for (unsigned int i = 0; i < fileSize; i++) {
            base64_encode(output, (input++), 3);
            if (i % 3 == 0) {
                imageFile += urlencode(String(output));
            }
        }

        // transfer file to Google Drive
        String Data = myFoldername + myFilename + myImage;
        const char *myDomain = "script.google.com";
        String getAll = "", getBody = "";
        Serial.println("Connect to " + String(myDomain));

        if (wifiClient.connect(myDomain, 443)) {
            Serial.println("Connection successful");

            wifiClient.println("POST " + myScript + " HTTP/1.1");
            wifiClient.println("Host: " + String(myDomain));
            wifiClient.println("Content-Length: " + String(Data.length() + imageFile.length()));
            wifiClient.println("Content-Type: application/x-www-form-urlencoded");
            wifiClient.println("Connection: keep-alive");
            wifiClient.println();

            wifiClient.print(Data);
            for (unsigned int Index = 0; Index < imageFile.length(); Index = Index + 1000) {
                wifiClient.print(imageFile.substring(Index, Index + 1000));
            }

            int waitTime = 10000;  // timeout 10 seconds
            unsigned int startTime = millis();
            boolean state = false;

            while ((startTime + waitTime) > millis()) {
                // Serial.print(".");
                delay(100);
                while (wifiClient.available()) {
                    char c = wifiClient.read();
                    if (state == true) {
                        getBody += String(c);
                    }
                    if (c == '\n') {
                        if (getAll.length() == 0) {
                            state = true;
                        }
                        getAll = "";
                    } else if (c != '\r') {
                        getAll += String(c);
                    }
                    startTime = millis();
                }
                if (getBody.length() > 0) {
                    break;
                }
            }
            wifiClient.stop();
            Serial.println(getBody);
            Serial.println("File uploading done.");
            Serial.println("===================================");
        } else {
            getBody = "Connected to " + String(myDomain) + " failed.";
            Serial.println("Connected to " + String(myDomain) + " failed.");
        }
}

void RenameVideo(){//Función para renombrar el archivo de video guardado en caso de cometer infracción de velocidad
    // SD card init
        if (!fs.begin()) {
            pinMode(LED_B, OUTPUT);
            digitalWrite(LED_B, HIGH);
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] SD Card Mount Failed(RENAME_VIDEO) !!!");
            Serial.println("================================");
            while(1);
        }
  delay(250);
  //bool rename(String(fs.getRootPath()) + "Vehicle_Recording_" + String(Vhc) + ".mp4", String(fs.getRootPath()) + "Speeding_Violation_Car_" + String(NI) + ".mp4");
  String filepath1 = String(fs.getRootPath()) + "Vehicle_Recording_" + String(Vhc) + ".mp4";
  String filepath_2 = String(fs.getRootPath()) + "Speeding_Violation_" + String(NI) + ".mp4";
  bool file1 = fs.rename(filepath1, filepath_2);
  if (!file1) {
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] Failed to open file for reading(RENAME_VIDEO)");
            Serial.println("================================");
            fs.end();
        }
        delay(100);
}

void DeleteVideo(){//Función para eliminar el archivo de video guardado en caso de no cometer infracción de velocidad

    // SD card init
        if (!fs.begin()) {
            pinMode(LED_B, OUTPUT);
            digitalWrite(LED_B, HIGH);
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] SD Card Mount Failed !!!");
            Serial.println("================================");
            while(1);
        }

        String filepath2 = String(fs.getRootPath()) + "Vehicle_Recording_" + String(Vhc) + ".mp4";
        bool file2 = fs.remove(filepath2);
        if (!file2) {
            Serial.println("");
            Serial.println("================================");
            Serial.println("[ERROR] Failed to open file for reading(DELETE_VIDEO)");
            Serial.println("================================");
            fs.end();
        }
        delay(100);

}


String urlencode(String str) {
    const char *msg = str.c_str();
    const char *hex = "0123456789ABCDEF";
    String encodedMsg = "";
    while (*msg != '\0') {
        if (('a' <= *msg && *msg <= 'z') || ('A' <= *msg && *msg <= 'Z') ||
            ('0' <= *msg && *msg <= '9') || *msg == '-' || *msg == '_' ||
            *msg == '.' || *msg == '~') {
            encodedMsg += *msg;
        } else {
            encodedMsg += '%';
            encodedMsg += hex[(unsigned char)*msg >> 4];
            encodedMsg += hex[*msg & 0xf];
        }
        msg++;
    }
    return encodedMsg;
}
