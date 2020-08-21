#include "control.h"

QString formatSize(double size) {
    QString pattern;
    if (size < 1024) {
        pattern = "%1 B";
        return QString::number(size);
    } else if (size < 1024*1024) {
        size /= 1024;
        pattern = "%1 KB";
    } else if (size < 1024*1024*1024) {
        size /= 1024*1024;
        pattern = "%1 MB";
    } else {
        size /= 1024*1024*1024;
        pattern = "%1 GB";
    }
    return QString(pattern).arg(size, 0, 'f', 2);
}

QString formatRate(double rate) {
    return QString("%1/s").arg(formatSize(rate));
}
