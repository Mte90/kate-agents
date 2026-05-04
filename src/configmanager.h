#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>

struct ProviderConfig {
    QString type;
    QString name;
    QString baseUrl;
    QString apiKey;
    QString defaultModel;
    bool enabled = false;
};

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() override;

    void load();
    void save();

    QString getActiveProvider() const { return m_activeProvider; }
    void setActiveProvider(const QString &provider) { m_activeProvider = provider; }

    QString getActiveModel() const { return m_activeModel; }
    void setActiveModel(const QString &model) { m_activeModel = model; }

    int getMaxIterations() const { return m_maxIterations; }
    void setMaxIterations(int max) { m_maxIterations = max; }

    double getTemperature() const { return m_temperature; }
    void setTemperature(double temp) { m_temperature = temp; }

    int getMaxTokens() const { return m_maxTokens; }
    void setMaxTokens(int tokens) { m_maxTokens = tokens; }

    QString getSystemPrompt() const { return m_systemPrompt; }
    void setSystemPrompt(const QString &prompt) { m_systemPrompt = prompt; }

    std::vector<ProviderConfig> getProviders() const { return m_providers; }
    void setProviders(const std::vector<ProviderConfig> &providers) { m_providers = providers; }

    ProviderConfig getProviderConfig(const QString &name) const;
    void setProviderConfig(const QString &name, const ProviderConfig &config);

    bool bufferContextEnabled() const { return m_bufferContextEnabled; }
    void setBufferContextEnabled(bool enabled) { m_bufferContextEnabled = enabled; }

signals:
    void configChanged();

private:
    QString m_activeProvider;
    QString m_activeModel;
    int m_maxIterations = 20;
    double m_temperature = 0.7;
    int m_maxTokens = 4096;
    QString m_systemPrompt;
    std::vector<ProviderConfig> m_providers;
    bool m_bufferContextEnabled = true;
};

#endif
