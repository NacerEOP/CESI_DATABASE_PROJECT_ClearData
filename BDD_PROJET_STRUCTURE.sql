DROP DATABASE IF EXISTS cleargaz;
CREATE DATABASE cleargaz;
USE cleargaz;

-- Tables
CREATE TABLE Gaz(
   Id_Gaz INT AUTO_INCREMENT,
   Nom_Gaz VARCHAR(50),
   PRIMARY KEY(Id_Gaz)
);

CREATE TABLE Région(
   Id_Région INT AUTO_INCREMENT,
   Nom_Région VARCHAR(50),
   PRIMARY KEY(Id_Région)
);

CREATE TABLE Ville(
   Id_Ville INT AUTO_INCREMENT,
   Id_Région INT NOT NULL,
   Nom_Ville VARCHAR(100),
   PRIMARY KEY(Id_Ville),
   FOREIGN KEY(Id_Région) REFERENCES Région(Id_Région)
);

CREATE TABLE Type_Gaz(
   Id_Type_Gaz INT AUTO_INCREMENT,
   Type VARCHAR(50),
   PRIMARY KEY(Id_Type_Gaz)
);

CREATE TABLE Agence(
   Id_Agence INT AUTO_INCREMENT,
   Nom_Agence VARCHAR(100),
   Adresse_Agence VARCHAR(200),
   Id_Ville INT NOT NULL,
   PRIMARY KEY(Id_Agence),
   FOREIGN KEY(Id_Ville) REFERENCES Ville(Id_Ville)
);

CREATE TABLE Personnel(
   Id_Personnel INT AUTO_INCREMENT,
   Nom_Personnel VARCHAR(100),
   Prenom_Personnel VARCHAR(100),
   Adresse_Personnel VARCHAR(200),
   Date_Prise_Poste DATE,
   Date_Naissance DATE,
   Id_Agence INT NOT NULL,
   PRIMARY KEY(Id_Personnel),
   FOREIGN KEY(Id_Agence) REFERENCES Agence(Id_Agence)
);

CREATE TABLE Admins(
   Id_Personnel INT,
   PRIMARY KEY(Id_Personnel),
   FOREIGN KEY(Id_Personnel) REFERENCES Personnel(Id_Personnel)
);

CREATE TABLE Technique(
   Id_Personnel INT,
   PRIMARY KEY(Id_Personnel),
   FOREIGN KEY(Id_Personnel) REFERENCES Personnel(Id_Personnel)
);

CREATE TABLE Chef(
   Id_Personnel INT,
   Diplome_Chef VARCHAR(100),
   Id_Agence INT NOT NULL,
   PRIMARY KEY(Id_Personnel),
   UNIQUE(Id_Agence),
   FOREIGN KEY(Id_Personnel) REFERENCES Personnel(Id_Personnel),
   FOREIGN KEY(Id_Agence) REFERENCES Agence(Id_Agence)
);

CREATE TABLE Capteur(
   Id_Capteur INT AUTO_INCREMENT,
   Nom_Capteur VARCHAR(50),
   Etat_Capteur BOOLEAN,
   Id_Ville INT NOT NULL,
   Id_Gaz INT NOT NULL,
   Id_Personnel INT NOT NULL,
   PRIMARY KEY(Id_Capteur),
   FOREIGN KEY(Id_Ville) REFERENCES Ville(Id_Ville),
   FOREIGN KEY(Id_Gaz) REFERENCES Gaz(Id_Gaz),
   FOREIGN KEY(Id_Personnel) REFERENCES Technique(Id_Personnel)
);

CREATE TABLE Rapport(
   Id_Rapport INT AUTO_INCREMENT,
   Titre_Rapport VARCHAR(150),
   Date_Publication DATE,
   Id_Personnel INT NOT NULL,
   PRIMARY KEY(Id_Rapport),
   FOREIGN KEY(Id_Personnel) REFERENCES Admins(Id_Personnel)
);

CREATE TABLE Donnée(
   Id_Donnée INT AUTO_INCREMENT,
   Date_Mesure DATE,
   Valeur_PPM DOUBLE,
   Id_Rapport INT NOT NULL,
   Id_Capteur INT NOT NULL,
   PRIMARY KEY(Id_Donnée),
   FOREIGN KEY(Id_Rapport) REFERENCES Rapport(Id_Rapport),
   FOREIGN KEY(Id_Capteur) REFERENCES Capteur(Id_Capteur)
);

CREATE TABLE Asso(
   Id_Gaz INT,
   Id_Type_Gaz INT,
   PRIMARY KEY(Id_Gaz, Id_Type_Gaz),
   FOREIGN KEY(Id_Gaz) REFERENCES Gaz(Id_Gaz),
   FOREIGN KEY(Id_Type_Gaz) REFERENCES Type_Gaz(Id_Type_Gaz)
);

