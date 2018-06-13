/*
Copyright (c) 2018 FabLAb du Don. All right reserved.

facebookIOT - Interface pour l'API facebook des objets connectés

https://developers.facebook.com/docs/facebook-login/for-devices

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/


#ifndef facebookIOT_h
#define facebookIOT_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define IOT_FB_HOST "graph.facebook.com"
#define IOT_FB_SSL_PORT 443
#define IOT_FB_Version  "/v3.0/"
#define IOT_TIMEOUT     10000        // Timeout for requests GET and POST, in milliseconds

#define IOT_ERROR_AUTH -111         // Auth failed, userToken must be cleared
#define IOT_ERROR_GENE -222         // An error has occured
#define IOT_ERROR_JSON -333         // Not a json format
#define IOT_ERROR_NYET -444         // Not yet implemented
#define IOT_ERROR_LATE -555         // Activation Code Expired
#define IOT_ERROR_CONN -666         // Connection with Facebook server failed
#define IOT_NO_ERROR	0

#define IOT_CHECK_POLL  31          // User has not yet authorized your application. Continue polling.
#define IOT_CHECK_FAST  17          // Your device is polling too frequently. Space your requests with a minium interval of 5 seconds.
#define IOT_CHECK_DEAD 463          // The code you entered has expired. Please go back to your device for a new code and try again.

#define IOT_IN_HEADER   0           // Item for whereAmI
#define IOT_IN_BODY     1           // Item for whereAmI

class facebookIOT
{
  public:
    facebookIOT(Client & client, String appId, String appToken) ;
	int askAuth (String scopeList, String * authCode, String * authPrompt, String * authUri, unsigned int * authExpire, unsigned int * authDelay) ;
	int checkAuth (String * userToken, unsigned long * expireIn) ;
    int clear (void) ;
    int getFanCount(String pageId, unsigned int * fanCount) ;
	int getName(String pageId, String * pageName) ;
    String _errorMessage ;

  private:
    int facebookCMD (String command, String * response, const char CMD []) ;
    int facebookGET (String command, String * response) ;
	int facebookPOST (String command, String * response) ;
    String _accessToken ;
	Client * _client ;
	unsigned int _authDelay ;
	String _authCode ;
	String _userToken ;
} ;

#endif
