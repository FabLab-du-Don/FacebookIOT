/*
Copyright (c) 2017 Brian Lough. All right reserved.
Modifications par FabLab du Don 2018

FacebookMonApi - An Arduino wrapper for the YouTube API

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


#ifndef FacebookMonApi_h
#define FacebookMonApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define FB_API_HOST "graph.facebook.com"
#define FB_API_SSL_PORT 443
#define FB_API_TIMEOUT 1500

class FacebookMonApi
{
  public:
    FacebookMonApi(Client &client, String accessToken) ;
    int getPageFanCount(String pageId) ;
	String getName(String pageId) ;
    String extendAccessToken(String pageId) ;
    String sendGetToFacebook(String command) ;

  private:
    Client *client ;
    String _accessToken ;
    const int maxMessageLength = 10000 ;
} ;

#endif
