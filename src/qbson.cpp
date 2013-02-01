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

#include "qbson.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QRegExp>

#include <bson.h>

bool value2bson(const QVariant &from, bson *to, const char *key)
{
    bool ret = true;
    switch (from.type()) {
    case QVariant::Invalid:
        bson_append_null(to, key);
        break;
    case QVariant::Bool:
        bson_append_bool(to, key, from.toBool());
        break;
    case QVariant::Int:
        bson_append_int(to, key, from.toInt());
        break;
    case QVariant::Double:
        bson_append_double(to, key, from.toDouble());
        break;
    case QVariant::List: {
        bson_append_start_array(to, key);
        array2bson(from.toList(), to);
        bson_append_finish_array(to);
        break; }
    case QVariant::String: {
        QByteArray val = from.toString().toUtf8();
        bson_append_string(to, key, val.constData());
        break; }
    case QVariant::Map: {
        bson_append_start_object(to, key);
        object2bson(from.toMap(), to);
        bson_append_finish_object(to);
        break; }
    default:
        qWarning() << Q_FUNC_INFO << __LINE__ << from << "not supported";
        break;
    }    return ret;
}

bool object2bson(const QVariantMap &from, bson *to)
{
    bool ret = true;
    QVariantMap::const_iterator i = from.constBegin();
    while (i != from.constEnd()) {
        QByteArray key = i.key().toUtf8();
        value2bson(i.value(), to, key.constData());
        ++i;
    }

    return ret;
}

bool array2bson(const QVariantList &from, bson *to)
{
    bool ret = true;
    int index = 0;
    foreach (const QVariant &v, from) {
        value2bson(v, to, QByteArray::number(index).constData());
        index++;
    }
    return ret;
}

QVariant bson2value(const bson_iterator *from)
{
    QVariant ret;

    bson_type type = bson_iterator_type(from);
    switch (type) {
    case BSON_EOO:
        break;
    case BSON_DOUBLE:
        ret = bson_iterator_double(from);
        break;
    case BSON_STRING:
        ret = QString::fromUtf8(bson_iterator_string(from));
        break;
    case BSON_OBJECT: {
        bson sub[1];
        bson_iterator_subobject(from, sub);
        QVariantMap object;
        bson2object(sub, &object);
        ret = object;
        break; }
    case BSON_ARRAY: {
        bson sub[1];
        bson_iterator_subobject(from, sub);
        QVariantList array;
        bson2array(sub, &array);
        ret = array;
        break; }
    case BSON_BINDATA:
        break;
    case BSON_UNDEFINED:
        break;
    case BSON_OID: {
        char oid[25];
        bson_oid_to_string(bson_iterator_oid(from), oid);
        ret = QString::fromLatin1(oid);
        break; }
    case BSON_BOOL:
        ret = bson_iterator_bool(from);
        break;
    case BSON_DATE:
        ret = QDateTime::fromTime_t(bson_iterator_date(from));
        break;
    case BSON_NULL:
        break;
    case BSON_REGEX:
        ret = QRegExp(bson_iterator_regex(from));
        break;
    case BSON_CODE:
        ret = bson_iterator_code(from);
        break;
    case BSON_INT:
        ret = bson_iterator_int(from);
        break;
    default:
        qDebug() << Q_FUNC_INFO << __LINE__ << type;
        break;
    }
    return ret;
}

bool bson2object(const bson *from, QVariantMap *to)
{
    bool ret = true;
    bson_iterator iterator[1];
    bson_iterator_init(iterator, from);
    if (bson_iterator_more(iterator)) {
        forever {
            bson_type type = bson_iterator_next(iterator);
            if (type == BSON_EOO) break;
            const char *key = bson_iterator_key(iterator);
            to->insert(key, bson2value(iterator));
        }
    }
    return ret;
}

bool bson2array(const bson *from, QVariantList *to)
{
    bool ret = true;
    bson_iterator iterator[1];
    bson_iterator_init(iterator, from);
    if (bson_iterator_more(iterator)) {
        forever {
            bson_type type = bson_iterator_next(iterator);
            if (type == BSON_EOO) break;
            QVariant value = bson2value(iterator);
            to->append(value);
        }
    }
    return ret;
}
