# CoupDePouce

Le projet permet d'activer un mécansime à distance, via la construction d'un 'doigt connecté', par envoi de SMS. Le système total coûte environ 60€

# 1. Construction du système CoupDePouce

## 1.1. Matériel nécessaire

CoupDePouce nécéssite : 
  - une carte Arduino [MKR GSM 1400](https://www.kubii.fr/26-arduino/2911-arduino-mkr-gsm-1400-3272496301078.html) ou [NB 1500](https://www.gotronic.fr/art-carte-arduino-mkr-nb-1500-abx00019-28733.htm) : 40€
  - Une antenne associée
  - Un forfait téléphonique le moins cher possible : le site [monpetitforfait](https://www.monpetitforfait.com/comparatif-forfaits-mobiles-2-euros) propose les bons plans du moment, il est assez facile de trouver un forfait à 2€/mois chez [Prixtel](https://www.prixtel.com/?gclid=CjwKCAjwv-GUBhAzEiwASUMm4guqNUokI6BJ6zby10jOds1HJ0ug_529WNJx9wK_Wr-zYUhOZG7cFxoCU4IQAvD_BwE) ou [Syma](https://www.symamobile.com/forfait?path=forfait.html&id_affliator=zydbwxbqpg&tag_source=COMPARATEUR#to_enregitrement_tab). Free propose un forfait à 0€/mois si vous êtes déja client chez eux.
  - un moteur "pas à pas" [28byj-48](https://fr.aliexpress.com/item/32906617266.html?spm=a2g0o.productlist.0.0.45025be5Rf1dDb&algo_pvid=9b464eba-f000-4a8b-9fe3-bf1fcc171f3f&algo_exp_id=9b464eba-f000-4a8b-9fe3-bf1fcc171f3f-0&pdp_ext_f=%7B%22sku_id%22%3A%2212000027293392609%22%7D&pdp_npi=2%40dis%21EUR%21%211.76%21%21%212.11%21%21%402100bde716527777656226226ee080%2112000027293392609%21sea) avec sa carte ULN2003 (inclue) : 2€
  - Un [cable micro USB data](https://fr.aliexpress.com/item/32958208619.html?spm=a2g0o.productlist.0.0.22da7e26bJZ9jx&algo_pvid=568f939d-50da-48d5-9a96-0d9570edc883&algo_exp_id=568f939d-50da-48d5-9a96-0d9570edc883-1&pdp_ext_f=%7B%22sku_id%22%3A%2212000025661347345%22%7D&pdp_npi=2%40dis%21EUR%21%212.35%21%21%21%21%21%402100bdd816527785667561725ecad3%2112000025661347345%21sea) (Attention : si vous avez déja ce cable pour votre téléphone ou autre, il faut être bien sûr que l'on peut echanger de la donnée via ce câble (data sync), et pas uniquement recharger)
  - Un [adaptateur 220V/USB](https://fr.aliexpress.com/item/1005002021554317.html?spm=a2g0o.productlist.0.0.11b77dfep4et0p&algo_pvid=598c855d-20a3-461b-b7d4-edb555703ac4&algo_exp_id=598c855d-20a3-461b-b7d4-edb555703ac4-1&pdp_ext_f=%7B%22sku_id%22%3A%2212000018440472990%22%7D&pdp_npi=2%40dis%21EUR%21%211.63%21%21%211.66%21%21%402100bdcf16527870083772904ebb7f%2112000018440472990%21sea) (vous en avez peut-être un qui traine chez vous...?)
  - des [cables Dupont Femelle-Femelle](https://fr.aliexpress.com/item/4000848184096.html?spm=a2g0o.productlist.0.0.741a6a6dmE4jt8&algo_pvid=aedfdd9c-d065-4b2b-8132-cb3ce46ef6ba&algo_exp_id=aedfdd9c-d065-4b2b-8132-cb3ce46ef6ba-2&pdp_ext_f=%7B%22sku_id%22%3A%2210000009393531605%22%7D&pdp_npi=2%40dis%21EUR%21%211.69%21%21%210.26%21%21%402100bdd716527787087616523e02b5%2110000009393531605%21sea) Bien choisir le cable femelle-femelle de 40cm, comme le montre la photo 1 : 

  ![image](https://user-images.githubusercontent.com/24956276/168775555-40b71cad-7cf6-4dff-9ea0-ebbd9e430b6c.png)

## 1.2. Montage du système 

# 2. Installation et paramétrage du logiciel Arduino
## 2.1. Installation d'Arduino

Si ceci n'est pas encore fait, il faut au préalable installer le [logiciel Arduino](https://www.arduino.cc/en/software) sur son ordinateur. Choisissez l'installation pour votre ordinateur (Windows, linux, mac, etc) comme le montre la photo 2 :

![image](https://user-images.githubusercontent.com/24956276/168775993-2691511c-9ea9-4fee-9f0e-90afb1747dbb.png)
L'installation vous posera quelques questions, dites "oui" à tout.

## 2.2. Paramétrage d'Arduino pour la carte MKR GSM 1400 ou NB 1500
Une fois installé, on lance le logiciel Arduino. Une fenêtre s'ouvre.

![image](https://user-images.githubusercontent.com/24956276/168776482-0613947c-0370-4ca8-819b-ab0c045d7667.png)

On ouvre Outils > Type de cartes > Gestionnaire de Cartes

![image](https://user-images.githubusercontent.com/24956276/168776797-24dedbd7-e37d-494b-a5fc-c4302abd9980.png)

On recherce depuis la barre de recherche MKR. Une librairie est trouvée : "Arduino SAMD Boards". On clique sur Installer. L'installation peut prendre 1min.

![image](https://user-images.githubusercontent.com/24956276/168777036-4c035800-1c95-4f91-ac7e-8996d260b482.png)

On selectionne finalement la carte utilisée dans outils > Type de Carte > Arduino SAMD > Arduino MKR GSM 1400 : 

![image](https://user-images.githubusercontent.com/24956276/168798769-1dd4935a-432c-4c87-a512-c35bd6eb8f39.png)

Tout est configuré ! 

# Flash du code sur l'Arduino
On va maintenant installer le code sur la carte qui permettra de contrôler le moteur et l'utilisation de SMS.

Sur cette page Github, vous pouvez télécharger le code de CoupDePouce en cliquant sur le bouton vert Code > Download ZIP.
![image](https://user-images.githubusercontent.com/24956276/168778176-622ee3c4-dfb0-4f59-bc3c-572b62e82df3.png)

Le fichier téléchargé est à dézipper : 
![image](https://user-images.githubusercontent.com/24956276/168778462-85c77ce9-071b-4fb1-88b8-e2819b0d9c49.png)

Ouvrez ensuite CoupDePouce-main > Scripts > Arduino > CoupDePouce_1moteur > CoupDePouce_1moteur.ino 
![image](https://user-images.githubusercontent.com/24956276/168778694-88b776e7-f3f9-46e8-9a3d-7a6f8aac5c0a.png). Une fenêtre Arduino s'ouvre. Notez qu'il y a deux onglets : 
CoupDePouce_1moteur.ino et arduino_secrets.h. Dans arduino_secrets.h, vous pouvez renseigner votre code PIN ainsi que le numéro de téléphone de 2 personnes qui recevront le message d'initialisation.

![image](https://user-images.githubusercontent.com/24956276/168793924-a50748bb-0edb-4f66-8bf8-55dd8b184129.png)

Le code est maintenant prêt à être envoyé sur la carte. Branchez l'arduino à l'ordinateur via la câble USB, puis allez dans Outils > Ports > COMxx. Il faut séléctionner le bon port COM, si vous en avez plusieurs à sipsosition, débranchez la carte Arduino et retournez dans cet onglet pour voir lequel a disparu. C'est celui la qu'il faudra prendre une fois quand vous l'aurez rebranché ;)
![image](https://user-images.githubusercontent.com/24956276/168798673-c2b3ea28-46b7-49b5-a0cc-5f867a747a61.png)

Cliquez finalement sur le bouton "Téléverser" (en blanc sur l'image) : 

![image](https://user-images.githubusercontent.com/24956276/168799435-aab4b449-4f5c-4a61-bc32-bd5839dc17b6.png)

Vous pouvez débrancher l'Arduino, on passe au montage.

# Montage du système

Le montage éléctronique est très simple. Il faut impérativement brancher l'antenne sur la carte, puis les câbles comme suit :

MOTEUR <-> ARDUINO

- **"-"** <-> **pin GND**
- **"5V"** <-> **pin VCC**
- **IN1** <-> **pin 6**
- **IN2** <-> **pin 7**
- **IN3** <-> **pin 8**
- **IN4** <-> **pin 9**

Vous pouvez maintenant tester le système en le branchant sur le secteur via l'adaptateur
