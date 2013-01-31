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

#ifndef COLLECTION_H
#define COLLECTION_H

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QVariantMap>
#include <QtQml/QQmlParserStatus>

class Database;
class Query;

class Collection : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(Database *database READ database WRITE database NOTIFY databaseChanged)
    Q_PROPERTY(QString name READ name WRITE name NOTIFY nameChanged)

    Q_INTERFACES(QQmlParserStatus)
public:
    explicit Collection(QObject *parent = 0);
    ~Collection();

    virtual void classBegin();
    virtual void componentComplete();

    Q_INVOKABLE Query *find(const QVariantMap &query = QVariantMap(), const QVariantMap &fields = QVariantMap());
    Q_INVOKABLE QJsonObject insert(const QJsonObject &json);

signals:
    void databaseChanged(Database *database);
    void nameChanged(const QString &name);

private:
    class Private;
    Private *d;

    Q_DISABLE_COPY(Collection)

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

    ADD_PROPERTY(Database *, database, Database *)
    ADD_PROPERTY(const QString &, name, QString)
};

#endif // COLLECTION_H
