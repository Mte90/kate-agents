#ifndef TERMINALTOOL_H
#define TERMINALTOOL_H

#include <KLocalizedString>

#include "../toolregistry.h"
#include <KLocalizedString>
#include <QProcess>
#include <QTimer>
#include <QEventLoop>
#include <QSet>
#include <QString>
#include <QRegularExpression>

class TerminalTool : public AgentTool
{
    Q_OBJECT

private:
    const QSet<QString> &blockedCommands() const {
        static QSet<QString> commands = {
            "rm", "rmdir", "del",
            "wget", "curl", "nc", "netcat",
            "sh", "bash", "zsh", "fish", "ksh", "dash", "csh", "tcsh",
            "cp", "mv", "ln", "chmod", "chown", "touch", "mkdir",
            "python", "python3", "python2", "perl", "ruby", "node",
            "php", "lua", "tcl", "expect",
            "sudo", "su",
            "dd", "mkfs", "fdisk", "parted",
            "apt", "apt-get", "yum", "dnf", "pacman", "zypper", "brew",
            "systemctl", "service", "init", "shutdown", "reboot", "halt",
            "modprobe", "insmod", "rmmod", "mknod",
            "ping", "ping6", "traceroute", "tracepath", "arp", "arping",
            "nmap", "ssh", "scp", "rsync", "sftp",
            "iptables", "ip6tables", "firewall-cmd", "ufw",
            "nmcli", "ip", "ifconfig", "route", "netstat", "ss"
        };
        return commands;
    }

    bool isCommandBlocked(const QString &command) const {
        QString trimmed = command.trimmed();
        if (trimmed.isEmpty()) {
            return false;
        }

        QString firstWord = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).first().toLower();

        if (firstWord.contains("/")) {
            QString baseName = firstWord.mid(firstWord.lastIndexOf("/") + 1);
            if (blockedCommands().contains(baseName)) {
                return true;
            }
        }

        return blockedCommands().contains(firstWord);
    }

public:
    explicit TerminalTool(QObject *parent = nullptr) : AgentTool(parent) {}

    QString name() const override { return "terminal"; }
    QString description() const override {
        return i18n("Run a terminal command - returns stdout and stderr output");
    }

    QJsonObject parametersSchema() const override {
        QJsonObject schema;
        schema["type"] = "object";
        QJsonObject properties;
        
        QJsonObject cmdProp;
        cmdProp["type"] = "string";
        cmdProp["description"] = i18n("Shell command to execute");
        properties["command"] = cmdProp;
        
        QJsonObject wdProp;
        wdProp["type"] = "string";
        wdProp["description"] = i18n("Working directory");
        properties["working_directory"] = wdProp;
        
        QJsonObject timeoutProp;
        timeoutProp["type"] = "integer";
        timeoutProp["description"] = i18n("Timeout in seconds (default: 60, max: 300)");
        timeoutProp["default"] = 60;
        properties["timeout_seconds"] = timeoutProp;
        
        schema["properties"] = properties;
        schema["required"] = QJsonArray{"command"};
        return schema;
    }

    QJsonObject execute(const QJsonObject &args) override {
        QString command = args["command"].toString();
        QString workingDir = args["working_directory"].toString(".");
        int timeout = qMin(args["timeout_seconds"].toInt(60), 300);
        
        if (isCommandBlocked(command)) {
            return QJsonObject{
                {"error", "Command blocked for security reasons"},
                {"success", false}
            };
        }
        
        QProcess process;
        process.setWorkingDirectory(workingDir);
        
        QEventLoop loop;
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start(timeout * 1000);
        
        connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                &loop, &QEventLoop::quit);
        connect(&timeoutTimer, &QTimer::timeout, [&]() {
            process.kill();
        });
        
        process.start("bash", QStringList() << "-c" << command);
        process.waitForFinished(-1);
        
        QString stdout = process.readAllStandardOutput();
        QString stderr = process.readAllStandardError();
        int exitCode = process.exitCode();
        
        QString output = stdout;
        if (!stderr.isEmpty()) {
            output += "\n--- stderr ---\n" + stderr;
        }
        
        if (output.length() > 10000) {
            output = output.left(10000) + "\n... (truncated)";
        }
        
        return QJsonObject{
            {"success", true},
            {"command", command},
            {"output", output},
            {"exit_code", exitCode},
            {"timed_out", timeoutTimer.isActive() == false && process.state() == QProcess::NotRunning}
        };
    }

    bool requiresPermission() const override { return true; }
};

#endif