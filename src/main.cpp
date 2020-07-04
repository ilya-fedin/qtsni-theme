#include <qpa/qplatformthemeplugin.h>
#include "qtsnitheme.h"

QT_BEGIN_NAMESPACE

class QtSNIThemePlugin : public QPlatformThemePlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID QPlatformThemeFactoryInterface_iid FILE "qtsni.json")

public:
    QPlatformTheme *create(const QString &key, const QStringList &params) override;
};

QPlatformTheme *QtSNIThemePlugin::create(const QString &key, const QStringList &params)
{
    Q_UNUSED(params);
    if (!key.compare(QLatin1String("qtsni"), Qt::CaseInsensitive))
        return new QtSNITheme;

    return nullptr;
}

QT_END_NAMESPACE

#include "main.moc"
