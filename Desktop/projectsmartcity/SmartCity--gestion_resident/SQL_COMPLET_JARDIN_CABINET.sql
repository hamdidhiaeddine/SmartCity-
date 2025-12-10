-- ============================================================
-- SCRIPT SQL COMPLET POUR JARDIN ET CABINET
-- Base de données Oracle
-- SmartCity - Gestion Résident
-- ============================================================

-- ============================================================
-- 1. TABLE GEST_JARDIN (Gestion des Jardins)
-- ============================================================

-- Supprimer la table si elle existe
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE GEST_JARDIN CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Créer la table GEST_JARDIN
CREATE TABLE GEST_JARDIN (
    ID_JARDIN       NUMBER(10) PRIMARY KEY,
    NOM             VARCHAR2(100) NOT NULL,
    ADRESSE         VARCHAR2(255) NOT NULL,
    SUPERFICIE      NUMBER(10,2) NOT NULL,
    TYPE_JARDIN     VARCHAR2(50) NOT NULL,
    DATE_CREATION   DATE DEFAULT SYSDATE,
    CONSTRAINT chk_superficie_positive CHECK (SUPERFICIE > 0),
    CONSTRAINT chk_type_jardin CHECK (TYPE_JARDIN IN ('Public', 'Privé', 'Communautaire', 'Botanique', 'Potager'))
);

-- Commentaires sur la table
COMMENT ON TABLE GEST_JARDIN IS 'Table de gestion des jardins de la ville intelligente';
COMMENT ON COLUMN GEST_JARDIN.ID_JARDIN IS 'Identifiant unique du jardin';
COMMENT ON COLUMN GEST_JARDIN.NOM IS 'Nom du jardin (type de sol dans l''application)';
COMMENT ON COLUMN GEST_JARDIN.ADRESSE IS 'Emplacement/Adresse du jardin';
COMMENT ON COLUMN GEST_JARDIN.SUPERFICIE IS 'Superficie en mètres carrés';
COMMENT ON COLUMN GEST_JARDIN.TYPE_JARDIN IS 'Type de jardin: Public, Privé, Communautaire, Botanique, Potager';
COMMENT ON COLUMN GEST_JARDIN.DATE_CREATION IS 'Date de création de l''enregistrement';

-- Index pour optimiser les recherches
CREATE INDEX idx_jardin_type ON GEST_JARDIN(TYPE_JARDIN);
CREATE INDEX idx_jardin_nom ON GEST_JARDIN(NOM);

-- Séquence pour auto-incrément (optionnel)
CREATE SEQUENCE seq_jardin_id START WITH 1 INCREMENT BY 1 NOCACHE;

-- ============================================================
-- 2. TABLE GEST_CABINET (Gestion des Cabinets Médicaux)
-- ============================================================

-- Supprimer la table si elle existe
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE GEST_CABINET CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Créer la table GEST_CABINET
CREATE TABLE GEST_CABINET (
    ID              NUMBER(10) PRIMARY KEY,
    NOM             VARCHAR2(100) NOT NULL,
    ADRESSE         VARCHAR2(255) NOT NULL,
    SPECIALITE      VARCHAR2(100) NOT NULL,
    TELEPHONE       VARCHAR2(20) NOT NULL,
    EMAIL           VARCHAR2(100),
    ID_RESIDENT     NUMBER(10),
    DATE_CREATION   DATE DEFAULT SYSDATE,
    CONSTRAINT chk_email_format CHECK (EMAIL IS NULL OR REGEXP_LIKE(EMAIL, '^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$')),
    CONSTRAINT chk_telephone_format CHECK (REGEXP_LIKE(TELEPHONE, '^[0-9+\-\s()]{8,20}$'))
);

-- Commentaires sur la table
COMMENT ON TABLE GEST_CABINET IS 'Table de gestion des cabinets médicaux';
COMMENT ON COLUMN GEST_CABINET.ID IS 'Identifiant unique du cabinet';
COMMENT ON COLUMN GEST_CABINET.NOM IS 'Nom du cabinet médical';
COMMENT ON COLUMN GEST_CABINET.ADRESSE IS 'Adresse du cabinet';
COMMENT ON COLUMN GEST_CABINET.SPECIALITE IS 'Spécialité médicale';
COMMENT ON COLUMN GEST_CABINET.TELEPHONE IS 'Numéro de téléphone';
COMMENT ON COLUMN GEST_CABINET.EMAIL IS 'Adresse email';
COMMENT ON COLUMN GEST_CABINET.ID_RESIDENT IS 'ID du résident associé (optionnel)';

-- Index pour optimiser les recherches
CREATE INDEX idx_cabinet_specialite ON GEST_CABINET(SPECIALITE);
CREATE INDEX idx_cabinet_nom ON GEST_CABINET(NOM);
CREATE INDEX idx_cabinet_resident ON GEST_CABINET(ID_RESIDENT);

-- Séquence pour auto-incrément (optionnel)
CREATE SEQUENCE seq_cabinet_id START WITH 1 INCREMENT BY 1 NOCACHE;

-- ============================================================
-- 3. TABLE MAINTENANCE (Maintenance Prédictive des Jardins)
-- ============================================================

-- Supprimer la table si elle existe
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE MAINTENANCE CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Créer la table MAINTENANCE
CREATE TABLE MAINTENANCE (
    ID_MAINTENANCE      NUMBER(10) PRIMARY KEY,
    ID_JARDIN           NUMBER(10) NOT NULL,
    TYPE_MAINTENANCE    VARCHAR2(100) NOT NULL,
    DESCRIPTION         VARCHAR2(500),
    DATE_EFFECTUEE      DATE,
    DATE_RECOMMANDEE    DATE NOT NULL,
    STATUT              VARCHAR2(20) DEFAULT 'Prévu',
    FREQUENCE           VARCHAR2(50),
    DATE_CREATION       DATE DEFAULT SYSDATE,
    CONSTRAINT fk_maintenance_jardin FOREIGN KEY (ID_JARDIN) REFERENCES GEST_JARDIN(ID_JARDIN) ON DELETE CASCADE,
    CONSTRAINT chk_statut CHECK (STATUT IN ('Prévu', 'En cours', 'Terminé', 'Annulé', 'Reporté'))
);

-- Commentaires
COMMENT ON TABLE MAINTENANCE IS 'Table de maintenance prédictive des jardins';
COMMENT ON COLUMN MAINTENANCE.ID_MAINTENANCE IS 'Identifiant unique de la maintenance';
COMMENT ON COLUMN MAINTENANCE.ID_JARDIN IS 'Référence au jardin concerné';
COMMENT ON COLUMN MAINTENANCE.TYPE_MAINTENANCE IS 'Type: Arrosage, Taille, Fertilisation, Désherbage, etc.';
COMMENT ON COLUMN MAINTENANCE.DESCRIPTION IS 'Description détaillée de la maintenance';
COMMENT ON COLUMN MAINTENANCE.DATE_EFFECTUEE IS 'Date réelle d''exécution';
COMMENT ON COLUMN MAINTENANCE.DATE_RECOMMANDEE IS 'Date recommandée pour la maintenance';
COMMENT ON COLUMN MAINTENANCE.STATUT IS 'Statut: Prévu, En cours, Terminé, Annulé, Reporté';
COMMENT ON COLUMN MAINTENANCE.FREQUENCE IS 'Fréquence: Quotidien, Hebdomadaire, Mensuel, Trimestriel, Annuel';

-- Index
CREATE INDEX idx_maintenance_jardin ON MAINTENANCE(ID_JARDIN);
CREATE INDEX idx_maintenance_date ON MAINTENANCE(DATE_RECOMMANDEE);
CREATE INDEX idx_maintenance_statut ON MAINTENANCE(STATUT);

-- Séquence
CREATE SEQUENCE seq_maintenance_id START WITH 1 INCREMENT BY 1 NOCACHE;

-- ============================================================
-- 4. TABLE RECOMMANDATION_PLANTE (Recommandations de Biodiversité)
-- ============================================================

-- Supprimer la table si elle existe
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE RECOMMANDATION_PLANTE CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Créer la table RECOMMANDATION_PLANTE
CREATE TABLE RECOMMANDATION_PLANTE (
    ID_PLANTE       NUMBER(10) PRIMARY KEY,
    TYPE_SOL        VARCHAR2(100) NOT NULL,
    RECOMMANDATION  VARCHAR2(500) NOT NULL,
    NOM_PLANTE      VARCHAR2(100),
    COMPATIBILITE   VARCHAR2(50) DEFAULT 'Moyenne',
    SAISON          VARCHAR2(50),
    DATE_CREATION   DATE DEFAULT SYSDATE,
    CONSTRAINT chk_compatibilite CHECK (COMPATIBILITE IN ('Excellente', 'Bonne', 'Moyenne', 'Faible'))
);

-- Commentaires
COMMENT ON TABLE RECOMMANDATION_PLANTE IS 'Recommandations de plantes selon le type de sol';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.ID_PLANTE IS 'Identifiant unique de la recommandation';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.TYPE_SOL IS 'Type de sol: Argileux, Sableux, Limoneux, Calcaire, Humifère';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.RECOMMANDATION IS 'Description de la recommandation';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.NOM_PLANTE IS 'Nom de la plante recommandée';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.COMPATIBILITE IS 'Niveau de compatibilité avec le sol';
COMMENT ON COLUMN RECOMMANDATION_PLANTE.SAISON IS 'Saison de plantation recommandée';

-- Index
CREATE INDEX idx_recommandation_sol ON RECOMMANDATION_PLANTE(TYPE_SOL);
CREATE INDEX idx_recommandation_compat ON RECOMMANDATION_PLANTE(COMPATIBILITE);

-- Séquence
CREATE SEQUENCE seq_recommandation_id START WITH 1 INCREMENT BY 1 NOCACHE;

-- ============================================================
-- 5. DONNÉES D'EXEMPLE - JARDINS
-- ============================================================

INSERT INTO GEST_JARDIN (ID_JARDIN, NOM, ADRESSE, SUPERFICIE, TYPE_JARDIN) VALUES 
(1, 'Sol Argileux', 'Parc Central - Zone Nord', 5000.00, 'Public');

INSERT INTO GEST_JARDIN (ID_JARDIN, NOM, ADRESSE, SUPERFICIE, TYPE_JARDIN) VALUES 
(2, 'Sol Sableux', 'Quartier Résidentiel Est', 1200.50, 'Communautaire');

INSERT INTO GEST_JARDIN (ID_JARDIN, NOM, ADRESSE, SUPERFICIE, TYPE_JARDIN) VALUES 
(3, 'Sol Limoneux', 'Avenue des Fleurs 45', 800.00, 'Potager');

INSERT INTO GEST_JARDIN (ID_JARDIN, NOM, ADRESSE, SUPERFICIE, TYPE_JARDIN) VALUES 
(4, 'Sol Calcaire', 'Boulevard Principal 120', 3500.75, 'Botanique');

INSERT INTO GEST_JARDIN (ID_JARDIN, NOM, ADRESSE, SUPERFICIE, TYPE_JARDIN) VALUES 
(5, 'Sol Humifère', 'Résidence Les Oliviers', 450.00, 'Privé');

-- ============================================================
-- 6. DONNÉES D'EXEMPLE - CABINETS
-- ============================================================

INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL) VALUES 
(1, 'Cabinet Dr. Martin', '15 Rue de la Santé', 'Médecine Générale', '01 23 45 67 89', 'dr.martin@sante.fr');

INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL) VALUES 
(2, 'Clinique Dentaire Sourire', '28 Avenue du Soleil', 'Dentiste', '01 98 76 54 32', 'contact@sourire.fr');

INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL) VALUES 
(3, 'Cabinet Pédiatrie Enfants', '5 Place des Enfants', 'Pédiatrie', '01 11 22 33 44', 'pediatrie@enfants.fr');

INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL) VALUES 
(4, 'Centre Cardiologie', '100 Boulevard Coeur', 'Cardiologie', '01 55 66 77 88', 'cardio@coeur.fr');

INSERT INTO GEST_CABINET (ID, NOM, ADRESSE, SPECIALITE, TELEPHONE, EMAIL) VALUES 
(5, 'Cabinet Dermatologie Peau', '22 Rue de la Beauté', 'Dermatologie', '01 44 33 22 11', 'dermo@peau.fr');

-- ============================================================
-- 7. DONNÉES D'EXEMPLE - MAINTENANCE
-- ============================================================

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(1, 1, 'Arrosage', 'Arrosage automatique zone nord', SYSDATE + 7, 'Prévu', 'Hebdomadaire');

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(2, 1, 'Taille', 'Taille des haies principales', SYSDATE + 30, 'Prévu', 'Mensuel');

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(3, 2, 'Fertilisation', 'Apport d''engrais organique', SYSDATE + 14, 'Prévu', 'Mensuel');

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(4, 3, 'Désherbage', 'Désherbage manuel du potager', SYSDATE + 3, 'Prévu', 'Hebdomadaire');

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(5, 4, 'Plantation', 'Plantation de nouvelles espèces', SYSDATE + 60, 'Prévu', 'Trimestriel');

INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_EFFECTUEE, DATE_RECOMMANDEE, STATUT, FREQUENCE) VALUES 
(6, 5, 'Arrosage', 'Arrosage manuel jardin privé', SYSDATE - 1, SYSDATE - 1, 'Terminé', 'Quotidien');

-- ============================================================
-- 8. DONNÉES D'EXEMPLE - RECOMMANDATIONS PLANTES
-- ============================================================

-- Sol Argileux
INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(1, 'Argileux', 'Rose', 'Idéale pour sols argileux, nécessite un bon drainage', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(2, 'Argileux', 'Iris', 'Supporte bien l''humidité des sols argileux', 'Bonne', 'Automne');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(3, 'Argileux', 'Saule', 'Arbre parfait pour sols lourds et humides', 'Excellente', 'Printemps');

-- Sol Sableux
INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(4, 'Sableux', 'Lavande', 'Excellente drainage, supporte la sécheresse', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(5, 'Sableux', 'Romarin', 'Aromatique méditerranéen idéal', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(6, 'Sableux', 'Cactus', 'Parfait pour sols très drainants', 'Excellente', 'Été');

-- Sol Limoneux
INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(7, 'Limoneux', 'Tomate', 'Sol idéal pour les légumes', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(8, 'Limoneux', 'Carotte', 'Légume racine parfait pour sol meuble', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(9, 'Limoneux', 'Fraisier', 'Fruit délicieux en sol fertile', 'Bonne', 'Printemps');

-- Sol Calcaire
INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(10, 'Calcaire', 'Clématite', 'Grimpante qui aime le calcaire', 'Excellente', 'Automne');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(11, 'Calcaire', 'Lilas', 'Arbuste parfumé pour sols alcalins', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(12, 'Calcaire', 'Buis', 'Arbuste persistant classique', 'Bonne', 'Printemps');

-- Sol Humifère
INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(13, 'Humifère', 'Hortensia', 'Adore les sols riches en humus', 'Excellente', 'Printemps');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(14, 'Humifère', 'Rhododendron', 'Arbuste acidophile parfait', 'Excellente', 'Automne');

INSERT INTO RECOMMANDATION_PLANTE (ID_PLANTE, TYPE_SOL, NOM_PLANTE, RECOMMANDATION, COMPATIBILITE, SAISON) VALUES 
(15, 'Humifère', 'Fougère', 'Plante d''ombre pour sol humide', 'Excellente', 'Printemps');

-- ============================================================
-- 9. VUES UTILES
-- ============================================================

-- Vue des maintenances à venir
CREATE OR REPLACE VIEW V_MAINTENANCE_A_VENIR AS
SELECT 
    m.ID_MAINTENANCE,
    j.NOM AS TYPE_SOL,
    j.ADRESSE AS EMPLACEMENT,
    m.TYPE_MAINTENANCE,
    m.DESCRIPTION,
    m.DATE_RECOMMANDEE,
    m.STATUT,
    m.FREQUENCE,
    ROUND(m.DATE_RECOMMANDEE - SYSDATE) AS JOURS_RESTANTS
FROM MAINTENANCE m
JOIN GEST_JARDIN j ON m.ID_JARDIN = j.ID_JARDIN
WHERE m.DATE_RECOMMANDEE >= SYSDATE
  AND m.STATUT IN ('Prévu', 'En cours')
ORDER BY m.DATE_RECOMMANDEE;

-- Vue des recommandations par jardin
CREATE OR REPLACE VIEW V_RECOMMANDATION_JARDIN AS
SELECT 
    j.ID_JARDIN,
    j.NOM AS TYPE_SOL,
    j.ADRESSE AS EMPLACEMENT,
    r.NOM_PLANTE,
    r.RECOMMANDATION,
    r.COMPATIBILITE,
    r.SAISON
FROM GEST_JARDIN j
LEFT JOIN RECOMMANDATION_PLANTE r ON UPPER(j.NOM) LIKE '%' || UPPER(r.TYPE_SOL) || '%'
ORDER BY j.ID_JARDIN, r.COMPATIBILITE;

-- Vue statistiques cabinets par spécialité
CREATE OR REPLACE VIEW V_STATS_CABINET AS
SELECT 
    SPECIALITE,
    COUNT(*) AS NOMBRE_CABINETS,
    ROUND(COUNT(*) * 100.0 / (SELECT COUNT(*) FROM GEST_CABINET), 2) AS POURCENTAGE
FROM GEST_CABINET
GROUP BY SPECIALITE
ORDER BY NOMBRE_CABINETS DESC;

-- ============================================================
-- 10. PROCÉDURES STOCKÉES
-- ============================================================

-- Procédure pour générer les maintenances périodiques
CREATE OR REPLACE PROCEDURE SP_GENERER_MAINTENANCE_PERIODIQUE(
    p_id_jardin IN NUMBER,
    p_type_maintenance IN VARCHAR2,
    p_frequence IN VARCHAR2,
    p_description IN VARCHAR2
) AS
    v_date_prochaine DATE;
    v_next_id NUMBER;
BEGIN
    -- Calculer la prochaine date selon la fréquence
    CASE p_frequence
        WHEN 'Quotidien' THEN v_date_prochaine := SYSDATE + 1;
        WHEN 'Hebdomadaire' THEN v_date_prochaine := SYSDATE + 7;
        WHEN 'Mensuel' THEN v_date_prochaine := ADD_MONTHS(SYSDATE, 1);
        WHEN 'Trimestriel' THEN v_date_prochaine := ADD_MONTHS(SYSDATE, 3);
        WHEN 'Annuel' THEN v_date_prochaine := ADD_MONTHS(SYSDATE, 12);
        ELSE v_date_prochaine := SYSDATE + 7;
    END CASE;
    
    -- Obtenir le prochain ID
    SELECT NVL(MAX(ID_MAINTENANCE), 0) + 1 INTO v_next_id FROM MAINTENANCE;
    
    -- Insérer la maintenance
    INSERT INTO MAINTENANCE (ID_MAINTENANCE, ID_JARDIN, TYPE_MAINTENANCE, DESCRIPTION, DATE_RECOMMANDEE, STATUT, FREQUENCE)
    VALUES (v_next_id, p_id_jardin, p_type_maintenance, p_description, v_date_prochaine, 'Prévu', p_frequence);
    
    COMMIT;
END;
/

-- ============================================================
-- 11. TRIGGERS
-- ============================================================

-- Trigger pour auto-générer la prochaine maintenance après complétion
CREATE OR REPLACE TRIGGER TRG_MAINTENANCE_COMPLETE
AFTER UPDATE OF STATUT ON MAINTENANCE
FOR EACH ROW
WHEN (NEW.STATUT = 'Terminé' AND OLD.STATUT != 'Terminé' AND NEW.FREQUENCE IS NOT NULL)
BEGIN
    SP_GENERER_MAINTENANCE_PERIODIQUE(
        :NEW.ID_JARDIN,
        :NEW.TYPE_MAINTENANCE,
        :NEW.FREQUENCE,
        :NEW.DESCRIPTION
    );
END;
/

-- ============================================================
-- 12. VALIDATION
-- ============================================================

-- Vérifier les tables créées
SELECT table_name FROM user_tables WHERE table_name IN ('GEST_JARDIN', 'GEST_CABINET', 'MAINTENANCE', 'RECOMMANDATION_PLANTE');

-- Compter les enregistrements
SELECT 'GEST_JARDIN' AS TABLE_NAME, COUNT(*) AS NB_RECORDS FROM GEST_JARDIN
UNION ALL
SELECT 'GEST_CABINET', COUNT(*) FROM GEST_CABINET
UNION ALL
SELECT 'MAINTENANCE', COUNT(*) FROM MAINTENANCE
UNION ALL
SELECT 'RECOMMANDATION_PLANTE', COUNT(*) FROM RECOMMANDATION_PLANTE;

COMMIT;

-- ============================================================
-- FIN DU SCRIPT
-- ============================================================
