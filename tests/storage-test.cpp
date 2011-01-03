/*
 * This file is part of telepathy-nepomuk-service
 *
 * Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
 *   @author George Goldberg <george.goldberg@collabora.co.uk>
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

#include "storage-test.h"

#include "test-backdoors.h"

#include <KDebug>

#include <qtest_kde.h>
#include </home/gberg/development/build/work/collabora/telepathy-kde/ontologies/person.h>

StorageTest::StorageTest(QObject *parent)
: Test(parent),
  m_storage(0)
{
    kDebug();
}

StorageTest::~StorageTest()
{
    kDebug();
}

void StorageTest::initTestCase()
{
    initTestCaseImpl();
}

void StorageTest::testConstructorDestructor()
{
    // First test constructing the NepomukStorage on a Nepomuk database with no relevant
    // data already in it.
    m_storage = new NepomukStorage(this);

    // Check that the Nepomuk mePersonContact has been created.
    QVERIFY(TestBackdoors::nepomukStorageMePersonContact(m_storage).exists());

    // Check that the PersonContact and IMAccount lists are empty
    QVERIFY(TestBackdoors::nepomukStorageAccounts(m_storage)->isEmpty());
    QVERIFY(TestBackdoors::nepomukStorageContacts(m_storage)->isEmpty());

    // Now destroy the NepomukStorage, running the event loop
    // to make sure the destruction is completed.
    connect(m_storage, SIGNAL(destroyed(QObject*)), mLoop, SLOT(quit()));
    m_storage->deleteLater();
    m_storage = 0;
    QCOMPARE(mLoop->exec(), 0);

    // Next, try constructing a NepomukStorage instance where the mePersonContact already exists.
    m_storage = new NepomukStorage(this);

    // Check that the Nepomuk mePersonContact has been created.
    QVERIFY(TestBackdoors::nepomukStorageMePersonContact(m_storage).exists());

    // Check that the PersonContact and IMAccount lists are empty
    QVERIFY(TestBackdoors::nepomukStorageAccounts(m_storage)->isEmpty());
    QVERIFY(TestBackdoors::nepomukStorageContacts(m_storage)->isEmpty());

    // Now destroy the NepomukStorage, running the event loop
    // to make sure the destruction is completed.
    connect(m_storage, SIGNAL(destroyed(QObject*)), mLoop, SLOT(quit()));
    m_storage->deleteLater();
    m_storage = 0;
    QCOMPARE(mLoop->exec(), 0);
}

void StorageTest::testCreateAccount()
{
    m_storage = new NepomukStorage(this);
    Nepomuk::PersonContact mePersonContact = TestBackdoors::nepomukStorageMePersonContact(m_storage);
    QHash<QString, Nepomuk::IMAccount> *accounts = TestBackdoors::nepomukStorageAccounts(m_storage);

    QVERIFY(m_storage);

    QVERIFY(mePersonContact.exists());
    QCOMPARE(mePersonContact.iMAccounts().size(), 0);

    QCOMPARE(TestBackdoors::nepomukStorageAccounts(m_storage)->size(), 0);
    QCOMPARE(TestBackdoors::nepomukStorageContacts(m_storage)->size(), 0);

    // Test creating an account which is not already in Nepomuk.
    // Issue the command to the storage
    m_storage->createAccount(QLatin1String("/foo/bar/baz"),
                             QLatin1String("foo@bar.baz"),
                             QLatin1String("test"));

    // Check the Account is created
    QCOMPARE(TestBackdoors::nepomukStorageAccounts(m_storage)->size(), 1);
    QCOMPARE(TestBackdoors::nepomukStorageContacts(m_storage)->size(), 0);

    // Check its properties are correct
    Nepomuk::IMAccount imAcc1 = accounts->value(QLatin1String("/foo/bar/baz"));
    QVERIFY(imAcc1.exists());
    QCOMPARE(imAcc1.imIDs().size(), 1);
    QCOMPARE(imAcc1.imIDs().first(), QLatin1String("foo@bar.baz"));
    QCOMPARE(imAcc1.accountIdentifiers().size(), 1);
    QCOMPARE(imAcc1.accountIdentifiers().first(), QLatin1String("/foo/bar/baz"));
    QCOMPARE(imAcc1.imAccountTypes().size(), 1);
    QCOMPARE(imAcc1.imAccountTypes().first(), QLatin1String("test"));

    // Check that it is correctly related to the mePersonContact.
    QCOMPARE(mePersonContact.iMAccounts().size(), 1);

    // Test creating an account which *is* already in Nepomuk.
    // Add the account to Nepomuk.
    Nepomuk::IMAccount imAcc2;
    imAcc2.setAccountIdentifiers(QStringList() << QLatin1String("/foo/bar/baz/bong"));
    imAcc2.setImIDs(QStringList() << QLatin1String("foo.bar@baz.bong"));
    imAcc2.setImAccountTypes(QStringList() << QLatin1String("test"));
    QVERIFY(imAcc2.exists());
    mePersonContact.addIMAccount(imAcc2);

    // Now tell the storage about that account.
    m_storage->createAccount(QLatin1String("/foo/bar/baz/bong"),
                             QLatin1String("foo.bar@baz.bong"),
                             QLatin1String("test"));

    // Check the account is found.
    QCOMPARE(TestBackdoors::nepomukStorageAccounts(m_storage)->size(), 2);
    QCOMPARE(TestBackdoors::nepomukStorageContacts(m_storage)->size(), 0);

    // Check its properties are correct
    Nepomuk::IMAccount imAcc3 = accounts->value(QLatin1String("/foo/bar/baz/bong"));
    QVERIFY(imAcc3.exists());
    QCOMPARE(imAcc3.imIDs().size(), 1);
    QCOMPARE(imAcc3.imIDs().first(), QLatin1String("foo.bar@baz.bong"));
    QCOMPARE(imAcc3.accountIdentifiers().size(), 1);
    QCOMPARE(imAcc3.accountIdentifiers().first(), QLatin1String("/foo/bar/baz/bong"));
    QCOMPARE(imAcc3.imAccountTypes().size(), 1);
    QCOMPARE(imAcc3.imAccountTypes().first(), QLatin1String("test"));
    QCOMPARE(imAcc2, imAcc3);

    // Check that it is correctly related to the mePersonContact.
    QCOMPARE(mePersonContact.iMAccounts().size(), 2);

    // Test creating an account twice (by recreating the first account we created).
    m_storage->createAccount(QLatin1String("/foo/bar/baz"),
                             QLatin1String("foo@bar.baz"),
                             QLatin1String("test"));

    // Check the Account is created
    QCOMPARE(TestBackdoors::nepomukStorageAccounts(m_storage)->size(), 2);
    QCOMPARE(TestBackdoors::nepomukStorageContacts(m_storage)->size(), 0);

    // Check its properties are correct
    Nepomuk::IMAccount imAcc4 = accounts->value(QLatin1String("/foo/bar/baz"));
    QVERIFY(imAcc4.exists());
    QCOMPARE(imAcc4.imIDs().size(), 1);
    QCOMPARE(imAcc4.imIDs().first(), QLatin1String("foo@bar.baz"));
    QCOMPARE(imAcc4.accountIdentifiers().size(), 1);
    QCOMPARE(imAcc4.accountIdentifiers().first(), QLatin1String("/foo/bar/baz"));
    QCOMPARE(imAcc4.imAccountTypes().size(), 1);
    QCOMPARE(imAcc4.imAccountTypes().first(), QLatin1String("test"));
    QCOMPARE(imAcc4, imAcc1);

    // Check that it is correctly related to the mePersonContact.
    QCOMPARE(mePersonContact.iMAccounts().size(), 2);

    // Question: Should we test for cases where there is data in Nepomuk that would
    // partially match the query but shouldn't actually match because it is not a complete
    // set of data wrt telepathy?
    // Answer: for now, I don't think there's any point - but if an error in the query is found in
    // the future, then of course we should add a test for that error here to avoid it
    // regressing in the future (grundleborg).
}

void StorageTest::cleanupTestCase()
{
    cleanupTestCaseImpl();

    // Clear re-used member variables.
    if (m_storage) {
        m_storage->deleteLater();
        m_storage = 0;
    }
}


QTEST_KDEMAIN(StorageTest, GUI)


#include "storage-test.moc"

