#include "qtsnitheme.h"

#include <private/qguiapplication_p.h>
#include <qpa/qplatformtheme_p.h>
#include <qpa/qplatformthemefactory_p.h>
#include <qpa/qplatformintegration.h>

#include <QStandardPaths>
#include "qtsnisystemtrayicon.h"

QT_BEGIN_NAMESPACE

static bool checkNeedPortalSupport()
{
#if QT_CONFIG(dbus)
    return !QStandardPaths::locate(QStandardPaths::RuntimeLocation, QLatin1String("flatpak-info")).isEmpty() || qEnvironmentVariableIsSet("SNAP");
#else
    return false;
#endif // QT_CONFIG(dbus)
}

class QtSNIThemePrivate : public QPlatformThemePrivate
{
public:
    QtSNIThemePrivate()
        : QPlatformThemePrivate()
    { }

    ~QtSNIThemePrivate()
    {
        delete baseTheme;
    }

    QPlatformTheme *baseTheme = nullptr;
};

QtSNITheme::QtSNITheme()
    : d_ptr(new QtSNIThemePrivate)
{
    Q_D(QtSNITheme);

    // Get platform theme override from a variable
    QString platformThemeName = QString::fromLocal8Bit(qgetenv("QTSNI_PLATFORMTHEME"));

    // 1) Fetch the platform name from the environment if present.
    QStringList themeNames;
    if (!platformThemeName.isEmpty())
        themeNames.append(platformThemeName);

    // 2) Special case - check whether it's a flatpak or snap app to use xdg-desktop-portal platform theme for portals support
    if (checkNeedPortalSupport())
        themeNames.append(QStringLiteral("xdgdesktopportal"));

    // 3) Ask the platform integration for a list of theme names
    themeNames += QGuiApplicationPrivate::platform_integration->themeNames();
    // 4) Look for a theme plugin.
    for (const QString &themeName : qAsConst(themeNames)) {
        d->baseTheme = QPlatformThemeFactory::create(themeName, nullptr);
        if (d->baseTheme)
            break;
    }

    // 4) If no theme plugin was found ask the platform integration to
    // create a theme
    if (!d->baseTheme) {
        for (const QString &themeName : qAsConst(themeNames)) {
            d->baseTheme = QGuiApplicationPrivate::platform_integration->createPlatformTheme(themeName);
            if (d->baseTheme)
                break;
        }
        // No error message; not having a theme plugin is allowed.
    }

    // 5) Fall back on the built-in "null" platform theme.
    if (!d->baseTheme)
        d->baseTheme = new QPlatformTheme;
}

QPlatformMenuItem* QtSNITheme::createPlatformMenuItem() const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->createPlatformMenuItem();
}

QPlatformMenu* QtSNITheme::createPlatformMenu() const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->createPlatformMenu();
}

QPlatformMenuBar* QtSNITheme::createPlatformMenuBar() const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->createPlatformMenuBar();
}

void QtSNITheme::showPlatformMenuBar()
{
    Q_D(const QtSNITheme);
    return d->baseTheme->showPlatformMenuBar();
}

bool QtSNITheme::usePlatformNativeDialog(DialogType type) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->usePlatformNativeDialog(type);
}

QPlatformDialogHelper* QtSNITheme::createPlatformDialogHelper(DialogType type) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->createPlatformDialogHelper(type);
}

#ifndef QT_NO_SYSTEMTRAYICON
QPlatformSystemTrayIcon* QtSNITheme::createPlatformSystemTrayIcon() const
{
    Q_D(const QtSNITheme);

    auto trayIcon = new QtSNISystemTrayIcon;
    if (trayIcon->isSystemTrayAvailable())
        return trayIcon;
    else {
        delete trayIcon;
        return d->baseTheme->createPlatformSystemTrayIcon();
    }
}
#endif

const QPalette *QtSNITheme::palette(Palette type) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->palette(type);
}

const QFont* QtSNITheme::font(Font type) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->font(type);
}

QVariant QtSNITheme::themeHint(ThemeHint hint) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->themeHint(hint);
}

QPixmap QtSNITheme::standardPixmap(StandardPixmap sp, const QSizeF &size) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->standardPixmap(sp, size);
}

QIcon QtSNITheme::fileIcon(const QFileInfo &fileInfo,
                              QPlatformTheme::IconOptions iconOptions) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->fileIcon(fileInfo, iconOptions);
}

QIconEngine * QtSNITheme::createIconEngine(const QString &iconName) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->createIconEngine(iconName);
}

#if QT_CONFIG(shortcut)
QList<QKeySequence> QtSNITheme::keyBindings(QKeySequence::StandardKey key) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->keyBindings(key);
}
#endif

QString QtSNITheme::standardButtonText(int button) const
{
    Q_D(const QtSNITheme);
    return d->baseTheme->standardButtonText(button);
}

QT_END_NAMESPACE
