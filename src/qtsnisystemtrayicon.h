#pragma once

#include <memory>

#include <qpa/qplatformmenu.h>
#include <qpa/qplatformsystemtrayicon.h>
#include <QTemporaryFile>

#include "statusnotifieritem/statusnotifieritem.h"

class SystemTrayMenuItem;
class QAction;
class QMenu;

class SystemTrayMenu : public QPlatformMenu
{
    Q_OBJECT
public:
    SystemTrayMenu();
    ~SystemTrayMenu() Q_DECL_OVERRIDE;
    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemAt(int position) const Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const Q_DECL_OVERRIDE;
    void removeMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void syncMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncSeparatorsCollapsible(bool enable) Q_DECL_OVERRIDE;
    quintptr tag() const Q_DECL_OVERRIDE;
    QPlatformMenuItem *createMenuItem() const Q_DECL_OVERRIDE;

    QMenu *menu() const;

private:
    quintptr m_tag;
    QPointer<QMenu> m_menu;
    QList<SystemTrayMenuItem*> m_items;
};

class SystemTrayMenuItem : public QPlatformMenuItem
{
    Q_OBJECT
public:
    SystemTrayMenuItem();
    ~SystemTrayMenuItem() Q_DECL_OVERRIDE;
    void setCheckable(bool checkable) Q_DECL_OVERRIDE;
    void setChecked(bool isChecked) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setFont(const QFont &font) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setIsSeparator(bool isSeparator) Q_DECL_OVERRIDE;
    void setMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    void setRole(MenuRole role) Q_DECL_OVERRIDE;
    void setShortcut(const QKeySequence &shortcut) Q_DECL_OVERRIDE;
    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setVisible(bool isVisible) Q_DECL_OVERRIDE;
    quintptr tag() const Q_DECL_OVERRIDE;
    void setIconSize(int size)
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    Q_DECL_OVERRIDE
    #endif
    ;

    QAction *action() const;

private:
    quintptr m_tag;
    QAction *m_action;
};

class QtSNISystemTrayIcon : public QPlatformSystemTrayIcon
{
public:
    QtSNISystemTrayIcon();
    ~QtSNISystemTrayIcon() Q_DECL_OVERRIDE;

    void init() Q_DECL_OVERRIDE;
    void cleanup() Q_DECL_OVERRIDE;
    void updateIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void updateToolTip(const QString &tooltip) Q_DECL_OVERRIDE;
    void updateMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    QRect geometry() const Q_DECL_OVERRIDE;
    void showMessage(const QString &title, const QString &msg,
                     const QIcon &icon, MessageIcon iconType, int secs) Q_DECL_OVERRIDE;

    bool isSystemTrayAvailable() const Q_DECL_OVERRIDE;
    bool supportsMessages() const Q_DECL_OVERRIDE;

    QPlatformMenu *createMenu() const Q_DECL_OVERRIDE;

private:
    StatusNotifierItem *mSni;
    std::unique_ptr<QTemporaryFile> mTempIcon = nullptr;
};
