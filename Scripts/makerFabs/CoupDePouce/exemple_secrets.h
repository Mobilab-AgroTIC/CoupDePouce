// ===========================================================
//    FICHIER D'EXEMPLE : exemple_secrets.h
// ⚠ IMPORTANT : VOUS DEVEZ MODIFIER CE FICHIER ! Comment le modifier :
//
// 1. Faites un clic droit > Renommer ce fichier en : secrets.h 
// (il faut enlever le mot "exemple_" dans le nom)
//
// 2. Remplissez les infos ci-dessous (code PIN et numéros).
//
// 3. NE PARTAGEZ JAMAIS CE FICHIER PUBLIQUEMENT ! (il contient vos numéros et votre code SIM)
//
// ===========================================================

#pragma once

// Mets ton code PIN ci-dessous, ex: "1234". Laisse "" si pas de PIN.
#define SIM_PIN ""   

// Remplace les numéros ci-dessous par les numéros de téléphone (Sans espace, et au format international (+33 pour la France))
// Chaque numéro est entre guillemets "" et séparés par une virgule ",".
// Il peut y avoir 1 à 5 numéros. (après le dernier numéro, pas de virgule). 
// Si tu ne mets qu'un numéro, supprime le 2ème numéro "+336xxxxxxxx" inutile.

static const char* DESTS[] = {  
  "+336xxxxxxxx",               
  "+336xxxxxxxx"                
};
