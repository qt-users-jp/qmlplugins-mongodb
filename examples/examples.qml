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

import QtQuick 2.0
import me.qtquick.MongoDB 0.1

Rectangle {
    id: root
    width: 360
    height: 360

    Database {
        id: db
        host: '127.0.0.1'
        port: 27017
        name: 'test'

        property Collection test: Collection { name: 'test' }
    }

    TextInput {
        id: text
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        Keys.onReturnPressed: {
            db.test.insert(JSON.parse(text.text))
            text.text = ''
        }

        Text {
            id: placeholder
            text: qsTr('enter {"key": "value"} here then press return key')
            font: parent.font
            color: Qt.lighter(parent.color)
            opacity: 0.0

            states: State {
                when: text.text.length === 0 && !text.focus
                PropertyChanges {
                    target: placeholder
                    opacity: 0.75
                }
            }

            transitions: Transition {
                NumberAnimation { property: 'opacity' }
            }
        }
    }

    ListView {
        anchors.top: text.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true

//        model: db.test.find({x: {$exists: 1}, j: {$exists: 1}}).skip(5).limit(5).sort({j: -1})
        model: db.test.find()

        delegate: Text {
            text: JSON.stringify(model.modelData)
        }
    }
}
