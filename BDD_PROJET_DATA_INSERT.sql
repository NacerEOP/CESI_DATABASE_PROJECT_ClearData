-- Data Insertion

USE cleargaz;
INSERT INTO Région (Nom_Region) VALUES 
('Île-de-France'), ('Provence-Alpes-Côte d\'Azur'), ('Nouvelle-Aquitaine');

INSERT INTO Ville (Id_Région,Nom_Ville) VALUES 
(1, 'Paris'), 
(1, 'Versailles'), 
(2, 'Marseille'), 
(2, 'Nice'), 
(3, 'Bordeaux'), 
(3, 'La Rochelle');

INSERT INTO Gaz (Nom_Gaz) VALUES 
('CO2'), ('CH4'), ('NO2');

INSERT INTO Type_Gaz (Type) VALUES 
('Greenhouse'), ('Toxic'), ('Flammable');

INSERT INTO Asso (Id_Gaz, Id_Type_Gaz) VALUES 
(1, 1), (2, 3), (3, 2);

INSERT INTO Agence (Nom_Agence, Adresse_Agence, Id_Ville) VALUES 
('Agence Nord', '12 Rue de la Paix, Paris', 1),
('Agence Sud', '88 Avenue Lumière, Marseille', 2),
('Agence Ouest', '5 Boulevard Atlantique, Bordeaux', 3);

INSERT INTO Personnel (Nom_Personnel, Prenom_Personnel, Adresse_Personnel, Date_Prise_Poste, Date_Naissance, Id_Agence) VALUES 
('Durand', 'Luc', '10 Rue Victor Hugo', '2020-03-15', '1980-07-10', 1),
('Martin', 'Sophie', '25 Place des Fleurs', '2021-06-20', '1990-11-22', 2),
('Bernard', 'Antoine', '7 Allée des Tilleuls', '2019-09-01', '1978-02-14', 3);

-- Admins
INSERT INTO Admins (Id_Personnel) VALUES (1);

-- Technique
INSERT INTO Technique (Id_Personnel) VALUES (2);

-- Chef
INSERT INTO Chef (Id_Personnel, Diplome_Chef, Id_Agence) VALUES 
(3, 'Master Gestion Environnementale', 3);

-- Capteur
INSERT INTO Capteur (Nom_Capteur, Etat_Capteur, Id_Ville, Id_Gaz, Id_Personnel) VALUES 
('Capteur Alpha', TRUE, 1, 1, 2),
('Capteur Beta', FALSE, 2, 2, 2),
('Capteur Gamma', TRUE, 3, 3, 2);

-- Rapport
INSERT INTO Rapport (Titre_Rapport, Date_Publication, Id_Personnel) VALUES 
('Rapport Pollution Mars', '2024-03-10', 1),
('Rapport Qualité Air Février', '2024-02-05', 1);

-- Donnée
INSERT INTO Donnée (Date_Mesure, Valeur_PPM, Id_Rapport, Id_Capteur) VALUES 
('2024-03-09', 412.5, 1, 1),
('2024-03-10', 399.8, 1, 2),
('2024-02-04', 275.6, 2, 3);

commit;