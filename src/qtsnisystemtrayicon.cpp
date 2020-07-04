#include "qtsnisystemtrayicon.h"
#include "flatpakutils.h"
#include <QStandardPaths>
#include <QDir>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QRect>
#include <QApplication>
#include <QDBusMetaType>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

static bool isIndicatorApplication() {
    // Hack for indicator-application, which doesn't handle icons sent across D-Bus:
    // save the icon to a temp file and set the icon name to that filename.
    static const auto Result = [] {
        const auto interface = QDBusConnection::sessionBus().interface();

        if (!interface) {
            return false;
        }

        const auto ubuntuIndicator = interface->isServiceRegistered(
            "com.canonical.indicator.application");

        const auto ayatanaIndicator = interface->isServiceRegistered(
            "org.ayatana.indicator.application");

        return ubuntuIndicator || ayatanaIndicator;
    }();

    return Result;
}

static QString iconTempPath()
{
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (!tempPath.isEmpty()) {
        if (QtSNI::FlatpakUtils::inFlatpak()) {
            const auto flatpakId = QtSNI::FlatpakUtils::flatpakId();
            if (!flatpakId.isEmpty()) {
                tempPath += "/app/" + flatpakId;
                return tempPath;
            }
        } else {
            return tempPath;
        }
    }

    tempPath = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);

    if (!tempPath.isEmpty()) {
        QDir tempDir(tempPath);
        if (tempDir.exists())
            return tempPath;

        if (tempDir.mkpath(QStringLiteral("."))) {
            const QFile::Permissions permissions = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
            if (QFile(tempPath).setPermissions(permissions))
                return tempPath;
        }
    }

    return QDir::tempPath();
}

static inline QString tempFileTemplate()
{
    static const QString TempFileTemplate = iconTempPath() + QLatin1String("/qtsni-trayicon-XXXXXX.png");
    return TempFileTemplate;
}

static std::unique_ptr<QTemporaryFile> tempIcon(
        const QIcon &icon,
        QObject *parent) {
    qreal dpr = qGuiApp->devicePixelRatio();
    const auto desiredSize = QSize(22 * dpr, 22 * dpr);

    auto ret = std::make_unique<QTemporaryFile>(
        tempFileTemplate(),
        parent);

    ret->open();

    if (icon.actualSize(desiredSize) == desiredSize) {
        icon.pixmap(desiredSize).save(ret.get());
    } else {
        const auto availableSizes = icon.availableSizes();

        const auto biggestSize = std::max_element(
            availableSizes.begin(),
            availableSizes.end(),
            [](const QSize &a, const QSize &b) {
                return a.width() < b.width();
            });

        icon
            .pixmap(*biggestSize)
            .scaled(
                desiredSize,
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation)
            .save(ret.get());
    }

    ret->close();

    return ret;
}

SystemTrayMenu::SystemTrayMenu()
    : QPlatformMenu(),
    m_tag(0),
    m_menu(new QMenu())
{
    connect(m_menu.data(), &QMenu::aboutToShow, this, &QPlatformMenu::aboutToShow);
    connect(m_menu.data(), &QMenu::aboutToHide, this, &QPlatformMenu::aboutToHide);
}

SystemTrayMenu::~SystemTrayMenu()
{
    if (m_menu)
        m_menu->deleteLater();
}

QPlatformMenuItem *SystemTrayMenu::createMenuItem() const
{
    return new SystemTrayMenuItem();
}

void SystemTrayMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem))
    {
        bool inserted = false;

        if (SystemTrayMenuItem *oursBefore = qobject_cast<SystemTrayMenuItem*>(before))
        {
            for (auto it = m_items.begin(); it != m_items.end(); ++it)
            {
                if (*it == oursBefore)
                {
                    m_items.insert(it, ours);
                    if (m_menu)
                        m_menu->insertAction(oursBefore->action(), ours->action());

                    inserted = true;
                    break;
                }
            }
        }

        if (!inserted)
        {
            m_items.append(ours);
            if (m_menu)
                m_menu->addAction(ours->action());
        }
    }
}

QPlatformMenuItem *SystemTrayMenu::menuItemAt(int position) const
{
    if (position < m_items.size())
        return m_items.at(position);

    return nullptr;
}

QPlatformMenuItem *SystemTrayMenu::menuItemForTag(quintptr tag) const
{
    auto it = std::find_if(m_items.constBegin(), m_items.constEnd(), [tag] (SystemTrayMenuItem *item)
    {
        return item->tag() == tag;
    });

    if (it != m_items.constEnd())
        return *it;

    return nullptr;
}

void SystemTrayMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem))
    {
        m_items.removeOne(ours);
        if (ours->action() && m_menu)
            m_menu->removeAction(ours->action());
    }
}

void SystemTrayMenu::setEnabled(bool enabled)
{
    if (!m_menu)
        return;

    m_menu->setEnabled(enabled);
}

void SystemTrayMenu::setIcon(const QIcon &icon)
{
    if (!m_menu)
        return;

    m_menu->setIcon(icon);
}

void SystemTrayMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenu::setText(const QString &text)
{
    if (!m_menu)
        return;

    m_menu->setTitle(text);
}

void SystemTrayMenu::setVisible(bool visible)
{
    if (!m_menu)
        return;

    m_menu->setVisible(visible);
}

void SystemTrayMenu::syncMenuItem(QPlatformMenuItem *)
{
    // Nothing to do
}

void SystemTrayMenu::syncSeparatorsCollapsible(bool enable)
{
    if (!m_menu)
        return;

    m_menu->setSeparatorsCollapsible(enable);
}

quintptr SystemTrayMenu::tag() const
{
    return m_tag;
}

QMenu *SystemTrayMenu::menu() const
{
    return m_menu.data();
}

SystemTrayMenuItem::SystemTrayMenuItem()
    : QPlatformMenuItem(),
    m_tag(0),
    m_action(new QAction(this))
{
    connect(m_action, &QAction::triggered, this, &QPlatformMenuItem::activated);
    connect(m_action, &QAction::hovered, this, &QPlatformMenuItem::hovered);
}

SystemTrayMenuItem::~SystemTrayMenuItem()
{
}

void SystemTrayMenuItem::setCheckable(bool checkable)
{
    m_action->setCheckable(checkable);
}

void SystemTrayMenuItem::setChecked(bool isChecked)
{
    m_action->setChecked(isChecked);
}

void SystemTrayMenuItem::setEnabled(bool enabled)
{
    m_action->setEnabled(enabled);
}

void SystemTrayMenuItem::setFont(const QFont &font)
{
    m_action->setFont(font);
}

void SystemTrayMenuItem::setIcon(const QIcon &icon)
{
    m_action->setIcon(icon);
}

void SystemTrayMenuItem::setIsSeparator(bool isSeparator)
{
    m_action->setSeparator(isSeparator);
}

void SystemTrayMenuItem::setMenu(QPlatformMenu *menu)
{
    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu *>(menu))
        m_action->setMenu(ourMenu->menu());
}

void SystemTrayMenuItem::setRole(QPlatformMenuItem::MenuRole)
{
}

void SystemTrayMenuItem::setShortcut(const QKeySequence &shortcut)
{
    m_action->setShortcut(shortcut);
}

void SystemTrayMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenuItem::setText(const QString &text)
{
    m_action->setText(text);
}

void SystemTrayMenuItem::setVisible(bool isVisible)
{
    m_action->setVisible(isVisible);
}

void SystemTrayMenuItem::setIconSize(int)
{
}

quintptr SystemTrayMenuItem::tag() const
{
    return m_tag;
}

QAction *SystemTrayMenuItem::action() const
{
    return m_action;
}

QtSNISystemTrayIcon::QtSNISystemTrayIcon()
    : QPlatformSystemTrayIcon(),
    mSni(nullptr)
{
    // register types
    qDBusRegisterMetaType<ToolTip>();
    qDBusRegisterMetaType<IconPixmap>();
    qDBusRegisterMetaType<IconPixmapList>();
}

QtSNISystemTrayIcon::~QtSNISystemTrayIcon()
{
}

void QtSNISystemTrayIcon::init()
{
    if (!mSni)
    {
        mSni = new StatusNotifierItem(QString::number(QCoreApplication::applicationPid()), this);
        mSni->setTitle(QApplication::applicationDisplayName());

        // default menu
        QPlatformMenu *menu = createMenu();
        menu->setParent(mSni);
        QPlatformMenuItem *menuItem = menu->createMenuItem();
        menuItem->setParent(menu);
        menuItem->setText(tr("Quit"));
        menuItem->setIcon(QIcon::fromTheme(QLatin1String("application-exit")));
        connect(menuItem, &QPlatformMenuItem::activated, qApp, &QApplication::quit);
        menu->insertMenuItem(menuItem, nullptr);
        updateMenu(menu);

        connect(mSni, &StatusNotifierItem::activateRequested, [this](const QPoint &)
        {
            Q_EMIT activated(QPlatformSystemTrayIcon::Trigger);
        });

        connect(mSni, &StatusNotifierItem::secondaryActivateRequested, [this](const QPoint &)
        {
            Q_EMIT activated(QPlatformSystemTrayIcon::MiddleClick);
        });
    }
}

void QtSNISystemTrayIcon::cleanup()
{
    delete mSni;
    mSni = nullptr;
}

void QtSNISystemTrayIcon::updateIcon(const QIcon &icon)
{
    if (!mSni)
        return;

    if (mTempIcon && !isIndicatorApplication())
    {
        mTempIcon = nullptr;
    }

    if (icon.name().isEmpty())
    {
        if (isIndicatorApplication())
        {
            mTempIcon = tempIcon(icon, this);

            if (mTempIcon) {
                mSni->setIconByName(mTempIcon->fileName());
                mSni->setToolTipIconByName(mTempIcon->fileName());
            }
        }
        else
        {
            mSni->setIconByPixmap(icon);
            mSni->setToolTipIconByPixmap(icon);
        }
    }
    else
    {
        mSni->setIconByName(icon.name());
        mSni->setToolTipIconByName(icon.name());
    }
}

void QtSNISystemTrayIcon::updateToolTip(const QString &tooltip)
{
    if (!mSni)
        return;

    mSni->setToolTipTitle(tooltip);
}

void QtSNISystemTrayIcon::updateMenu(QPlatformMenu *menu)
{
    if (!mSni)
        return;

    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu*>(menu))
        mSni->setContextMenu(ourMenu->menu());
}

QPlatformMenu *QtSNISystemTrayIcon::createMenu() const
{
    return new SystemTrayMenu();
}

QRect QtSNISystemTrayIcon::geometry() const
{
    // StatusNotifierItem doesn't provide the geometry
    return {};
}

void QtSNISystemTrayIcon::showMessage(const QString &title, const QString &msg,
                                     const QIcon &icon, MessageIcon, int secs)
{
    if (!mSni)
        return;

    mSni->showMessage(title, msg, icon.name(), secs);
}

bool QtSNISystemTrayIcon::isSystemTrayAvailable() const
{
    QDBusInterface systrayHost(QLatin1String("org.kde.StatusNotifierWatcher"),
                               QLatin1String("/StatusNotifierWatcher"),
                               QLatin1String("org.kde.StatusNotifierWatcher"));

    return systrayHost.isValid() && systrayHost.property("IsStatusNotifierHostRegistered").toBool();
}

bool QtSNISystemTrayIcon::supportsMessages() const
{
    return true;
}
