#pragma once

#include <QStandardPaths>

namespace QtSNI{
namespace FlatpakUtils {

inline bool inFlatpak()
{
    return !QStandardPaths::locate(QStandardPaths::RuntimeLocation, QLatin1String("flatpak-info")).isEmpty();
}

inline QString flatpakId() {
    return QString::fromLocal8Bit(qgetenv("FLATPAK_ID"));
}

} // namespace FlatpakUtils
} // namespace QtSNI
