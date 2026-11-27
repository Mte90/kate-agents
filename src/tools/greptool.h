#ifndef GREPTOOL_H
#define GREPTOOL_H

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

class GrepTool : public AgentTool
{
    Q_OBJECT

public:
    explicit GrepTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "grep"; }
    QString description() const override {
        return i18n("Search text in files - takes pattern and optional file path");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject patternProp;
        patternProp["type"] = "string";
        patternProp["description"] = i18n("Regex pattern to search for");
        properties["pattern"] = patternProp;
        
        QJsonObject pathProp;
        pathProp["type"] = "string";
        pathProp["description"] = i18n("Directory to search in (default: .)");
        properties["path"] = pathProp;
        
        QJsonObject includeProp;
        includeProp["type"] = "string";
        includeProp["description"] = i18n("Glob pattern for file inclusion");
        properties["include"] = includeProp;
        
        QJsonObject maxResultsProp;
        maxResultsProp["type"] = "integer";
        maxResultsProp["description"] = i18n("Maximum number of results");
        maxResultsProp["default"] = 50;
        properties["max_results"] = maxResultsProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"pattern"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        static constexpr qint64 kMaxFileBytes = 2 * 1024 * 1024; // 2 MiB
        static constexpr int kMaxLineChars = 4096;

        const QString pattern = args["pattern"].toString();
        const QString rawPath = args["path"].toString().trimmed();
        const QString searchPath = rawPath.isEmpty() ? QString(".") : rawPath;
        const QString rawInclude = args["include"].toString().trimmed();
        const QString include = rawInclude.isEmpty() ? QString("*") : rawInclude;
        const int maxResults = qBound(1, args["max_results"].toInt(50), 500);

        if (pattern.isEmpty() || pattern.size() > 512) {
            return QJsonObject{
                {"error", i18n("Invalid or overly long regex pattern")},
                {"success", false}
            };
        }

        const QRegularExpression regex(pattern);
        if (!regex.isValid()) {
            return QJsonObject{
                {"error", i18n("Invalid regex pattern: %1", regex.errorString())},
                {"success", false}
            };
        }

        const QFileInfo rootInfo(searchPath);
        if (!rootInfo.exists() || !rootInfo.isDir()) {
            return QJsonObject{
                {"error", i18n("Search directory does not exist: %1", searchPath)},
                {"success", false}
            };
        }

        const QDir rootDir(rootInfo.absoluteFilePath());
        QJsonArray results;
        int count = 0;

        const QStringList excludeDirs = {
            ".git", ".hg", ".svn", ".venv", "venv", "node_modules", "dist", "build", "bin", "__pycache__"
        };
        QDirIterator it(rootDir.absolutePath(),
                        QStringList() << include,
                        QDir::Files | QDir::Readable,
                        QDirIterator::Subdirectories);

        while (it.hasNext() && count < maxResults) {
            const QFileInfo fileInfo(it.next());
            if (fileInfo.size() > kMaxFileBytes) {
                continue;
            }

            const QString absolutePath = fileInfo.absoluteFilePath();
            const QString relPath = rootDir.relativeFilePath(absolutePath);
            bool skipFile = false;
            const QStringList parts = relPath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
            // Check parent directory segments within searchPath (exclude the filename itself)
            for (int i = 0; i < parts.size() - 1; ++i) {
                if (excludeDirs.contains(parts.at(i))) {
                    skipFile = true;
                    break;
                }
            }
            if (skipFile) {
                continue;
            }

            QFile file(absolutePath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                continue;
            }

            QTextStream in(&file);
            in.setEncoding(QStringConverter::Utf8);
            int lineNum = 0;
            while (!in.atEnd() && count < maxResults) {
                lineNum++;
                QString line = in.readLine(kMaxLineChars);
                if (line.contains(QChar::Null)) {
                    break; // Skip binary-like files
                }
                const QRegularExpressionMatch match = regex.match(line);
                if (match.hasMatch()) {
                    results.append(QJsonObject{
                        {"file", absolutePath},
                        {"line", lineNum},
                        {"content", line},
                        {"match", match.captured(0)}
                    });
                    count++;
                }
            }
            file.close();
        }

        return QJsonObject{
            {"success", true},
            {"results", results},
            {"count", count}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif
