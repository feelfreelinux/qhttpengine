/*
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QUrl>

#include <QHttpEngine/QFilesystemHandler>
#include <QHttpEngine/QHttpRange>
#include <QHttpEngine/QIODeviceCopier>
#include <QDebug>
#include "qfilesystemhandler_p.h"

QMap<int, QUrl> fileMap;
int fileMapCounter = 0;

QFilesystemHandlerPrivate::QFilesystemHandlerPrivate(QFilesystemHandler *handler)
    : QObject(handler)
{
}

QByteArray QFilesystemHandlerPrivate::mimeType(const QString &absolutePath)
{
    // Query the MIME database based on the filename and its contents
    return database.mimeTypeForFile(absolutePath).name().toUtf8();
}

void QFilesystemHandlerPrivate::processFile(QHttpSocket *socket, const QString &absolutePath)
{
    // Attempt to open the file for reading
    QFile *file = new QFile(absolutePath);

    if (!file->open(QIODevice::ReadOnly)) {
        delete file;

        socket->writeError(QHttpSocket::Forbidden);
        return;
    }

    // Create a QIODeviceCopier to copy the file contents to the socket
    QIODeviceCopier *copier = new QIODeviceCopier(file, socket);
    connect(copier, &QIODeviceCopier::finished, copier, &QIODeviceCopier::deleteLater);
    connect(copier, &QIODeviceCopier::finished, file, &QFile::deleteLater);
    connect(copier, &QIODeviceCopier::finished, [socket]() {
        socket->close();
    });

    qint64 fileSize = file->size();

    // Checking for partial content request
    QByteArray rangeHeader = socket->headers().value("Range");
    QHttpRange range;

    if (!rangeHeader.isEmpty() && rangeHeader.startsWith("bytes=")) {
        // Skiping 'bytes=' - first 6 chars and spliting ranges by comma
        QList<QByteArray> rangeList = rangeHeader.mid(6).split(',');

        // Taking only first range, as multiple ranges require multipart
        // reply support
        range = QHttpRange(QString(rangeList.at(0)), fileSize);
    }

    // If range is valid, send partial content
    if (range.isValid()) {
        socket->setStatusCode(QHttpSocket::PartialContent);
        socket->setHeader("Content-Length", QByteArray::number(range.length()));
        socket->setHeader("Content-Range", QByteArray("bytes ") + range.contentRange().toLatin1());
        copier->setRange(range.from(), range.to());
    } else {
        // If range is invalid or if it is not a partial content request,
        // send full file
        socket->setHeader("Content-Length", QByteArray::number(fileSize));
    }

    // Set the mimetype and content length
    socket->setHeader("Content-Type", mimeType(absolutePath));
    socket->writeHeaders();

    // Start the copy
    copier->start();
}

QFilesystemHandler::QFilesystemHandler(QObject *parent)
    : QHttpHandler(parent),
      d(new QFilesystemHandlerPrivate(this))
{
}

void QFilesystemHandler::process(QHttpSocket *socket, const QString &path)
{
    // Get filename of file
    QString fileName = QUrl(path).fileName();
    QUrl filePath = fileMap[fileName.split(".").first().toInt()];
    QFileInfo checkFile(filePath.toString());
    // Check, is file valid
    if(checkFile.suffix() != fileName.split(".").last() ||
            !checkFile.exists() ||
            !checkFile.isFile() ||
            !checkFile.isReadable() )
    {
        socket->writeError(QHttpSocket::NotFound);
        return;
    }
    d->processFile(socket, filePath.toString());

}
// Adds file to server stack
int QFilesystemHandler::serveFile(QUrl path)
{
    fileMap.insert(fileMapCounter, path);
    fileMapCounter++;
    return fileMapCounter - 1;
}
