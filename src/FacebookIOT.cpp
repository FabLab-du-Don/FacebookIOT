/*
   Copyright (c) 2018 FabLab du Don. All right reserved.

   facebookIOT - Interface pour l'API facebook des objets connectés

   https://developers.facebook.com/docs/facebook-login/for-devices
   
   This library is free software ; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation ; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY ; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library ; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "facebookIOT.h"

facebookIOT::facebookIOT(Client & client, String appId, String appToken) 
{
	_client = & client ;
	_accessToken = appId + "%7C" + appToken ;
    _errorMessage = "" ;
}

int facebookIOT::facebookGET(String command, String * response) 
{
    return facebookCMD(command, response, "GET ") ;
}

int facebookIOT::facebookPOST(String command, String * response) 
{
    return facebookCMD(command, response, "POST ") ;
}

int facebookIOT::facebookCMD(String command, String * response, const char CMD[]) 
{
//Serial.print (CMD) ; Serial.print (" command = ") ; Serial.println (command) ;
    String header = "" ;
    String body = "" ;
    int whereAmI ;
    int retCode ;
    unsigned long maxTime ;
    char c ;
    char lastchar = ' ' ;
    int debutJson = 0 ; 
    int finJson = 0 ;

    retCode = _client->connect (IOT_FB_HOST, IOT_FB_SSL_PORT) ;
    if (retCode)
    {
//Serial.println ("Connexion OK") ;        
        _client->print (CMD) ;
        _client->print (IOT_FB_Version) ;
        _client->print (command ) ;
        _client->println (" HTTP/1.1") ;
        _client->print ("Host: ") ;
        _client->println (IOT_FB_HOST) ;
        _client->println ("User-Agent: Excalibur v1.0") ;
        _client->println () ;

        
//Serial.println ("Requete envoyee") ;
        maxTime = millis () + IOT_TIMEOUT ;
        whereAmI = IOT_IN_HEADER ;
        while (millis () < maxTime)
        {
            while (_client->available ())
            {
                c = _client->read () ;
                //Serial.print (c) ;
                switch (whereAmI)
                {
                    case IOT_IN_HEADER:
                        if (c == '\n' && lastchar == '\n')
                        {
                            // Header end
//Serial.println ("Debut Header") ;                            
//Serial.println (header) ;
//Serial.println ("Fin Header") ;
                            whereAmI = IOT_IN_BODY ;                            
                        }
                        else
                        {
                            header += c ;                   // For debug only
                            if (c != '\r') lastchar = c ;
                        }
                        break ;
                    case IOT_IN_BODY:
                        // Il faut controler de ne pas dépasser les limites raisonnables
                        // sachant que Facebook n'indique aucune limite à un jeton (255 pouvant être dépassé)
                        body += c ;
                        break ;
                    default:
                        ;
                }
            }
            delay (1) ;
            // Il n'y a rien à lire, mais il reste du temps, on vérifie qu'on a un body complet
            // Pour un Json à 2 niveaux, on peut avoir une accolade ouvrante et une fermante, sans avoir 
            // la totalité.
            debutJson = body.indexOf ("{") ;
            finJson = body.lastIndexOf("}") ;
            if (debutJson != -1 && finJson != -1)
            {
                // Inutile d'attendre davantage
                break ;
            }
        }
    }
    else    // 
    {
        _errorMessage = String("Connection failed with ") + String(IOT_FB_HOST) + String(" (") + retCode + String(")") ;
        return IOT_ERROR_CONN ;
    }
//Serial.println ("Debut Body") ;
//Serial.println (body) ;
//Serial.println ("Fin Body") ;                

 	* response = body.substring(debutJson, finJson +1) ;
    _errorMessage = "" ;
	return IOT_NO_ERROR ;
}


/*
 * askAuth
 * 
 * Première phase du mécanisme d'authentification. La requête est faite de la part de l'objet connecté, identifié par son Identifiant d'App et
 * par son jeton d'accès. Ces 2 informations ont été saisies lors de l'instanciation de la classe.
 * Il faut préciser dans cette demande, quels seront les privilèges demandés (scope). Ce sera pour la prochaine version.
 * En retour, on a un code (permettant de vérifier que la réponse à l'authentification correspond bien à la reqûete), une invite que l'utilisateur
 * devra saisir en ligne, l'URL sur laquelle l'utilisateur devra faire sa saisie et son authentification, un délai d'expiration de la demande qui
 * correspond au temps que ne doit pas dépasser l'utilisateur pour se connecter, et enfin un interval minimum à respecter entre 2 interrogations
 * du serveur d'autorisation pour récupérer le résuktat de l'authentification.
 */
int facebookIOT::askAuth (String scopeList, String * authCode, String * authPrompt, String * authUri, unsigned int * authExpire, unsigned int * authDelay)
{
    String command ;
    String response ;
    int retCode ;
    DynamicJsonBuffer jsonBuffer ;
    
    // init output
    * authCode = "" ;
    * authPrompt = "" ;
    * authUri = "" ;
    * authExpire = 0 ;
    * authDelay = 0 ;
    _errorMessage = "" ;
    
    // beginning of construction of command
    command = "device/login?" ;
    if (scopeList.length () > 0 )
    {
        command += "scope=" + scopeList + "&" ;
    }
    command += "access_token=" + _accessToken ;
    retCode = facebookPOST (command, & response) ;
    if (retCode != IOT_NO_ERROR)
    {
        return retCode ;
    }
//Serial.println ("Analyse du Json : ") ;
//Serial.println (response) ;    
    JsonObject& root = jsonBuffer.parseObject(response) ;
    if (!root.success()) 
    {
        _errorMessage = "Not a JSON format" ;
        return IOT_ERROR_JSON ; 
    }
    if (root.containsKey("error"))
    {
        _errorMessage = root["error"]["message"].as<String>() ;  
        return IOT_ERROR_GENE ;
    }
    else if (root.containsKey("code"))
    {
        * authCode = root["code"].as<String>() ;
        _authCode = * authCode ;
        * authPrompt = root["user_code"].as<String>() ;
        * authUri = root["verification_uri"].as<String>() ;
        * authExpire = root["expires_in"].as<unsigned int>() ;
        * authDelay = root["interval"].as<unsigned int>() ;
        _authDelay = * authDelay ;
    }
    else
    {
        _errorMessage = "Unable to know what's in the JSON" ;
        return IOT_ERROR_GENE ;
    }
    return IOT_NO_ERROR ;
}

/*
 * checkAuth
 * 
 * Deuxième phase de l'authentification. Pendant que l'utilisateur se connecte depuis son PC ou tout autre navigateur,
 * on interroge à instants réguliers le serveur d'autoirisation de Facebook pour récupérer l'état de la procédure. Si celle-ci
 * est en cours, on continue les interrogations, si celle-ci a expiré, on arrête tout, c'est un échec. Si la procédure a abouti,
 * on récupère le jeton de l'utilisateur et on va pouvoir interragir avec Facebook.
 */
 
int facebookIOT::checkAuth (String * userToken, unsigned long * expireIn)
{
    String command ;
    String response ;
    int retCode ;
    DynamicJsonBuffer jsonBuffer ;
    unsigned long nextRequest = 0 ;
    unsigned long interval ;
    int numError ;

    // init output
    * userToken = "" ;
    * expireIn = 0 ;
    _errorMessage = "" ;

    // beginning of construction of command
    command = "device/login_status?" ;
    command += "code=" + _authCode ;
    command += "&access_token=" + _accessToken ;

    interval = _authDelay * 1000 ;  // interval in milliseconds, _authDelay in seconds

    while (true)
    {
        if (millis () > nextRequest)
        {
            retCode = facebookPOST (command, & response) ;
            if (retCode != IOT_NO_ERROR)
            {
                return retCode ;
            }
            JsonObject & root = jsonBuffer.parseObject(response) ;
            if (!root.success()) 
            {
                _errorMessage = "Not a JSON format" ;
                return IOT_ERROR_JSON ; 
            }
            if (root.containsKey("error"))
            {
                numError = root["error"]["code"].as<int>() ;
                switch (numError)
                {
                    case IOT_CHECK_POLL:
//Serial.print ("x") ;
                        break ;
                    case IOT_CHECK_FAST:
                        interval += interval / 10 ;
                        break ;
                        case IOT_CHECK_DEAD:
                        _errorMessage = root["error"]["error_user_msg"].as<String> () ;
                        return IOT_ERROR_LATE ;
                        //break ;
                    default:
//Serial.println () ;
//Serial.print ("Erreur inconnue : ") ;
//Serial.println (numError) ;
                        _errorMessage = root["error"]["error_user_msg"].as<String> () ;
                        return IOT_ERROR_GENE ;
                }
            }
            else if (root.containsKey("access_token"))
            {
                * expireIn = root["expires_in"].as<unsigned long>() ;
                * userToken = root["access_token"].as<String>() ;
                _userToken = * userToken ;
                return IOT_NO_ERROR ;
            }
            else
            {
                _errorMessage = "Unable to know what's in the JSON" ;
                return IOT_ERROR_GENE ;
            }
            jsonBuffer.clear () ;
            nextRequest = millis() + interval ;
        }
        delay (1) ;
}

    
 
    
	delay (5000) ;
	* userToken = "0123456789azertyuiopqsdfghjklmwxcvbn" ;
	_userToken = * userToken ;
	return IOT_NO_ERROR ;
}	
    
int facebookIOT::clear()
{
    _errorMessage = "" ;
    _accessToken = "" ;
    _authDelay = 0 ;
    _authCode = "" ;
    _userToken = "" ;
    return IOT_NO_ERROR ;
}

int facebookIOT::getName(String pageId, String * pageName)
{
    String command ;
    String response ;
    int retCode ;
    DynamicJsonBuffer jsonBuffer ;

    // init output
    * pageName = "" ;
    _errorMessage = "" ;

	command = pageId + "?fields=name&access_token=" + _accessToken ;
	retCode = facebookGET(command, & response) ;

    if (retCode != IOT_NO_ERROR)
    {
        return retCode ;
    }
//Serial.println ("Analyse du Json : ") ;
//Serial.println (response) ;    

    JsonObject& root = jsonBuffer.parseObject(response) ;
    if (!root.success()) 
    {
        _errorMessage = "Not a JSON format" ;
        return IOT_ERROR_JSON ; 
    }
    if (root.containsKey("error"))
    {
        _errorMessage = root["error"]["message"].as<String>() ;  
        return IOT_ERROR_GENE ;
    }
    else if (root.containsKey("name"))
    {
        * pageName = root["name"].as<String>() ;
    }
    else
    {
        _errorMessage = "Unable to know what's in the JSON" ;
        return IOT_ERROR_GENE ;
    }
    return IOT_NO_ERROR ;
}

int facebookIOT::getFanCount(String pageId, unsigned int * fanCount)  
{
    String command ;
    String response ;
    int retCode ;
    DynamicJsonBuffer jsonBuffer ;

    // init output
    * fanCount = 0 ;
    _errorMessage = "" ;

    command = pageId + "?fields=fan_count&access_token=" + _accessToken ;
    retCode = facebookGET(command, & response) ;

    if (retCode != IOT_NO_ERROR)
    {
        return retCode ;
    }
//Serial.println ("Analyse du Json : ") ;
//Serial.println (response) ;    

    JsonObject& root = jsonBuffer.parseObject(response) ;
    if (!root.success()) 
    {
        _errorMessage = "Not a JSON format" ;
        return IOT_ERROR_JSON ; 
    }
    if (root.containsKey("error"))
    {
        _errorMessage = root["error"]["message"].as<String>() ;  
        return IOT_ERROR_GENE ;
    }
    else if (root.containsKey("fan_count"))
    {
        * fanCount = root["fan_count"].as<unsigned int>() ;
    }
    else
    {
        _errorMessage = "Unable to know what's in the JSON" ;
        return IOT_ERROR_GENE ;
    }
    return IOT_NO_ERROR ;
}
