# FacebookIOT
Facebook API for IOT (ESP8266)

Il s'agit d'une API utilisable dans les objets connectés pour s'interfacer avec Facebook et son API GRAPH.

La caractéristique de l'objet connecté est qu'il n'embarque pas de navigateur susceptible de réaliser une authentification à Facebook. Tout au plus dispose-t-il d'un client REST. 


L'objet connecté doit être enregistré sur Facebook (espace développeur, créer une application), en activant l'option :
Facebook Login -> Paramètres -> Connexion à partir des appareils 

On dispose alors d'un identifiant d'objet (**appId**) et d'un token client (**appToken**). Ces deux infrmations combinées forment un jeton d'accès (**accessToken**).

Pour initier une procédure d'authentification de l'objet connecté, il faut effectuer une requête POST (contenant le jeton d'accès et éventuellement la liste des privilèges demandés) dont la réponse contient les éléments suivants :
- un code (**authCode**)
- une invite (**authPrompt**)
- une URI (**authUri**)
- un délai d'expiration (**authExpire**)
- un délai inter-requêtes (**authDelay**)

Il faut ensuite afficher l'invite (**authPrompt**) à l'utilisateur et lui demander de se connecter sur l'URI (**authUri**) depuis son navigateur préféré et d'entrer l'invite. Le site Web procèera alors à son authentification (avec ses authentifiants Facebook) et lui fera valider les éventuels privilèges demandés par l'objet connecté.

Pendant ce temps, l'objet connecté va sonder le serveur d'autorisations pour récupérer le résultat de l'authentification de l'utilisateur. Ces reqêtes de type POST devront mentionner :
- le jeton d'acces (**accessToken**)
- le code (**authCode**)
Il faut respecter un delai minimum entre 2 interrogations (**authDelay**).
La réponse à ces requêtes peut avoir les significations suivantes :
- En attente d'authentification de l'utilisateur (il n'a pas fini sa procédure de connexion)
- Interrogation trop précoce (le délai n'est pas respecté)
- Demande expirée aprs le délai d'expiration (**authExpire**)
- Authentification réussie

Dans ce dernier cas, on récupère un jeton de session (**userToken**) permettant d'accéder à l'API GRAPH. On peut confirmer la réussite de l'authentification en affichant le nom de l'utilisateur connecté.