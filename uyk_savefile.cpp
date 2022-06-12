#include "uyk_savefile.h"

bool savefile(QString suffix, std::function<void(QTextStream&)> pFunc) {
    // 选择文件保存路径
    QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString csvFile = QFileDialog::getExistingDirectory(nullptr, "selct a path to save file", desktop);
    if (csvFile.isEmpty()) return false;

    // 以系统时间戳生成文件名
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString   current_date      = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
    csvFile += QStringLiteral("/data_%1.%2").arg(current_date, suffix);

    // 打开文件(如不存在则会自动创建)
    QFile file(csvFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return false;

    // 输入字符流
    QTextStream out(&file);
    pFunc(out);

    // 关闭文件
    file.close();

    return true;
}
