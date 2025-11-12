-- Script to create VEHICULE table and sequence in Oracle database
-- Run this script as SYSTEM user or a user with CREATE TABLE privileges

-- Create sequence for ID_VEHI
CREATE SEQUENCE SEQ_VEHI
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOCYCLE;

-- Create VEHICULE table
CREATE TABLE "SYSTEM"."VEHICULE" (
    "ID_VEHI" NUMBER PRIMARY KEY,
    "IMMAT" VARCHAR2(20) NOT NULL UNIQUE,
    "MARQUE" VARCHAR2(50),
    "MODELE" VARCHAR2(50),
    "TYPE" VARCHAR2(50),
    "ETAT" VARCHAR2(20),
    "SERVICE" VARCHAR2(100),
    "DATE_MAINT" DATE
);

-- Create index on IMMAT for faster lookups
CREATE INDEX "IDX_VEHICULE_IMMAT" ON "SYSTEM"."VEHICULE"("IMMAT");

-- Grant permissions (adjust as needed)
-- GRANT SELECT, INSERT, UPDATE, DELETE ON "SYSTEM"."VEHICULE" TO your_user;

COMMIT;

