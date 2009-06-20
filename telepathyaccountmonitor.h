/*
 * This file is part of telepathy-integration-daemon
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef TELEPATHY_INTEGRATION_DAEMON_TELEPATHYACCOUNTMONITOR_H
#define TELEPATHY_INTEGRATION_DAEMON_TELEPATHYACCOUNTMONITOR_H

#include "telepathyaccount.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <TelepathyQt4/AccountManager>

namespace Tp {
    class PendingOperation;
}

class TelepathyAccountMonitor : public QObject
{
    Q_OBJECT

public:
    explicit TelepathyAccountMonitor(QObject *parent = 0);
    ~TelepathyAccountMonitor();

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void onAccountCreated(const QString &path);
    void onAccountRemoved(const QString &path);

private:
    Tp::AccountManagerPtr m_accountManager;
    QMap<QString, TelepathyAccount*> m_accounts;
};


#endif // Header guard

