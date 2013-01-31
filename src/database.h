/* Copyright (c) 2012 Silk Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Silk nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SILK BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore/QObject>

class mongo;

class Database : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE host NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE port NOTIFY portChanged)
    Q_PROPERTY(QString name READ name WRITE name NOTIFY nameChanged)
    Q_PROPERTY(QString user READ user WRITE user NOTIFY userChanged)
    Q_PROPERTY(QString pass READ pass WRITE pass NOTIFY passChanged)
    Q_PROPERTY(bool open READ open NOTIFY openChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)

public:
    explicit Database(QObject *parent = 0);
    ~Database();

    bool open();
    bool authenticated();

    mongo *connection();

public slots:
    void open(bool open);
    void authenticated(bool authenticated);

signals:
    void hostChanged(const QString &host);
    void portChanged(int port);
    void nameChanged(const QString &name);
    void userChanged(const QString &user);
    void passChanged(const QString &pass);
    void openChanged(bool open);
    void authenticatedChanged(bool authenticated);

private:
    class Private;
    Private *d;

    Q_DISABLE_COPY(Database)

#define ADD_PROPERTY(type, name, type2) \
public: \
    type name() const { return m_##name; } \
    void name(type name) { \
        if (m_##name == name) return; \
        m_##name = name; \
        emit name##Changed(name); \
    } \
private: \
    type2 m_##name;

    ADD_PROPERTY(const QString &, host, QString)
    ADD_PROPERTY(int, port, int)
    ADD_PROPERTY(const QString &, name, QString)
    ADD_PROPERTY(const QString &, user, QString)
    ADD_PROPERTY(const QString &, pass, QString)
};

#endif // DATABASE_H
