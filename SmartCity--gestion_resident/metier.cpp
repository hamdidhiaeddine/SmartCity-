// metier.cpp
#include "metier.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QDate>
#include <QCoreApplication>
#include <QDirIterator>
#include <QDebug>
#include <QTextDocument>
#include <QPdfWriter>
#include <QLocale>
#include <QPageSize>

#include <QFile>

Metier::Metier(const QString &projectRoot)
    : m_projectRoot(projectRoot)
{
}

void Metier::initializeFilePath(const QString &fileName)
{
    QString dataPath = QDir::currentPath();
    if (!QDir(dataPath).exists())
        QDir().mkpath(dataPath);
    m_pathFile = QDir(dataPath).filePath(fileName);
    qDebug() << "Metier::initializeFilePath() - media list file path:" << m_pathFile;
}

QString Metier::mediaListFilePath() const
{
    if (!m_pathFile.isEmpty())
        return m_pathFile;
    return QDir::current().filePath("path.txt");
}

bool Metier::savePaths(const QString &pathFile)
{
    QString target = pathFile.isEmpty() ? mediaListFilePath() : pathFile;
    QFile f(target);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Metier::savePaths() - Failed to open for writing:" << target;
        return false;
    }
    QTextStream out(&f);
    for (int i = 0; i < m_paths.size(); ++i) {
        QString name = (i < m_names.size() ? m_names.at(i) : QString());
        if (!name.isEmpty())
            out << name << ":" << m_paths.at(i) << "\n";
        else
            out << m_paths.at(i) << "\n";
    }
    f.close();
    qDebug() << "Metier::savePaths() - Saved" << m_paths.size() << "paths to" << target;
    return true;
}

bool Metier::ensureVdInWorkingDir() const
{
    QDir targetDir(QDir::current().filePath("vd"));
    if (!targetDir.exists())
        targetDir.mkpath(".");

    // if there is already at least one mp4 in working dir vd, we're good
    QDir td(targetDir.absolutePath());
    QStringList existing = td.entryList(QStringList() << "*.mp4" << "*.MP4", QDir::Files);
    if (!existing.isEmpty()) {
        qDebug() << "Metier::ensureVdInWorkingDir() - found existing" << existing.size() << "files in" << targetDir.absolutePath();
        return true;
    }

    // try to locate a source vd in project root
    QStringList srcRoots;
    if (!m_projectRoot.isEmpty())
        srcRoots << m_projectRoot;
    srcRoots << QCoreApplication::applicationDirPath();
    srcRoots << QDir::currentPath();

    bool copiedAny = false;
    for (const QString &r : srcRoots) {
        QDir candidate(QDir(r).filePath("vd"));
        if (!candidate.exists())
            continue;

        // iterate files and copy into targetDir
        QDirIterator it(candidate.absolutePath(), QStringList() << "*.mp4" << "*.MP4", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString src = it.next();
            QString name = QFileInfo(src).fileName();
            QString dest = targetDir.filePath(name);
            if (!QFile::exists(dest)) {
                if (QFile::copy(src, dest)) {
                    qDebug() << "Metier::ensureVdInWorkingDir() - copied" << src << "->" << dest;
                    copiedAny = true;
                } else {
                    qDebug() << "Metier::ensureVdInWorkingDir() - failed to copy" << src << "->" << dest;
                }
            } else {
                qDebug() << "Metier::ensureVdInWorkingDir() - already present" << dest;
                copiedAny = true; // already present counts as success
            }
        }

        if (copiedAny)
            break; // stop after first root where we found files
    }

    // final check
    existing = td.entryList(QStringList() << "*.mp4" << "*.MP4", QDir::Files);
    qDebug() << "Metier::ensureVdInWorkingDir() - final count in" << td.absolutePath() << ":" << existing.size();
    return !existing.isEmpty();
}

void Metier::clear()
{
    m_paths.clear();
    m_names.clear();
}

bool Metier::loadPaths(const QString &pathFile)
{
    clear();
    QString requested = pathFile.isEmpty() ? QString("path.txt") : pathFile;
    // prefer working directory
    QString fullPath = QDir::current().filePath(requested);
    // resolve using project root if provided and path is relative
    if (!m_projectRoot.isEmpty()) {
        QFileInfo fi(pathFile);
        if (fi.isRelative()) {
            QDir d(m_projectRoot);
            fullPath = d.filePath(pathFile);
        }
    }

    QFile f(fullPath);
    bool usingResource = false;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // try resolving via provided project root (as before)
        if (!m_projectRoot.isEmpty()) {
            QFileInfo fi(requested);
            if (fi.isRelative()) {
                QDir d(m_projectRoot);
                fullPath = d.filePath(requested);
                f.setFileName(fullPath);
                if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    // continue to next fallback
                }
            }
        }
        // try embedded resource with same file name (:/path.txt)
        QString resourceCandidate;
        if (pathFile.startsWith(":/"))
            resourceCandidate = pathFile;
        else
            resourceCandidate = QString(":/%1").arg(QFileInfo(pathFile).fileName());

        if (QFile::exists(resourceCandidate)) {
            usingResource = true;
            // if we are using the resource as the source of paths, set project root to application dir
            if (m_projectRoot.isEmpty())
                m_projectRoot = QCoreApplication::applicationDirPath();
            f.setFileName(resourceCandidate);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                return false; // should not happen if exists()
        } else {
            // not found yet — try to search in parent folders from application dir
            QDir d(QCoreApplication::applicationDirPath());
            bool found = false;
            for (int i = 0; i < 8; ++i) {
                QString tryPath = d.filePath(requested);
                if (QFile::exists(tryPath)) {
                    fullPath = tryPath;
                    f.setFileName(fullPath);
                    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        found = true;
                        break;
                    }
                }
                d.cdUp();
            }

            if (!found) {
                // As a final step, create a default path file in the current working directory so app can run
                QString target = QDir::current().filePath(requested);
                QFile tf(target);
                if (tf.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&tf);
                    // write conservative defaults (vd/vd1..vd4)
                    out << "cabinet1:vd/vd1.mp4\n";
                    out << "cabinet2:vd/vd2.mp4\n";
                    out << "cabinet3:vd/vd3.mp4\n";
                    out << "cabinet4:vd/vd4.mp4\n";
                    tf.close();
                    // now open it for reading
                    f.setFileName(target);
                    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
                        return false;
                } else {
                    // unable to create file
                    return false;
                }
            }
        }
    }

    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QString name;
        QString path;

        int colon = line.indexOf(':');
        if (colon > 0) {
            // format name:path
            name = line.left(colon).trimmed();
            path = line.mid(colon + 1).trimmed();
        } else {
            path = line;
            // derive name from file name (without extension)
            QFileInfo pinfo(path);
            name = pinfo.completeBaseName();
            if (name.isEmpty())
                name = path;
        }

        // resolve relative path if project root set
        QFileInfo pinfo(path);
        if (!pinfo.isAbsolute() && !m_projectRoot.isEmpty()) {
            QDir d(m_projectRoot);
            path = d.filePath(path);
        }

        m_names.append(name);
        m_paths.append(path);
    }

    f.close();

    // If we read from an embedded resource (usingResource == true) then try to auto-discover
    // a more appropriate project root by walking up from the application dir and finding a
    // folder that contains the video files (eg. a 'vd' directory) so relative paths like 'vd/..'
    // will resolve correctly.
    if (usingResource) {
        // collect basenames of files we want to find
        QStringList basenames;
        for (const QString &p : m_paths) {
            QFileInfo fi(p);
            basenames << fi.fileName();
        }

        QString bestRoot;
        int bestCount = 0;
        QDir d(QCoreApplication::applicationDirPath());
        const int upLimit = 12;
        for (int depth = 0; depth < upLimit; ++depth) {
            QString candidateRoot = d.absolutePath();
            int count = 0;
            // check common locations relative to candidateRoot
            for (const QString &b : basenames) {
                QString p1 = QDir(candidateRoot).filePath(b);
                QString p2 = QDir(candidateRoot).filePath(QDir("vd").filePath(b));
                if (QFile::exists(p1) || QFile::exists(p2))
                    ++count;
            }

            if (count > bestCount) {
                bestCount = count;
                bestRoot = candidateRoot;
            }

            if (!d.cdUp())
                break;
        }

        if (bestCount > 0 && !bestRoot.isEmpty()) {
            m_projectRoot = bestRoot;
            qDebug() << "Metier: auto-detected project root:" << m_projectRoot << "(matches=" << bestCount << ")";
        }
    }

    return true;
}

QStringList Metier::videoPaths() const
{
    return m_paths;
}

QString Metier::videoPathForIndex(int index) const
{
    if (index < 0 || index >= m_paths.size())
        return {};
    return m_paths.at(index);
}

static QString joinPath(const QString &base, const QString &p)
{
    QDir d(base);
    return d.filePath(p);
}

QString Metier::resolvedVideoPathForIndex(int index, QStringList *tried) const
{
    if (index < 0 || index >= m_paths.size())
        return {};

    QString original = m_paths.at(index);
    QFileInfo info(original);

    // 1) if absolute and exists
    if (info.isAbsolute() && info.exists())
    {
        if (tried) tried->append(info.absoluteFilePath());
        return info.absoluteFilePath();
    }

    // prepare candidate roots
    QStringList roots;
    if (!m_projectRoot.isEmpty())
        roots << m_projectRoot;
    roots << QCoreApplication::applicationDirPath();
    roots << QDir::currentPath();

    // Try usual root/original combos and root/vd/original
    for (const QString &root : roots) {
        QString candidate = joinPath(root, original);
        qDebug() << "Trying" << candidate;
        if (tried) tried->append(candidate);
        if (QFile::exists(candidate))
            return QFileInfo(candidate).absoluteFilePath();

        // try root/vd/<original>
        QString candidate2 = joinPath(root, QDir("vd").filePath(original));
        qDebug() << "Trying" << candidate2;
        if (tried) tried->append(candidate2);
        if (QFile::exists(candidate2))
            return QFileInfo(candidate2).absoluteFilePath();
    }

    // 3) try filename only in roots (root/fileName), and also recursively search inside <root>/vd
    QString baseName = info.fileName();

    for (const QString &root : roots) {
        QString candidate = joinPath(root, baseName);
        qDebug() << "Trying" << candidate;
        if (tried) tried->append(candidate);
        if (QFile::exists(candidate))
            return QFileInfo(candidate).absoluteFilePath();

        // search recursively under root/vd for the filename
        QString vdRoot = joinPath(root, "vd");
        if (QDir(vdRoot).exists()) {
            QDirIterator it(vdRoot, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                QString found = it.next();
                if (tried) tried->append(found);
                if (QFileInfo(found).fileName() == baseName) {
                    qDebug() << "Found in recursive search:" << found;
                    return QFileInfo(found).absoluteFilePath();
                }
            }
        }
    }

    // 4) Try searching up through parent directories looking for vd/<baseName> or <baseName>
    const int upSteps = 6;
    for (const QString &rootOrigin : QStringList{QCoreApplication::applicationDirPath(), QDir::currentPath()}) {
        QDir d(rootOrigin);
        for (int i = 0; i < upSteps; ++i) {
            QString try1 = d.filePath(baseName);
            qDebug() << "Trying parent:" << try1;
            if (tried) tried->append(try1);
            if (QFile::exists(try1))
                return QFileInfo(try1).absoluteFilePath();

            QString try2 = d.filePath(QDir("vd").filePath(baseName));
            qDebug() << "Trying parent vd/ :" << try2;
            if (tried) tried->append(try2);
            if (QFile::exists(try2))
                return QFileInfo(try2).absoluteFilePath();

            d.cdUp();
        }
    }

    // not found
    // 4) check embedded resources - e.g. :/vd/<baseName> or :/<original>
    QString res1 = QString(":/vd/%1").arg(baseName);
    qDebug() << "Trying resource" << res1;
    if (QFile::exists(res1)) {
        if (tried) tried->append(res1);
        return res1;
    }

    QString res2 = QString(":/%1").arg(original);
    qDebug() << "Trying resource" << res2;
    if (QFile::exists(res2)) {
        if (tried) tried->append(res2);
        return res2;
    }
    return {};
}

QStringList Metier::cabinetNames() const
{
    return m_names;
}

bool Metier::addDoctorCard(const QString &doctorName,
                           const QString &specialite,
                           const QString &cabinetName,
                           const QString &email,
                           const QString &telephone,
                           const QString &adresse,
                           const QString &outputImagePath,
                           const QSize &size)
{
    if (doctorName.trimmed().isEmpty() && cabinetName.trimmed().isEmpty())
        return false;

    // Carte de visite standard : 85mm x 55mm (ratio ~1.54)
    // On utilise la taille fournie ou une taille par défaut
    QSize cardSize = size.isValid() ? size : QSize(850, 550);
    
    // Créer l'image avec fond dégradé
    QImage img(cardSize, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);

    int w = cardSize.width();
    int h = cardSize.height();

    // Fond dégradé bleu médical
    QLinearGradient gradient(0, 0, w, h);
    gradient.setColorAt(0, QColor(30, 60, 114));    // Bleu foncé
    gradient.setColorAt(1, QColor(42, 82, 152));    // Bleu médium
    p.fillRect(0, 0, w, h, gradient);

    // Bande blanche à gauche (accent design)
    p.fillRect(0, 0, 12, h, QColor(255, 255, 255));

    // Cercle décoratif en haut à droite
    p.setBrush(QColor(255, 255, 255, 30));
    p.setPen(Qt::NoPen);
    p.drawEllipse(w - 180, -80, 300, 300);

    // Icône médicale (croix) en haut à gauche
    p.setBrush(QColor(255, 255, 255));
    int iconX = 40, iconY = 35;
    p.drawRect(iconX + 15, iconY, 10, 40);      // Barre verticale
    p.drawRect(iconX, iconY + 15, 40, 10);      // Barre horizontale

    // Nom du docteur (grand, blanc)
    QFont nameFont("Arial", qMax(28, w/25), QFont::Bold);
    p.setFont(nameFont);
    p.setPen(Qt::white);
    QString displayName = doctorName.startsWith("Dr", Qt::CaseInsensitive) ? doctorName : QString("Dr. %1").arg(doctorName);
    p.drawText(100, 70, displayName);

    // Spécialité (sous le nom, couleur accent)
    QFont specFont("Arial", qMax(14, w/50), QFont::Normal);
    p.setFont(specFont);
    p.setPen(QColor(100, 200, 255));  // Bleu clair
    QString spec = specialite.isEmpty() ? "Médecine Générale" : specialite;
    p.drawText(100, 105, spec.toUpper());

    // Ligne de séparation
    p.setPen(QPen(QColor(255, 255, 255, 100), 1));
    p.drawLine(100, 125, w - 50, 125);

    // Nom du cabinet
    QFont cabinetFont("Arial", qMax(13, w/55), QFont::Bold);
    p.setFont(cabinetFont);
    p.setPen(Qt::white);
    p.drawText(100, 160, cabinetName);

    // Informations de contact
    QFont contactFont("Arial", qMax(11, w/65));
    p.setFont(contactFont);
    p.setPen(QColor(200, 220, 255));
    
    int yContact = 195;
    int lineHeight = 28;

    // Adresse avec icône
    if (!adresse.isEmpty()) {
        p.drawText(120, yContact, QString::fromUtf8("📍 ") + adresse);
        yContact += lineHeight;
    }

    // Téléphone avec icône
    QString tel = telephone.isEmpty() ? "01 23 45 67 89" : telephone;
    p.drawText(120, yContact, QString::fromUtf8("📞 ") + tel);
    yContact += lineHeight;

    // Email avec icône
    if (!email.isEmpty()) {
        p.drawText(120, yContact, QString::fromUtf8("✉ ") + email);
        yContact += lineHeight;
    }

    // QR Code placeholder (cercle avec texte)
    int qrSize = 80;
    int qrX = w - qrSize - 30;
    int qrY = h - qrSize - 30;
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(qrX, qrY, qrSize, qrSize, 8, 8);
    p.setPen(QColor(30, 60, 114));
    QFont qrFont("Arial", 8);
    p.setFont(qrFont);
    p.drawText(QRect(qrX, qrY, qrSize, qrSize), Qt::AlignCenter, "SCAN\nME");

    // Footer - SmartCity branding
    p.setPen(QColor(255, 255, 255, 150));
    QFont footerFont("Arial", qMax(8, w/90));
    p.setFont(footerFont);
    p.drawText(QRect(20, h - 25, w - qrSize - 60, 20), Qt::AlignLeft | Qt::AlignVCenter, 
               QString("SmartCity Healthcare • %1").arg(QDate::currentDate().toString("yyyy")));

    p.end();

    // Sauvegarder l'image
    return img.save(outputImagePath, "PNG", 100);
}

bool Metier::addDoctorPdf(const QString &doctorName,
                          const QString &specialite,
                          const QString &cabinetName,
                          const QString &email,
                          const QString &telephone,
                          const QString &adresse,
                          const QString &outputPdfPath,
                          const QSize &pageSize)
{
    Q_UNUSED(pageSize);
    Q_UNUSED(adresse);
    Q_UNUSED(email);
    
    if (doctorName.trimmed().isEmpty() && cabinetName.trimmed().isEmpty())
        return false;

    QString displayName = doctorName.startsWith("Dr", Qt::CaseInsensitive) ? doctorName : QString("Dr. %1").arg(doctorName);
    QString spec = specialite.isEmpty() ? "Médecine Générale" : specialite;
    QString tel = telephone.isEmpty() ? "01 23 45 67 89" : telephone;

    // Construire une carte de visite HTML élégante
    QString html;
    html += "<html><head><meta charset=\"utf-8\"><style>";
    html += "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 0; }";
    html += ".card { ";
    html += "  width: 350px; height: 200px; ";
    html += "  background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); ";
    html += "  border-radius: 12px; padding: 25px; color: white; ";
    html += "  box-shadow: 0 8px 32px rgba(0,0,0,0.3); ";
    html += "  position: relative; overflow: hidden; }";
    html += ".card::before { ";
    html += "  content: ''; position: absolute; ";
    html += "  left: 0; top: 0; bottom: 0; width: 8px; ";
    html += "  background: white; }";
    html += ".icon { font-size: 24px; margin-bottom: 10px; }";
    html += ".name { font-size: 22px; font-weight: bold; margin: 5px 0; }";
    html += ".specialite { font-size: 12px; color: #64b5f6; text-transform: uppercase; letter-spacing: 1px; }";
    html += ".cabinet { font-size: 14px; font-weight: 600; margin: 15px 0 10px 0; }";
    html += ".contact { font-size: 11px; color: #b3d4fc; line-height: 1.8; }";
    html += ".footer { position: absolute; bottom: 15px; left: 35px; font-size: 9px; color: rgba(255,255,255,0.6); }";
    html += "</style></head><body>";
    html += "<div class='card'>";
    html += "<div class='icon'>⚕</div>";
    html += QString("<div class='name'>%1</div>").arg(displayName.toHtmlEscaped());
    html += QString("<div class='specialite'>%1</div>").arg(spec.toHtmlEscaped());
    html += QString("<div class='cabinet'>%1</div>").arg(cabinetName.toHtmlEscaped());
    html += "<div class='contact'>";
    if (!adresse.isEmpty())
        html += QString("📍 %1<br/>").arg(adresse.toHtmlEscaped());
    html += QString("📞 %1<br/>").arg(tel.toHtmlEscaped());
    if (!email.isEmpty())
        html += QString("✉ %1").arg(email.toHtmlEscaped());
    html += "</div>";
    html += QString("<div class='footer'>SmartCity Healthcare • %1</div>").arg(QDate::currentDate().toString("yyyy"));
    html += "</div>";
    html += "</body></html>";

    // Utiliser QPdfWriter pour créer le PDF
    QPdfWriter writer(outputPdfPath);
    // Taille carte de visite (85mm x 55mm)
    writer.setPageSize(QPageSize(QSizeF(85, 55), QPageSize::Millimeter));
    writer.setResolution(300);

    QTextDocument doc;
    doc.setHtml(html);
    doc.setPageSize(QSizeF(writer.width(), writer.height()));

    QPainter painter(&writer);
    doc.drawContents(&painter);
    painter.end();

    return QFile::exists(outputPdfPath);
}
