USE cleargaz;
DELIMITER $$




-- Insérer un Rapport

CREATE PROCEDURE InsertRapport(
    IN id_rapport VARCHAR(100),
    IN titre VARCHAR(150),
    IN date_publication DATE,
    IN analyse TEXT,
    IN nom_admin VARCHAR(100)
)
BEGIN
    DECLARE admin_id INT;

    SELECT Id_Personnel INTO admin_id FROM Personnel NATURAL JOIN Admins WHERE Nom_Personnel = nom_admin;

    INSERT INTO Rapports (Id_Rapport, Titre_Rapport, Date_Publication, Analyse, Id_Personnel)
    VALUES (id_rapport, titre, date_publication, analyse, admin_id);
END$$

-- Insérer une Donnée
CREATE PROCEDURE InsertDonnee(
    IN valeur FLOAT,
    IN date_donnee DATETIME,
    IN capteur_name VARCHAR(100),
    IN rapport_title VARCHAR(100)
)
BEGIN
    DECLARE capteur_id INT;
    DECLARE rapport_id VARCHAR(100);

    SELECT Id_Capteur INTO capteur_id
    FROM Capteurs
    WHERE Nom_Capteur = capteur_name;

    SELECT Id_Rapport INTO rapport_id
    FROM Rapports
    WHERE Titre_Rapport = rapport_title;

    INSERT INTO Données (Valeur_PPM, Date_Mesure, Id_Capteur, Id_Rapport)
    VALUES (valeur, date_donnee, capteur_id, rapport_id);
END$$



CREATE PROCEDURE DeleteRapportByTitle(
    IN rapport_title VARCHAR(150)
)
BEGIN
    DELETE FROM Rapports
    WHERE Titre_Rapport = rapport_title;
END$$


CREATE PROCEDURE DeleteDonnee(
    IN date_donnee DATETIME,
    IN capteur_name VARCHAR(100)
)
BEGIN
    DECLARE capteur_id INT;

    SELECT Id_Capteur INTO capteur_id
    FROM Capteurs
    WHERE Nom_Capteur = capteur_name;

    DELETE FROM Données
    WHERE Date_Mesure = date_donnee AND Id_Capteur = capteur_id;
END$$



CREATE PROCEDURE UpdateRapport(
    IN rapport_id VARCHAR(100),
    IN new_title VARCHAR(150),
    IN new_analyse TEXT
)
BEGIN
    UPDATE Rapports
    SET Titre_Rapport = new_title,
        Analyse = new_analyse
    WHERE Id_Rapport = rapport_id;
END$$


CREATE PROCEDURE UpdateDonnee(
    IN date_donnee DATETIME,
    IN capteur_name VARCHAR(100),
    IN new_valeur FLOAT
)
BEGIN
    DECLARE capteur_id INT;

    SELECT Id_Capteur INTO capteur_id
    FROM Capteurs
    WHERE Nom_Capteur = capteur_name;

    UPDATE Données
    SET Valeur_PPM = new_valeur
    WHERE Date_Mesure = date_donnee AND Id_Capteur = capteur_id;
END$$


DELIMITER ;



CALL InsertRapport('RPT078', 'Rapport 78', '2024-02-07', 'Analyse des gaz toxiques détectés.', 'Petit');


CALL InsertDonnee(345, '2024-02-07', 'Capteur_A', 'Rapport 78');






-- Disable safe updates for this session
SET SQL_SAFE_UPDATES = 0;

-- Update rapport analysis
CALL UpdateRapport('RPT078', 'Rapport 78 (MàJ)', 'Analyse mise à jour.');

-- Delete rapport by title
CALL DeleteRapportByTitle('Rapport 78');



-- Update data value
CALL UpdateDonnee('2024-02-07', 'Capteur_A', 298.5);

-- Delete data record
CALL DeleteDonnee('2024-02-07', 'Capteur_A');

-- (Optional) Re-enable safe updates
SET SQL_SAFE_UPDATES = 1;



