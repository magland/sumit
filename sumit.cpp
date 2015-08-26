#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDir>
#include <QStringList>
#include <QTime>
#include <QDataStream>
#include <sys/stat.h>

QString compute_the_file_hash(const QString &path) {
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QFile FF(path);
    if (!FF.open(QFile::ReadOnly)) return "";
    while (!FF.atEnd()) {
        hash.addData(FF.read(10000));
    }
    return QString(hash.result().toHex());
}

QString compute_the_string_hash(const QString &str) {
    QCryptographicHash X(QCryptographicHash::Sha1);
    X.addData(str.toLatin1());
    return QString(X.result().toHex());
}

void create_directory_if_doesnt_exist(const QString &dirname) {
    QString parent_dirname=QFileInfo(dirname).path();
    QString folder_name=QFileInfo(dirname).fileName();
    if (QDir(parent_dirname).exists(folder_name)) return;

    if (!parent_dirname.isEmpty()) {
        create_directory_if_doesnt_exist(parent_dirname);
    }
    QDir(parent_dirname).mkdir(folder_name);
}

QString read_text_file(const QString &path) {
    QFile f(path);
    if (!f.open(QFile::ReadOnly|QFile::Text)) return "";
    QTextStream in(&f);
    return in.readAll();
}
void write_text_file(const QString &path,const QString &txt) {
    QFile f(path);
    if (!f.open(QFile::WriteOnly|QFile::Text)) return;
    QTextStream out(&f);
    out << txt;
}

void create_hash_file(const QString &path,const QString &hash_path) {
    QString the_hash=compute_the_file_hash(path);
    write_text_file(hash_path,the_hash);
}

void usage() {
    qDebug() << "Usage: sumit [file_path]";
}

QString sumit(const QString &path) {
    //the file id is a hashed function of device,inode,size, and modification time (in seconds)
    //note that it is not dependent on the file name
    struct stat SS;
    stat(path.toLatin1().data(),&SS);
    QString id_string=QString("%1:%2:%3:%4").arg(SS.st_dev).arg(SS.st_ino).arg(SS.st_size).arg(SS.st_mtim.tv_sec);
    QString file_id=compute_the_string_hash(id_string);

    QString dirname=QString("/tmp/sumit/sha1/%1").arg(file_id.mid(0,4));
    create_directory_if_doesnt_exist(dirname);
    QString hash_path=QString("%1/%2").arg(dirname).arg(file_id);

    QString hash_sum=read_text_file(hash_path);
    if (hash_sum.isEmpty()) {
        create_hash_file(path,hash_path);
        hash_sum=read_text_file(hash_path);
    }
    if (hash_sum.count()!=40) {
        create_hash_file(path,hash_path);
        hash_sum=read_text_file(hash_path);
    }
    return hash_sum;
}

QString sumit_dir(const QString &path) {
    QStringList files=QDir(path).entryList(QStringList("*"),QDir::Files,QDir::Name);
    QStringList dirs=QDir(path).entryList(QStringList("*"),QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);

    QString str="";
    for (int i=0; i<dirs.count(); i++) {
        str+=QString("%1 %2\n").arg(sumit_dir(path+"/"+dirs[i])).arg(dirs[i]);
    }
    for (int i=0; i<files.count(); i++) {
        str+=QString("%1 %2\n").arg(sumit(path+"/"+files[i])).arg(files[i]);
    }

    return compute_the_string_hash(str);
}

int main(int argc, char *argv[])
{

    if (argc<2) {
        usage();
        return 0;
    }

    QTime timer; timer.start();

    QString path=QString(argv[1]);

    if (QFileInfo(path).isDir()) {
        QString the_hash=sumit_dir(path);
        printf("%s\n",the_hash.toLatin1().data());
    }
    else {
        QString the_hash=sumit(path);
        printf("%s\n",the_hash.toLatin1().data());
    }

    //qDebug() << "Elapsed: " << timer.elapsed()*1.0/1000;

    return 0;
}

