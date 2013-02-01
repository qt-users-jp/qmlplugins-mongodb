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

#include "query.h"
#include "collection.h"
#include "database.h"
#include "qbson.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include "mongo.h"

class Query::Private
{
public:
    enum {
        QueryDataRole = Qt::UserRole,
        QueryUserRole
    };
    QList<QVariantMap> data;
    QHash<int, QByteArray> roleNames;
};



Query::Query(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
    , m___collection(qobject_cast<Collection *>(parent))
    , m___skip(0)
    , m___limit(0)
{
}

Query::~Query()
{
    delete d;
}

Query *Query::sort(const QVariantMap &sort)
{
    __sort(sort);
    return this;
}

Query *Query::skip(int n)
{
    __skip(n);
    return this;
}

Query *Query::limit(int n)
{
    __limit(n);
    return this;
}

void Query::read()
{
    Database *db = m___collection->database();

    if (!db->open()) {
        qDebug() << Q_FUNC_INFO << __LINE__;
        return;
    }
    if (!db->authenticated()) {
        qDebug() << Q_FUNC_INFO << __LINE__;
        return;
    }

    if (db->name().isEmpty()) {
        qWarning() << Q_FUNC_INFO << __LINE__ << "Database.name is empty.";
        return;
    }

    if (m___collection->name().isEmpty()) {
        qWarning() << Q_FUNC_INFO << __LINE__ << "Collection.name is empty.";
        return;
    }

    mongo *conn = db->connection();

    QByteArray ns = QString("%1.%2").arg(db->name()).arg(m___collection->name()).toUtf8();

    mongo_cursor cursor[1];
    mongo_cursor_init(cursor, conn, ns.constData());

    QVariantMap query;
    query.insert(QLatin1String("$query"), m___query);
    query.insert(QLatin1String("$orderby"), m___sort);
    bson mongo_query[1];
    bson_init(mongo_query);
    object2bson(query, mongo_query);
    bson_finish(mongo_query);
    mongo_cursor_set_query(cursor, mongo_query);

    bson mongo_fields[1];
    bson_init(mongo_fields);
    object2bson(m___fields, mongo_fields);
    bson_finish(mongo_fields);
    mongo_cursor_set_fields(cursor, mongo_fields);

    mongo_cursor_set_skip(cursor, m___skip);
    mongo_cursor_set_limit(cursor, m___limit);

    d->roleNames.insert(Private::QueryDataRole, "modelData");
    int role = Private::QueryUserRole;
    QStringList keys;
    while (mongo_cursor_next(cursor) == MONGO_OK) {
        QVariantMap object;
        bson2object(&cursor->current, &object);
        foreach (const QString &key, object.keys()) {
            if (!keys.contains(key)) {
                keys.append(key);
                d->roleNames.insert(role++, key.toUtf8());
            }
        }

        d->data.append(object);
    }

    if (!d->data.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, d->data.count() - 1);
        endInsertRows();
    }
    mongo_cursor_destroy(cursor);
}

int Query::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d->data.count();
}

QVariant Query::data(const QModelIndex &index, int role) const
{
    if (role == Private::QueryDataRole) {
        return d->data.at(index.row());
    } else if (role > Private::QueryUserRole) {
//        QVariantMap object = d->data.at(index.row());
//        QJsonValue value = object.value(d->roleNames.value(role));
//        return value.toVariant();
        return d->data.at(index.row()).value(d->roleNames.value(role));
    }
    return QVariant();
}

QHash<int, QByteArray> Query::roleNames() const
{
    return d->roleNames;
}
