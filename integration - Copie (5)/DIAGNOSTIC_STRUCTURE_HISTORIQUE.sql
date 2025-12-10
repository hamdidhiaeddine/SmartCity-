-- ============================================================
-- DIAGNOSTIC DE LA STRUCTURE DE LA TABLE HISTORIQUE
-- ============================================================

PROMPT ============================================================
PROMPT STRUCTURE DE LA TABLE HISTORIQUE_RESIDENT
PROMPT ============================================================
DESC HIBA.HISTORIQUE_RESIDENT;

PROMPT
PROMPT ============================================================
PROMPT STRUCTURE DE LA TABLE HIST_RESIDENT
PROMPT ============================================================
DESC HIBA.HIST_RESIDENT;

PROMPT
PROMPT ============================================================
PROMPT COLONNES DE HISTORIQUE_RESIDENT
PROMPT ============================================================
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH, NULLABLE
FROM USER_TAB_COLUMNS
WHERE TABLE_NAME = 'HISTORIQUE_RESIDENT'
ORDER BY COLUMN_ID;

PROMPT
PROMPT ============================================================
PROMPT COLONNES DE HIST_RESIDENT
PROMPT ============================================================
SELECT COLUMN_NAME, DATA_TYPE, DATA_LENGTH, NULLABLE
FROM USER_TAB_COLUMNS
WHERE TABLE_NAME = 'HIST_RESIDENT'
ORDER BY COLUMN_ID;

PROMPT
PROMPT ============================================================
PROMPT DONNÉES DANS HIST_RESIDENT
PROMPT ============================================================
SELECT * FROM HIBA.HIST_RESIDENT;

PROMPT
PROMPT ============================================================
PROMPT FIN DU DIAGNOSTIC
PROMPT ============================================================
