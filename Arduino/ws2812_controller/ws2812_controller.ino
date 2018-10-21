/*
 * 注意，一定要看到文件隔壁还有很多后缀名为".h"的文件，如果没有的话，那说明当前文件夹下没有这个文件。编译会出错。
 * 参考资料:https://github.com/scottlawsonbc/audio-reactive-led-strip
 * 源码2年未更新，稍作修改。
 * 你只需要修改代码后面后中文注释的地方！
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WiFiUdp.h>
#include "ws2812_i2s.h"


#define NUM_LEDS 120 //LED灯数量

#define BUFFER_LEN 1024

#define PRINT_FPS 1


const char* ssid     = "zzzzzzzccccccc"; //你家WIFI名字
const char* password = "zcp5313.."; //你家WIFI密码
unsigned int localPort = 7777;
char packetBuffer[BUFFER_LEN];


static WS2812 ledstrip;
static Pixel_t pixels[NUM_LEDS];
WiFiUDP port;

//你可以使用Windows+R键输入cmd,然后输入命令ipconfig 查看自己网络情况。

IPAddress ip(192, 168, 1, 150); //你给ESP8266设置的IP地址，注意，这个IP不可以跟其他设备（手机电脑灯）重复。

IPAddress gateway(192, 168, 1, 1); //你家网络的网关，这个跟你电脑的网关一样

IPAddress subnet(255, 255, 255, 0); //子网掩码，跟你电脑的一样，一般不需要修改。

void setup() {
    Serial.begin(115200);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
    // Connect to wifi and print the IP address over serial
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    port.begin(localPort);
    ledstrip.init(NUM_LEDS);
}

uint8_t N = 0;
#if PRINT_FPS
    uint16_t fpsCounter = 0;
    uint32_t secondTimer = 0;
#endif

void loop() {
    // Read data over socket
    int packetSize = port.parsePacket();
    // If packets have been received, interpret the command
    if (packetSize) {
        int len = port.read(packetBuffer, BUFFER_LEN);
        for(int i = 0; i < len; i+=4) {
            packetBuffer[len] = 0;
            N = packetBuffer[i];
            pixels[N].R = (uint8_t)packetBuffer[i+1];
            pixels[N].G = (uint8_t)packetBuffer[i+2];
            pixels[N].B = (uint8_t)packetBuffer[i+3];
        } 
        ledstrip.show(pixels);
        #if PRINT_FPS
            fpsCounter++;
        #endif
    }
    #if PRINT_FPS
        if (millis() - secondTimer >= 1000U) {
            secondTimer = millis();
            Serial.printf("FPS: %d\n", fpsCounter);
            fpsCounter = 0;
        }   
    #endif
}
