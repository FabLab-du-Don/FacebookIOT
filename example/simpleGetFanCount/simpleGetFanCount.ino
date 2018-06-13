/*
 * 
 * 
 * 
 */
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

#include "FacebookIOT.h"
#include "simpleGetFanCount.h"


// All the secrets are in secret.h or secret.h is empty and all the secrets are here
#include "secret.h"
#ifndef secret_h
#define APPID "123456789012345"
#define APPTOKEN "0123456789abcdef0123456789abcdef"
#define SSID "MY_WIFI_SSID"
#define PASSWORD "PASSWORD_OF_MY_WIFI"
//#define SSID2
//#define PASSWORD2
#endif

ESP8266WiFiMulti wifiMulti ;
WiFiClientSecure client ;
facebookIOT * api ;


String scopeList = "" ;
String authCode = "" ;
String authPrompt = "" ;
String authUri = "" ;
unsigned int authExpire = 0 ;
unsigned int authDelay = 0 ;
String userToken = "" ;

unsigned long nextRequest ;
unsigned long expireIn ;

String sec2string (unsigned long seconds) ;

void setup ()
{
    int retCode ;
    
    Serial.begin (115200) ;
    wifiMulti.addAP (SSID, PASSWORD) ;
    #ifdef SSID2
    wifiMulti.addAP (SSID2, PASSWORD2) ;
    #endif
    WiFi.mode (WIFI_STA) ;
    Serial.print("WiFi Connection ") ;
    while (wifiMulti.run() != WL_CONNECTED) 
    {
        Serial.print (".") ;
        delay(500) ;
    }
    Serial.println(" OK") ;

    api = new facebookIOT (client, APPID, APPTOKEN) ;

//delay(1000) ;
    //scopeList = "public_profile" ;  
    retCode =  api->askAuth (scopeList, & authCode, & authPrompt, & authUri, & authExpire, & authDelay) ;
//Serial.print ("Code de retour askAuth : ") ; Serial.println (retCode) ;
    if (retCode != IOT_NO_ERROR)
    {
        Serial.println (api->_errorMessage) ;
        Serial.println ("Please reboot") ;
        for (;;) delay(100) ;
    }
    Serial.println ("====================================") ;
    Serial.println ("Login with Facebook") ;
    Serial.print   ("Next, visit ") ;
    Serial.println (authUri) ;
    Serial.println ("on your smartphone or computer") ;
    Serial.println ("and enter this code:") ;
    Serial.println (authPrompt) ;
    Serial.println ("====================================") ;
    
    retCode = api->checkAuth (& userToken, & expireIn) ;
//Serial.print ("Code de retour checkAuth : ") ; Serial.println (retCode) ;
    Serial.print ("Authenticated during : ") ;  Serial.println (sec2string (expireIn)) ;
    Serial.print ("Jeton de session     : ") ;  Serial.println (userToken) ;
    nextRequest = millis () ;
}

void loop ()
{
    String pageName ;
    unsigned int fanCount ;
    int retCode ;
        
    if (millis () >= nextRequest)
    {
        retCode =  api->getName (PAGEID, & pageName) ;  
        if (retCode != IOT_NO_ERROR)
        {
            Serial.println (api->_errorMessage) ;
            pageName = "Nom de page inconnue" ;
        }
        retCode = api->getFanCount (PAGEID, & fanCount) ;
        if (retCode != IOT_NO_ERROR)
        {
            Serial.println (api->_errorMessage) ;
            fanCount = 0 ;
        }

        Serial.print ("Page: '") ;
        Serial.print (pageName) ;
        Serial.print ("' has ") ;
        Serial.print (fanCount) ;
        Serial.println (" fans") ;

        nextRequest = millis() + POLL_DELAY ;
    }
        delay (5) ;
}

String sec2string (unsigned long duration) 
{
    unsigned long days ;
    unsigned long hours ;
    unsigned long minutes ;
    unsigned long seconds ;

    seconds = duration % 60 ;
    duration /= 60 ;
    minutes = duration % 60 ;
    duration /= 60 ;
    hours = duration % 24 ;
    days = duration/= 24 ;

    return String(days,DEC) + " days " + String(hours,DEC) + " hours " + String(minutes,DEC) + " minutes " + String(seconds, DEC) + " seconds" ;
}

