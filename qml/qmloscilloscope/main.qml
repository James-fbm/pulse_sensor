/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.15

Item {
    id: main
    width: Screen.width / 2
    height: width / 2

    property var displayView: controlPanel.displayState

    Component.onCompleted: {
        changeMainLayout()
    }

    ControlPanel {
        id: controlPanel
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom

        onDisplayStateChanged: {
            parent.displayView = displayState
            parent.changeMainLayout()
        }
    }


    ECGView {
        id: ecgView
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: controlPanel.right
        visible: parent.displayView[0] // 直接绑定
    }

    HRView {
        id: hrView
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: controlPanel.right
        visible: parent.displayView[1] // 直接绑定
    }


    function changeMainLayout() {
        if (ecgView.visible || hrView.visible) {
            controlPanel.anchors.horizontalCenter = undefined
            controlPanel.anchors.left = main.left
            controlPanel.anchors.leftMargin = 10

            if (ecgView.visible && !hrView.visible) {
                ecgView.anchors.bottom = main.bottom
            } else if (!ecgView.visible && hrView.visible) {
                hrView.anchors.top = main.top
            } else  {
                ecgView.anchors.bottom = main.verticalCenter
                hrView.anchors.top = main.verticalCenter
            }

        } else {
            // these three commands should be put strictly in this sequence.
            controlPanel.anchors.left = undefined
            controlPanel.anchors.leftMargin = undefined
            controlPanel.anchors.horizontalCenter = main.horizontalCenter
        }
    }
}
