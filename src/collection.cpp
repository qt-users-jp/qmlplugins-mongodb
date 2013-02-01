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

#include "collection.h"
#include "database.h"
#include "query.h"
#include "qbson.h"

#include <QtCore/QDebug>

#include <mongo.h>

class Collection::Private
{
public:
    Private();
};

Collection::Private::Private()
{}

Collection::Collection(QObject *parent)
    : QObject(parent)
    , d(new Private)
    , m_database(0)
{
}

Collection::~Collection()
{
    delete d;
}

void Collection::classBegin()
{
}

void Collection::componentComplete()
{
    foreach (Query *model, findChildren<Query*>()) {
        model->read();
    }
}

Query *Collection::find(const QVariantMap &query, const QVariantMap &fields)
{
    if (m_database == 0) database(qobject_cast<Database *>(parent()));

    Query *ret = new Query(this);
    ret->__collection(this);
    ret->__query(query);
    ret->__fields(fields);
    return ret;
}

QVariantMap Collection::insert(const QVariantMap &json)
{
    qDebug() << Q_FUNC_INFO << __LINE__ << json;
    QVariantMap ret = json;
    QByteArray ns = QString("%1.%2").arg(m_database->name()).arg(m_name).toUtf8();

    mongo_write_concern mongo_wc[1];
    mongo_write_concern_init(mongo_wc);
    mongo_wc->w = 1;
    mongo_write_concern_finish(mongo_wc);

    bson b[1];
    bson_init(b);
    object2bson(json, b);
    bson_finish(b);

    int status = mongo_insert(m_database->connection(), ns.constData(), b, mongo_wc);
    if (status != MONGO_OK) {
        switch (m_database->connection()->err) {
        case MONGO_CONN_NO_SOCKET:
            break;
        default:
            break;
        }
        qWarning() << Q_FUNC_INFO << __LINE__ << m_database->connection()->err;
        qWarning() << Q_FUNC_INFO << __LINE__ << QString::fromUtf8(m_database->connection()->errstr);
    }

    bson_destroy(b);
    mongo_write_concern_destroy(mongo_wc);

    qDebug() << Q_FUNC_INFO << __LINE__;
    return ret;
}
