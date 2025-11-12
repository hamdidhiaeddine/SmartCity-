# Jardin Module - Quick Reference Guide

## Button Mappings (UI Object Names → Operations)

| Button Name | Operation | Function |
|-------------|-----------|----------|
| `ajouter_8` | **ADD** / Ajout | Add a new garden to the database |
| `modifier_8` | **EDIT** / Modifier | Modify/Update an existing garden |
| `supprimer_7` | **DELETE** / Supprimer | Delete a garden from the database |
| (No button) | **EXPORT** | Export table to PDF (can be added) |

## Input Fields (UI Object Names)

| Field Name | Purpose | Type |
|------------|---------|------|
| `idjardinline` | Garden ID (Primary Key) | QLineEdit |
| `emplacementline` | Garden location/placement | QLineEdit |
| `superficieline` | Garden area/size (first input) | QLineEdit |
| `superficieline_2` | Soil type (second area input) | QLineEdit |
| `typechoix` | Garden type choice | QComboBox |

## Database Mapping

### Table: JARDINS

```sql
CREATE TABLE JARDINS (
    IDJARDIN NUMBER PRIMARY KEY,
    EMPLACEMENT VARCHAR2(255),
    SUPERFICIE VARCHAR2(100),
    TYPESOL VARCHAR2(100),
    TYPECHOIX VARCHAR2(100)
);
```

### SQL Operations Used

1. **SELECT**: `SELECT IDJARDIN, EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX FROM JARDINS ORDER BY IDJARDIN`
2. **INSERT**: `INSERT INTO JARDINS (IDJARDIN, EMPLACEMENT, SUPERFICIE, TYPESOL, TYPECHOIX) VALUES (:id, :emp, :sup, :ts, :tc)`
3. **UPDATE**: `UPDATE JARDINS SET EMPLACEMENT = :emp, SUPERFICIE = :sup, TYPESOL = :ts, TYPECHOIX = :tc WHERE IDJARDIN = :id`
4. **DELETE**: `DELETE FROM JARDINS WHERE IDJARDIN = :id`

## Display Table

| Column | Header | Data Source |
|--------|--------|-------------|
| 0 | ID | IDJARDIN |
| 1 | Emplacement | EMPLACEMENT |
| 2 | Superficie | SUPERFICIE |
| 3 | TypeSol | TYPESOL |
| 4 | TypeChoix | TYPECHOIX |

## User Flow

### Adding a Garden
1. Fill in all fields (ID, Emplacement, Superficie, TypeSol, TypeChoix)
2. Click `ajouter_8` button
3. Confirm dialog appears (for delete only)
4. Garden added to database
5. Form clears automatically
6. Table refreshes to show new entry

### Editing/Modifying a Garden
1. Enter the ID of the garden to modify
2. Fill in the new values for other fields
3. Click `modifier_8` button
4. Confirm dialog appears (for delete only)
5. Garden updated in database
6. Form clears automatically
7. Table refreshes to show updated entry

### Deleting a Garden
1. Enter the ID of the garden to delete
2. Click `supprimer_7` button
3. **Confirmation dialog appears** (UNIQUE to delete operation)
4. Click "Yes" to confirm deletion
5. Garden deleted from database
6. Form clears automatically
7. Table refreshes to show deletion

### Exporting to PDF
- PDF export includes:
  - Title: "Liste des Jardins"
  - Formatted headers and data
  - Professional layout with borders
  - Proportional column widths

## Error Handling

### Common Errors & Solutions

| Error | Cause | Solution |
|-------|-------|----------|
| "ID Jardin est obligatoire" | Empty ID field | Fill in the ID field |
| Database error with SQL code | Database connection issue | Check database connection |
| Failed export | No data in table | Add gardens first |
| Widget not found | UI element missing | Verify UI names match code |

## Code Entry Points

### Main Methods
- `MainWindow::connectJardinUi()` - Connects buttons to slots
- `MainWindow::refreshJardinTable()` - Loads gardens from DB
- `MainWindow::onAjouterJardin()` - Add operation
- `MainWindow::onModifierJardin()` - Update operation
- `MainWindow::onSupprimerJardin()` - Delete operation
- `MainWindow::onExporterJardin()` - PDF export
- `MainWindow::onGestionJardins()` - Tab switch handler

### Constructor Flow
1. `MainWindow::MainWindow()`
2. → `connectButtons()` - Connects all module buttons
3. → `connectJardinUi()` - Wires Jardin buttons
4. → `refreshJardinTable()` - Loads initial data

## Attributes Description

| Attribute | Description | Example |
|-----------|-------------|---------|
| IDJARDIN | Unique garden identifier | 1, 2, 3 |
| EMPLACEMENT | Where the garden is located | "Nord-Est", "Entrée principale" |
| SUPERFICIE | Size/area of the garden | "50m²", "Petite" |
| TYPESOL | Type of soil in the garden | "Argileux", "Sableux", "Riche" |
| TYPECHOIX | Type of garden selected | "Potager", "Ornemental", "Mixte" |

## Testing Checklist

- [ ] Add a new garden with all fields
- [ ] Verify garden appears in table
- [ ] Edit existing garden
- [ ] Verify changes in table
- [ ] Delete a garden with confirmation
- [ ] Verify deletion in table
- [ ] Export to PDF
- [ ] Verify PDF generates correctly
- [ ] Check empty field validation
- [ ] Test database error handling
