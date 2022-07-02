#include "savefile.h"
#include <QDebug>



bool savefile(const QString& filename, std::function<void(QTextStream&)> pFunc) {
    // 选择文件保存路径
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString savepath = QFileDialog::getExistingDirectory(nullptr, QLatin1String("selct a path to save file"), desktop);
    if (savepath.isEmpty()) return false;

    // 文件名拼接（仅传入后缀名时（eg: .txt），就以系统时间戳生成文件名）
    savepath= savepath
            % QLatin1String("/")
            % (filename.startsWith('.') ? (QDateTime::currentDateTime().toString(QLatin1String("yyyy_MM_dd_hh_mm_ss"))) : QString(""))
            % filename;

    // 打开文件(如不存在则会自动创建)
    QFile file(savepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;

    // 输入字符流
    QTextStream out(&file);
    pFunc(out);

    // 关闭文件
    file.close();

    return true;
}
