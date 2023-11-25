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
    width: Screen.width / 2.5
    height: width / 1.5

    property var displayView: controlPanel.displayState

//An initialization block that is called once the component is fully constructed and ready.
    Component.onCompleted: {
        changeMainLayout()
    }

    ControlPanel {
        id: controlPanel
        //Anchors are a way to define the relative positioning of components within their parent components.
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.bottom: parent.bottom

        /*
        This is a signal handler for the ControlPanel. When the displayState changes in ControlPanel,
        it triggers the parent.changeMainLayout() function to adjust the layout accordingly.
        The naming convention for these handlers is on<PropertyName>Changed,
        where <PropertyName> is the name of the property with the first letter capitalized.
        */

        onDisplayStateChanged: { //automatically connected to the displayStateChanged signal of the ControlPanel component.
            parent.changeMainLayout() //when this property changes, the displayStateChanged signal is emitted.
        }
    }

    ECGView {
        id: ecgView
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: controlPanel.right
        visible: parent.displayView[0] //The visible property of ECGView is bound to the displayView property of the main item.
    }

    HRView {
        id: hrView
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: controlPanel.right
        visible: parent.displayView[1]
    }


    function changeMainLayout() {
        main.displayView = controlPanel.displayState

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
