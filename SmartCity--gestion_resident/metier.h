// metier.h
#ifndef METIER_H
#define METIER_H

#include <QString>
#include <QStringList>
#include <QSize>

// Business logic / helper utilities for the project
class Metier {
public:
    // Construct with project root path (used to resolve relative paths). If empty, paths are used as-is.
    explicit Metier(const QString &projectRoot = {});
    // Initialize default file location for path list (sets the file to QDir::currentPath()/fileName)
    void initializeFilePath(const QString &fileName = "path.txt");

    // Save current loaded video paths back to the given file (or the initialized file when empty)
    bool savePaths(const QString &pathFile = QString());

    // Return the file path used by initializeFilePath
    QString mediaListFilePath() const;
    // Ensure the project's vd/ files are copied to the current working directory's vd/ folder.
    // Returns true if at least one file is present or copied into workingDir/vd.
    bool ensureVdInWorkingDir() const;

    // Load video paths from a text file (one path per line). Lines can be either:
    // - full or relative paths
    // - name:path (e.g. cabinet1:vd/cabinet1.mp4)
    // Returns true on success (file opened), false otherwise.
    bool loadPaths(const QString &pathFile = "path.txt");

    // Accessors
    QStringList videoPaths() const;
    QString videoPathForIndex(int index) const; // returns empty string if out of range
    // Return an existing absolute path for a stored path index by checking several places
    // Return an existing absolute path for a stored path index by checking several places.
    // If 'tried' is provided it will be filled with candidate paths that were checked.
    QString resolvedVideoPathForIndex(int index, QStringList *tried = nullptr) const;
    QStringList cabinetNames() const; // derived names (cabinet1, cabinet2...) or filename bases

    // Générer une carte de visite professionnelle du docteur (image PNG/JPEG)
    // Inclut nom, spécialité, cabinet, email, téléphone, adresse
    bool addDoctorCard(const QString &doctorName,
                       const QString &specialite,
                       const QString &cabinetName,
                       const QString &email,
                       const QString &telephone,
                       const QString &adresse,
                       const QString &outputImagePath,
                       const QSize &size = QSize(850, 500));

    // Générer une carte de visite professionnelle en PDF
    bool addDoctorPdf(const QString &doctorName,
                      const QString &specialite,
                      const QString &cabinetName,
                      const QString &email,
                      const QString &telephone,
                      const QString &adresse,
                      const QString &outputPdfPath,
                      const QSize &pageSize = QSize(595, 842)); // default A4 points

private:
    QStringList m_paths; // list of video paths
    QStringList m_names; // parallel list of cabinet names
    QString m_projectRoot; // optional project root used to resolve relative paths
    QString m_pathFile; // path file used for save/load when not specified
    void clear();
};

#endif // METIER_H
